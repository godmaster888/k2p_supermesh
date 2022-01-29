/*
 * The genealogy:
 * David Lamparter <equinox@diac24.net>
 * -> Amine Kherbouche <amine.kherbouche@6wind.com>
 * --> Alexander Nikulin <anikulin@dlink.ru>
 *
 * Copyrights: SPDX.
 */

#include <linux/netdevice.h>
#include <linux/slab.h>
#include <linux/etherdevice.h>
#include <linux/u64_stats_sync.h>
#include <linux/module.h>
#include <linux/mpls.h>
#include <linux/version.h>
#include <linux/if_tunnel.h>	/* struct pcpu_tstats */
#include <linux/if_vlan.h>	/* sizeof(struct vlan_hdr) */
#include <linux/if_ether.h>	/* sizeof(struct eth_hdr) */

#include <net/rtnetlink.h>
#include <net/dst.h>
#include <net/mpls.h>

#if IS_ENABLED(CONFIG_IPV6)
#include <net/ipv6.h>
#endif

#if IS_ENABLED(CONFIG_VPLS_CRYPTO)
#include <linux/crypto.h>
#include <linux/string.h>	/* kmemdup */
#include <linux/capability.h>
#endif

#define DRV_NAME               "vpls"
#define DRV_VERSION            "0.2"
#define VPLS_DEFAULT_TTL       255
#define VPLS_DEFAULT_MTU       (ETH_DATA_LEN -			\
                                /* sizeof(struct vlan_hdr) - */	\
                                sizeof(struct ethhdr) -		\
                                2 * sizeof(struct mpls_shim_hdr))

struct vpls_priv {
	struct net	*encap_net;

	//read: RCU and RTNL
	//write: RTNL
	u32	input_label;
	u32	output_label;
	u32	transport_label;
	u8	ttl;
	u32	domain;

	struct notifier_block nb;

	/* AF_INET or AF_INET6 */
	u8	rt_family;
	union {
		unsigned long rmt_pe_addr;
		struct in_addr  ipv4_addr;
		struct in6_addr ipv6_addr;
	};

#if IS_ENABLED(CONFIG_VPLS_CRYPTO)
	struct vpls_crypto_ctx {
		struct rcu_head		ctx_rcu;
		struct crypto_cipher	*ctx_tfm;
		u8			*ctx_key;
		unsigned int		ctx_keylen;
	} __rcu *cra_ctx;
#endif
};

#define VPLS_CB_MAGIC	0x76706c73
struct vpls_cb {
	u32 domain;
	u32 magic;
};
#define VPLSCB(skb) ((struct vpls_cb*) \
                     (skb->cb + sizeof(skb->cb) - sizeof(struct vpls_cb)))

static inline
void vpls_set_cb_rcu(struct sk_buff *skb, const struct vpls_priv *priv)
{
	struct vpls_cb *v = VPLSCB(skb);
	v->magic = VPLS_CB_MAGIC;
	v->domain = READ_ONCE(priv->domain);
}

static inline
bool vpls_must_split_rcu(struct sk_buff *skb, const struct vpls_priv *priv)
{
	struct vpls_cb *v = VPLSCB(skb);
	return (v->magic == VPLS_CB_MAGIC) &&
	       (v->domain == READ_ONCE(priv->domain));
}

#if IS_ENABLED(CONFIG_VPLS_CRYPTO)
static struct nla_policy vpls_crypto_policy[IFLA_VPLS_CRYPTO_MAX + 1] = {
	[IFLA_VPLS_CRYPTO_ALGO]		= { .type = NLA_NUL_STRING,
					    .len = CRYPTO_MAX_ALG_NAME - 1 },
	[IFLA_VPLS_CRYPTO_KEYSIZE]	= { .type = NLA_U32    },
	[IFLA_VPLS_CRYPTO_KEY]		= { .type = NLA_BINARY },
};

static int vpls_crypto_validate(const struct nlattr *attr/*,
				struct netlink_ext_ack *extack*/);

static int vpls_crypto_configure(struct vpls_priv *priv,
				 const struct nlattr *crattr/*,
				 struct netlink_ext_ack *extack*/);

#else

static inline int vpls_crypto_validate(const struct nlattr *attr/*,
				       struct netlink_ext_ack *extack*/)
{
	//NL_SET_ERR_MSG(extack, "Crypto support not enabled in kernel");
	return -EOPNOTSUPP;
}
static inline int vpls_crypto_configure(struct vpls_priv *priv,
					const struct nlattr *crattr/*,
					struct netlink_ext_ack *extack*/)
{
	//NL_SET_ERR_MSG(extack, "Crypto support not enabled in kernel");
	return -EOPNOTSUPP;
}

#endif

static struct nla_policy vpls_policy[IFLA_VPLS_MAX + 1] = {
	[IFLA_VPLS_IN_LABEL]    = { .type = NLA_U32 },
	[IFLA_VPLS_OUT_LABEL]   = { .type = NLA_U32 },
	[IFLA_VPLS_TTL]         = { .type = NLA_U8  },
	[IFLA_VPLS_NH]          = { .type = NLA_U32 },
	[IFLA_VPLS_NH6]         = { .len = sizeof(struct in6_addr) },
	[IFLA_VPLS_DOMAIN]      = { .type = NLA_U32 },
	/* TODO v3.10.108 has poor struct nla_policy
	 * w/o validation fields. Thus call checking in
	 * vpls_validate() and vpls_dev_configure().
	 * Change to modern API if ported forward.
	 */
	[IFLA_VPLS_CRYPTO]      = { .type = NLA_NESTED },
};

static int vpls_attach_dev(struct net_device *dev/*,
			   struct netlink_ext_ack *extack*/);
static int vpls_detach_dev(struct net_device *dev/*,
			   struct netlink_ext_ack *extack*/);

static inline int skb_align(struct sk_buff *skb, unsigned align)
{
	unsigned int size = skb->len;
	unsigned int x = size % align;

	if (unlikely(!x))
		return 0; /* already aligned */

	x = align - x; /* amount to add, positive */
	if (__skb_pad(skb, x, false))
		return -ENOMEM;

	skb->len = size + x;
	//skb_set_tail_pointer(skb, skb->len);
	return (int)x;
}

static inline int skb_is_aligned(struct sk_buff *skb, unsigned align)
{
	return !(skb->len % align);
}

#if IS_ENABLED(CONFIG_VPLS_CRYPTO)
static int vpls_encrypt(struct sk_buff **pskb,
			const struct vpls_crypto_ctx *ctx)
{
	int ret;
	unsigned i, bs;
	struct sk_buff *skb;

	if (!ctx)
		return 0;

	skb = skb_unshare(*pskb, GFP_ATOMIC);
	*pskb = skb;
	if (unlikely(!skb))
		return -ENOMEM;

	ret = skb_linearize(skb);
	if (unlikely(ret))
		return ret;

	/* complete csum'ing before encrypt */
	if (skb->ip_summed == CHECKSUM_PARTIAL) {
		ret = skb_checksum_help(skb);
		if (unlikely(ret))
			return ret;
	}

	bs = crypto_cipher_blocksize(ctx->ctx_tfm);

	if (bs == 1) {
		/* reset stream cipher any frame */
		ret = crypto_cipher_setkey(ctx->ctx_tfm, ctx->ctx_key,
					   ctx->ctx_keylen);
	} else
		ret = skb_align(skb, bs);

	if (unlikely(ret < 0))
		return ret;

	for (i = 0; i < skb->len; i += bs)
		crypto_cipher_encrypt_one(ctx->ctx_tfm,
					  skb->data + i,
					  skb->data + i);
	return 0;
}

static int vpls_decrypt(struct sk_buff **pskb,
			const struct vpls_crypto_ctx *ctx)
{
	int ret;
	unsigned i, bs;
	struct sk_buff *skb;

	if (!ctx)
		return 0;

	skb = skb_unshare(*pskb, GFP_ATOMIC);
	*pskb = skb;
	if (unlikely(!skb))
		return -ENOMEM;

	ret = skb_linearize(skb);
	if (unlikely(ret))
		return ret;

	bs = crypto_cipher_blocksize(ctx->ctx_tfm);

	if (bs == 1) {
		/* reset stream cipher any frame */
		ret = crypto_cipher_setkey(ctx->ctx_tfm, ctx->ctx_key,
					   ctx->ctx_keylen);
		if (unlikely(ret))
			return ret;
	} else if (!skb_is_aligned(skb, bs))
		return -EBADMSG;

	for (i = 0; i < skb->len; i += bs)
		crypto_cipher_decrypt_one(ctx->ctx_tfm,
					  skb->data + i,
					  skb->data + i);
	return 0;
}
#endif


static bool do_netif_rx = true;
module_param(do_netif_rx, bool, 0644);
MODULE_PARM_DESC(do_netif_rx, "Use netif_rx(1) or neigh_xmit(0)");

static netdev_tx_t vpls_xmit(struct sk_buff *skb, struct net_device *dev)
{
#if 0
	/* this READ_ONCE makes barrier() around __builtin_memcpy()
	 * I dont like it. */
	struct vpls_priv priv = READ_ONCE(*(struct vpls_priv *)netdev_priv(dev));
#else
	struct vpls_priv *priv = netdev_priv(dev);
	u32	input_label = READ_ONCE(priv->input_label);
	u32	output_label = READ_ONCE(priv->output_label);
	u32	transport_label = READ_ONCE(priv->transport_label);
	u8	ttl = READ_ONCE(priv->ttl);
#endif
	//struct pcpu_sw_netstats *stats;
	struct pcpu_tstats *stats;
	unsigned int new_header_size;
	struct net_device * const out_dev = priv->encap_net->loopback_dev;
	struct mpls_shim_hdr *hdr;
	unsigned int hh_len;
	int err;

	stats = this_cpu_ptr(dev->tstats);

	if (unlikely(!mpls_output_possible(out_dev))) {
		// lo is down!!
		goto drop;
	}

	if (unlikely(!input_label || !output_label ||
		     !transport_label)) {
		/* carrier has gone down */
		dev->stats.tx_carrier_errors++;
		goto drop;
	}

	if (vpls_must_split_rcu(skb, priv)) {
		/* split horizon, w/o stats++ */
		kfree_skb(skb);
		return NET_XMIT_DROP;
	}

	/* Skip skb checks (lro, share, ...)
	 * Let mpls_forward() do it.
	 * See for encap room only.
	 * if (skb_warn_if_lro(skb)) {
	 *	goto drop;
	 *}
	 */

	skb = skb_share_check(skb, GFP_ATOMIC);
	if (unlikely(!skb))
		goto end;

	skb_orphan(skb);
	//skb_forward_csum(skb); mpls_forward() does it

	new_header_size = 2 * sizeof(struct mpls_shim_hdr);


	if (do_netif_rx)
		hh_len = 0;
	else if (!out_dev->header_ops)
		hh_len = 0;
	else
		hh_len = LL_RESERVED_SPACE(out_dev);

	if (skb_cow(skb, hh_len + new_header_size)) {
		goto drop;
	}

#if IS_ENABLED(CONFIG_VPLS_CRYPTO)
	if (vpls_encrypt(&skb, rcu_dereference(priv->cra_ctx)))
		goto drop;
#endif

	skb_push(skb, new_header_size);
	skb_reset_network_header(skb); /* for mpls_hdr() */
	hdr = mpls_hdr(skb);

	/* TODO: different/inherited TTL's */
	hdr[0] = mpls_entry_encode(transport_label, ttl, 0, false);
	hdr[1] = mpls_entry_encode(output_label,    ttl, 0, true);

	skb->dev = out_dev; /* lo */
	skb->protocol = htons(ETH_P_MPLS_UC);
	skb->pkt_type = PACKET_OUTGOING; /* see mpls_forward() */

	/* rx on lo in mpls_forward() */
	if (do_netif_rx)
		err = netif_rx(skb);
	else
		err = neigh_xmit(NEIGH_LINK_TABLE, out_dev, out_dev->dev_addr, skb);

	if (err) {
		net_dbg_ratelimited("%s: packet transmission failed: %d\n",
				    __func__, err);
		goto end;
	}

	u64_stats_update_begin(&stats->syncp);
	stats->tx_packets++;
	stats->tx_bytes += skb->len - new_header_size; /* w/o MPLS */
	u64_stats_update_end(&stats->syncp);

	return NETDEV_TX_OK;
drop:
	if (likely(skb))
		kfree_skb(skb);
end:
	dev->stats.tx_errors++;
	return NET_XMIT_DROP;
}

static int vpls_rcv(void *arg, struct sk_buff *skb, struct net_device *in_dev,
                    struct mpls_entry_decoded *dec)
{
	//struct pcpu_sw_netstats *stats;
	struct pcpu_tstats *stats;
	struct net_device *dev = arg;
	struct vpls_priv *priv = netdev_priv(dev);
	int ret = NOTIFY_BAD;

	stats = this_cpu_ptr(dev->tstats);


	if (!dec->bos)
		return NOTIFY_DONE;

	/* substack should deviver a correct label */
	if (unlikely(dec->label != READ_ONCE(priv->input_label))) {
		/* label changed while receiving? */
		net_warn_ratelimited("%s: unexpected input %u on %s->%s\n",
				     __func__, dec->label,
				     netdev_name(in_dev), netdev_name(dev));

		goto drop;
	}

	/* TODO check in mpls_forward() before passing here */
	if (unlikely(dec->ttl <= 1))
		goto drop;

	/* skb_share_check() was performed in mpls_forward() */

	/* Pop the service label */
	skb_pull(skb, sizeof(struct mpls_shim_hdr));

	/* this frame is definitely for us. Exclusively. */

	if (unlikely(!pskb_may_pull(skb, ETH_HLEN)))
		goto drop;

#if IS_ENABLED(CONFIG_VPLS_CRYPTO)
	if (vpls_decrypt(&skb, rcu_dereference(priv->cra_ctx)))
		goto drop;
#endif

	skb->dev = dev;
	skb_reset_mac_header(skb);
	skb->protocol = eth_type_trans(skb, dev);
	skb->ip_summed = CHECKSUM_NONE;
	skb->pkt_type = PACKET_HOST;
	skb_clear_hash(skb);
	skb->vlan_tci = 0;
	skb_set_queue_mapping(skb, 0);
	skb_scrub_packet(skb, !net_eq(dev_net(in_dev), dev_net(dev)));
	skb_reset_network_header(skb);
	skb_probe_transport_header(skb, 0);

	vpls_set_cb_rcu(skb, priv);

	if (netif_rx(skb) == NET_RX_SUCCESS) {
		u64_stats_update_begin(&stats->syncp);
		stats->rx_packets++;
		stats->rx_bytes += skb->len;
		u64_stats_update_end(&stats->syncp);

		return NOTIFY_STOP;
	}

	ret = NOTIFY_STOP;
drop:
	dev->stats.rx_errors++;
	if (likely(skb))
		kfree_skb(skb);
	return ret;
}

/* Stub, nothing needs to be done. */
static void vpls_set_multicast_list(struct net_device *dev)
{
}

static int vpls_open(struct net_device *dev)
{
	return vpls_attach_dev(dev/*, extack/NULL */);
}

static int vpls_close(struct net_device *dev)
{
	return vpls_detach_dev(dev/*, extack/NULL */);
}

static struct rtnl_link_stats64 *
vpls_dev_get_stats64(struct net_device *dev,
                     struct rtnl_link_stats64 *stats)
{
	u64 rx_packets, rx_bytes, tx_packets, tx_bytes;
	//const struct pcpu_sw_netstats *tstats;
	const struct pcpu_tstats *tstats;
	u64 rx_errors = 0, tx_errors = 0;
	unsigned int start;
	int i;

	if (!dev->tstats)
		return stats;

	for_each_possible_cpu(i) {
		tstats = per_cpu_ptr(dev->tstats, i);

		do {
			start = u64_stats_fetch_begin_irq(&tstats->syncp);
			rx_packets = tstats->rx_packets;
			tx_packets = tstats->tx_packets;
			rx_bytes = tstats->rx_bytes;
			tx_bytes = tstats->tx_bytes;
		} while (u64_stats_fetch_retry_irq(&tstats->syncp, start));

		stats->rx_packets += rx_packets;
		stats->tx_packets += tx_packets;
		stats->rx_bytes += rx_bytes;
		stats->tx_bytes += tx_bytes;

		rx_errors += dev->stats.rx_errors;
		tx_errors += dev->stats.tx_errors;
	}

	stats->rx_dropped = rx_errors;
	stats->tx_dropped = tx_errors;
	stats->rx_errors = rx_errors;
	stats->tx_errors = tx_errors;

	return stats;
}

static int is_valid_vpls_mtu(int new_mtu)
{
	return new_mtu >= ETH_MIN_MTU && new_mtu <= ETH_MAX_MTU;
}

static int vpls_change_mtu(struct net_device *dev, int new_mtu)
{
	if (!is_valid_vpls_mtu(new_mtu))
		return -EINVAL;

	dev->mtu = new_mtu;
	return 0;
}

static int vpls_dev_init(struct net_device *dev)
{
	//dev->tstats = netdev_alloc_pcpu_stats(struct pcpu_sw_netstats);
	dev->tstats = alloc_percpu(struct pcpu_tstats);

	if (!dev->tstats)
		return -ENOMEM;

	return 0;
}

static void vpls_dev_uninit(struct net_device *dev)
{
	if (dev->tstats)
		free_percpu(dev->tstats);
}

static const struct net_device_ops vpls_netdev_ops = {
	.ndo_set_mac_address    = eth_mac_addr,
#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 18, 0)
//v3.18-11803-g5f35227ea34b
//v4.0-rc5-1167-ge38f30256b36
	.ndo_features_check     = passthru_features_check,
#endif
	.ndo_set_rx_mode        = vpls_set_multicast_list,
	.ndo_get_stats64        = vpls_dev_get_stats64,
	.ndo_start_xmit         = vpls_xmit,
	.ndo_change_mtu         = vpls_change_mtu,
	.ndo_init               = vpls_dev_init,
	.ndo_uninit		= vpls_dev_uninit,
	.ndo_open               = vpls_open,
	.ndo_stop               = vpls_close,
};

#define VPLS_FEATURES (NETIF_F_SG | NETIF_F_FRAGLIST | \
                      NETIF_F_HW_CSUM | NETIF_F_RXCSUM | NETIF_F_HIGHDMA)

static void vpls_setup(struct net_device *dev)
{
	ether_setup(dev);

	dev->priv_flags &= ~IFF_TX_SKB_SHARING;
	dev->priv_flags |= IFF_LIVE_ADDR_CHANGE;
	dev->priv_flags |= IFF_NO_QUEUE;

	dev->netdev_ops = &vpls_netdev_ops;
	dev->features |= NETIF_F_LLTX;
	dev->features |= VPLS_FEATURES;
	dev->vlan_features = dev->features;
	dev->hw_features = VPLS_FEATURES;
	dev->hw_enc_features = VPLS_FEATURES;

	//dev->needs_free_netdev = true;
	dev->destructor = free_netdev;
}

#if IS_ENABLED(CONFIG_VPLS_CRYPTO)
static int vpls_crypto_validate(const struct nlattr *attr/*,
				struct netlink_ext_ack *extack*/)
{
	struct nlattr *a[IFLA_VPLS_CRYPTO_MAX + 1];
	int err;
	const char *algname;

	err = nla_parse_nested(a, IFLA_VPLS_CRYPTO_MAX, attr,
			       vpls_crypto_policy/*, extack*/);
	if (err)
		return err;

	/* empty nested - drop crypto */
	if (nla_len(attr) == 0)
		return 0;

	if (!a[IFLA_VPLS_CRYPTO_ALGO] ||
	    !a[IFLA_VPLS_CRYPTO_KEYSIZE] ||
	    !a[IFLA_VPLS_CRYPTO_KEY]) {
		//NL_SET_ERR_MSG(extack, "vpls crypto: all params must be");
		return -EINVAL;
	}

	algname = nla_data(a[IFLA_VPLS_CRYPTO_ALGO]);
	if (!crypto_has_cipher(algname, 0, 0)) {
		//NL_SET_ERR_MSG(extack, "no such cipher avail");
		return -EINVAL;
	}

	return 0;
}
#endif

static int vpls_validate(struct nlattr *tb[], struct nlattr *data[]/*,
                        struct netlink_ext_ack *extack*/)
{
	if (tb[IFLA_ADDRESS]) {
		if (nla_len(tb[IFLA_ADDRESS]) != ETH_ALEN) {
			//NL_SET_ERR_MSG(extack, "Invalid mac address length");
			return -EINVAL;
		}

		if (!is_valid_ether_addr(nla_data(tb[IFLA_ADDRESS]))) {
			//NL_SET_ERR_MSG(extack, "Invalid mac address");
			return -EADDRNOTAVAIL;
		}
	}

	if (tb[IFLA_MTU])
		if (!is_valid_vpls_mtu(nla_get_u32(tb[IFLA_MTU]))) {
			//NL_SET_ERR_MSG(extack, "Invalid MTU");
			return -EINVAL;
		}

	if (!data) {
		//NL_SET_ERR_MSG(extack, "No vpls data available");
		return -EINVAL;
	}

	if (data[IFLA_VPLS_TTL]) {
		__u8 ttl = nla_get_u8(data[IFLA_VPLS_TTL]);

		if (ttl <= 1) {
			//NL_SET_ERR_MSG(extack, "vpls ttl is too small");
			return -EINVAL;
		}
	}

	if (data[IFLA_VPLS_IN_LABEL]) {
		__u32 label = nla_get_u32(data[IFLA_VPLS_IN_LABEL]);

		if (label && (label < MPLS_LABEL_FIRST_UNRESERVED)) {
			//NL_SET_ERR_MSG(extack, "vpls in label is too small");
			return -EINVAL;
		}
	}

	if (data[IFLA_VPLS_OUT_LABEL]) {
		__u32 label = nla_get_u32(data[IFLA_VPLS_OUT_LABEL]);

		if (label && (label < MPLS_LABEL_FIRST_UNRESERVED)) {
			//NL_SET_ERR_MSG(extack, "vpls out label is too small");
			return -EINVAL;
		}
	}

	if (data[IFLA_VPLS_NH]) {
		__u32 addr = ntohl(nla_get_in_addr(data[IFLA_VPLS_NH]));

		if ((addr == INADDR_ANY) ||
		    (addr == INADDR_BROADCAST) ||
		    (addr == INADDR_NONE) ||
		    IN_LOOPBACK(addr)) {
			//NL_SET_ERR_MSG(extack, "Invalid peer IPv4 address");
			return -EINVAL;
		}
	}

	if (data[IFLA_VPLS_NH6]) {
		struct in6_addr addr6;
		int atype;

		if (!IS_ENABLED(CONFIG_IPV6)) {
			//NL_SET_ERR_MSG(extack, "IPv6 not enabled");
			return -EPFNOSUPPORT;
		}

		addr6 = nla_get_in6_addr(data[IFLA_VPLS_NH6]);
		atype = ipv6_addr_type(&addr6);

		if (!(atype & IPV6_ADDR_UNICAST)) {
			//NL_SET_ERR_MSG(extack, "IPv6 is not unicast");
			return -EINVAL;
		}

		if (atype & IPV6_ADDR_LOOPBACK) {
			//NL_SET_ERR_MSG(extack, "IPv6 is loopback");
			return -EINVAL;
		}
	}

	if (data[IFLA_VPLS_DOMAIN]) {
		/* any value is valid */
	}

	if (data[IFLA_VPLS_CRYPTO]) {
		int err = vpls_crypto_validate(data[IFLA_VPLS_CRYPTO]/*, extack*/);
		if (err)
			return err;
	}

	return 0;
}

static void set_carrier_rtnl(struct net_device *dev)
{
	struct vpls_priv *priv = netdev_priv(dev);

	if (priv->input_label && priv->output_label && priv->transport_label)
		netif_carrier_on(dev);
	else
		netif_carrier_off(dev);
}

static int vpls_route_event(struct notifier_block *nb,
			    unsigned long action, void *data)
{
	struct mpls_notifier_data *rt_data = data;
	struct vpls_priv *priv = container_of(nb, struct vpls_priv, nb);
	/* I'm wondered there is netdev_priv() but not vice-versa */
	struct net_device *dev = (struct net_device *)((char*)priv - ALIGN(sizeof(struct net_device), NETDEV_ALIGN));
	bool matches;


	//BUG_ON(netdev_priv(dev) != priv);
	//ASSERT_RTNL();

	if (!net_eq(rt_data->net, priv->encap_net))
		return NOTIFY_DONE;

	/* rt_data->label >= MPLS_LABEL_FIRST_UNRESERVED */

	/* If we have a live route and the message is unrelated */
	if (priv->transport_label && (priv->transport_label != rt_data->label))
		return NOTIFY_DONE;

	/* the label concerns us */
	if (action == RTM_DELROUTE) {
		WRITE_ONCE(priv->transport_label, 0);
		netif_carrier_off(dev);
		return NOTIFY_OK;
	}

	/* RTM_NEWROUTE (may be replaced) */

	matches = mpls_fec_matches(priv->rt_family, &priv->rmt_pe_addr, &rt_data->fec);

	if (priv->transport_label) {
		u32 x;

		if (unlikely(!rt_data->replaced)) {
			printk("new label %u, but we uses %u already\n",
			       rt_data->label, priv->transport_label);
			BUG();
		}

		if (matches)
			return NOTIFY_OK; /* stays live */

		/* search for a new trasnport */
		x = mpls_label_find_fec(priv->encap_net,
					priv->rt_family,
					&priv->rmt_pe_addr);
		WRITE_ONCE(priv->transport_label, x);
		if (!priv->transport_label)
			netif_carrier_off(dev);

		return NOTIFY_OK;
	}

	/* was off */
	if (matches) {
		WRITE_ONCE(priv->transport_label, rt_data->label);
		set_carrier_rtnl(dev);
	}

	return NOTIFY_OK;
}

static int vpls_attach_dev(struct net_device *dev/*,
			   struct netlink_ext_ack *extack*/)
{
	struct vpls_priv *priv = netdev_priv(dev);
	int ret;
	u32 x;

	ret = mpls_substack_add(priv->encap_net,
				priv->input_label,
				dev, vpls_rcv/*,
				extack*/);
	synchronize_rcu();

	if (ret)
		return ret;

	x = mpls_label_find_fec(priv->encap_net,
				priv->rt_family,
				&priv->rmt_pe_addr);
	WRITE_ONCE(priv->transport_label, x);
	set_carrier_rtnl(dev);

	priv->nb.notifier_call = vpls_route_event;
	register_mpls_notifier(&priv->nb);

	return 0;
}

static int vpls_detach_dev(struct net_device *dev/*,
			   struct netlink_ext_ack *extack*/)
{
	struct vpls_priv *priv = netdev_priv(dev);
	int ret;

	unregister_mpls_notifier(&priv->nb);
	ret = mpls_substack_del(priv->encap_net, priv->input_label, dev/*, extack*/);
	netif_carrier_off(dev);
	return ret;
}

#if IS_ENABLED(CONFIG_VPLS_CRYPTO)

static void vpls_crypto_destroy_tfm_rcu(struct rcu_head *head)
{
	struct vpls_crypto_ctx *cra_ctx =
		container_of(head, struct vpls_crypto_ctx, ctx_rcu);

	crypto_free_cipher(cra_ctx->ctx_tfm);
	kfree(cra_ctx);
}

static void vpls_crypto_free(struct vpls_crypto_ctx *cra_ctx)
{
	if (!cra_ctx)
		return;

	kfree(cra_ctx->ctx_key);

	call_rcu(&cra_ctx->ctx_rcu, vpls_crypto_destroy_tfm_rcu);
}

static int vpls_crypto_configure(struct vpls_priv *priv,
				 const struct nlattr *crattr/*,
				 struct netlink_ext_ack *extack*/)
{
	struct nlattr *attr;
	int rem;
	const char *algname = "";
	unsigned int keysize = 0, attr_ksize = 0;
	const u8 *key = NULL;

	struct vpls_crypto_ctx *cra_ctx, *old;

	cra_ctx = NULL;
	if (nla_len(crattr) == 0)
		goto replace_ctx;

	nla_for_each_nested(attr, crattr, rem) {
		switch (nla_type(attr)) {
			case IFLA_VPLS_CRYPTO_ALGO:
				algname = nla_data(attr);
				break;
			case IFLA_VPLS_CRYPTO_KEYSIZE:
				keysize = nla_get_u32(attr);
				break;
			case IFLA_VPLS_CRYPTO_KEY:
				key = nla_data(attr);
				attr_ksize = nla_len(attr);
				break;
			default:
				//NL_SET_ERR_MSG(extack, "Unknown VPLS crypto arg");
				return -EINVAL;
		}
	}

	if (keysize != attr_ksize) {
		//NL_SET_ERR_MSG(extack, "Keysize mismatch in passed args");
		return -EINVAL;
	}

	cra_ctx = kzalloc(sizeof(*cra_ctx), GFP_KERNEL);
	if (unlikely(!cra_ctx)) {
		//NL_SET_ERR_MSG(extack, "No memory");
		return -ENOMEM;
	}

	cra_ctx->ctx_tfm = crypto_alloc_cipher(algname, 0, 0);
	if (IS_ERR(cra_ctx->ctx_tfm)) {
		//NL_SET_ERR_MSG(extack, "Unable to create cipher context");
		rem = PTR_ERR(cra_ctx->ctx_tfm);
		goto no_tfm;
	}

	rem = crypto_cipher_setkey(cra_ctx->ctx_tfm, key, keysize);
	if (rem)
		goto no_setkey;

	cra_ctx->ctx_key = kmemdup(key, keysize, GFP_KERNEL);
	if (unlikely(!cra_ctx->ctx_key)) {
		//NL_SET_ERR_MSG(extack, "No memory");
		rem = -ENOMEM;
		goto no_setkey;
	}
	cra_ctx->ctx_keylen = keysize;

replace_ctx:
	old = rtnl_dereference(priv->cra_ctx);
	rcu_assign_pointer(priv->cra_ctx, cra_ctx);
	vpls_crypto_free(old);

	return 0;

no_setkey:
	crypto_free_cipher(cra_ctx->ctx_tfm);
no_tfm:
	kfree(cra_ctx);
	return rem;
}
#endif

static int vpls_dev_configure(struct net *net, struct net_device *dev,
                              struct nlattr *tb[], struct nlattr *data[],
                              bool createlink/*, struct netlink_ext_ack *extack*/)
{
	struct vpls_priv *priv = netdev_priv(dev);
	int ret;
	bool is_up = (dev->flags & IFF_UP);

	if (createlink && !data[IFLA_VPLS_NH] && !data[IFLA_VPLS_NH6]) {
		//NL_SET_ERR_MSG(extack, "Remote IPv4/v6 is needed");
		return -EINVAL;
	}

	if (is_up) {
		ret = vpls_detach_dev(dev/*, extack*/);

		if (ret)
			return ret;
	}

	if (data[IFLA_VPLS_CRYPTO]) {
		ret = vpls_crypto_configure(priv, data[IFLA_VPLS_CRYPTO]/*,
					    extack*/);

		if (ret)
			return ret;
	}

	if (createlink) {
		if (!tb[IFLA_ADDRESS])
			eth_hw_addr_random(dev);

		if (!tb[IFLA_MTU])
			dev->mtu = VPLS_DEFAULT_MTU;
	}

	if (tb[IFLA_IFNAME])
		nla_strlcpy(dev->name, tb[IFLA_IFNAME], IFNAMSIZ);
	else if (createlink)
		snprintf(dev->name, IFNAMSIZ, DRV_NAME "%%d");

	if (data[IFLA_VPLS_IN_LABEL])
		WRITE_ONCE(priv->input_label, nla_get_u32(data[IFLA_VPLS_IN_LABEL]));

	if (data[IFLA_VPLS_OUT_LABEL])
		WRITE_ONCE(priv->output_label, nla_get_u32(data[IFLA_VPLS_OUT_LABEL]));

	if (data[IFLA_VPLS_NH]) {
		priv->ipv4_addr.s_addr = nla_get_in_addr(data[IFLA_VPLS_NH]);
		priv->rt_family = AF_INET;
	} else if (data[IFLA_VPLS_NH6]) {
		if (!IS_ENABLED(CONFIG_IPV6)) {
			//NL_SET_ERR_MSG(extack, "IPv6 not enabled");
			return -EPFNOSUPPORT;
		}
		priv->ipv6_addr = nla_get_in6_addr(data[IFLA_VPLS_NH6]);
		priv->rt_family = AF_INET6;
	}

	if (data[IFLA_VPLS_TTL])
		WRITE_ONCE(priv->ttl, nla_get_u8(data[IFLA_VPLS_TTL]));
	else if (createlink)
		WRITE_ONCE(priv->ttl, VPLS_DEFAULT_TTL);

	if (data[IFLA_VPLS_DOMAIN])
		WRITE_ONCE(priv->domain, nla_get_u32(data[IFLA_VPLS_DOMAIN]));

	if (is_up)
		return vpls_attach_dev(dev/*,extack*/);

	return 0;
}

static int vpls_newlink(struct net *src_net, struct net_device *dev,
                        struct nlattr *tb[], struct nlattr *data[]/*,
                       struct netlink_ext_ack *extack*/)
{
	int err;
	struct vpls_priv *priv = netdev_priv(dev);

	priv->encap_net = get_net(src_net);
	err = vpls_dev_configure(src_net, dev, tb, data, true/*, extack*/);

	if (err < 0)
		goto err;

	err = register_netdevice(dev);

	if (err < 0)
		goto err;

	netif_carrier_off(dev);

	return 0;

err:
	return err;
}

static void vpls_dellink(struct net_device *dev, struct list_head *head/*,
			 struct netlink_ext_ack *extack*/)
{
	struct vpls_priv *priv = netdev_priv(dev);

#if IS_ENABLED(CONFIG_VPLS_CRYPTO)
	struct vpls_crypto_ctx *cra_ctx = rtnl_dereference(priv->cra_ctx);
	// ??? RCU_INIT_POINTER(priv->cra_ctx, NULL);
	rcu_assign_pointer(priv->cra_ctx, NULL);
	vpls_crypto_free(cra_ctx);
#endif
	unregister_netdevice_queue(dev, head);
	put_net(priv->encap_net);
}

static int vpls_changelink(struct net_device *dev, struct nlattr *tb[],
                           struct nlattr *data[]/*,
                          struct netlink_ext_ack *extack*/)
{
	struct vpls_priv *priv = netdev_priv(dev);
	struct net *net;
	int err;

	net = priv->encap_net;
	err = vpls_dev_configure(net, dev, tb, data, false/*, extack*/);

	return err;
}

#if IS_ENABLED(CONFIG_VPLS_CRYPTO)
static int vpls_fill_crypto_info(struct sk_buff *skb,
				 const struct vpls_priv *priv)
{
	struct vpls_crypto_ctx *cra_ctx = rtnl_dereference(priv->cra_ctx);
	struct nlattr *crattr;
	const char *algname;

	if (!cra_ctx)
		return 0;

	crattr = nla_nest_start(skb, IFLA_VPLS_CRYPTO);
	if (!crattr)
		return -EMSGSIZE;

	if (!capable(CAP_NET_ADMIN))
		goto skip;

	algname = crypto_tfm_alg_name(crypto_cipher_tfm(cra_ctx->ctx_tfm));
	if (nla_put_string(skb, IFLA_VPLS_CRYPTO_ALGO, algname))
		return -EMSGSIZE;

	if (nla_put_u32(skb, IFLA_VPLS_CRYPTO_KEYSIZE, cra_ctx->ctx_keylen))
		return -EMSGSIZE;

	if (nla_put(skb, IFLA_VPLS_CRYPTO_KEY, cra_ctx->ctx_keylen,
		    cra_ctx->ctx_key))
		return -EMSGSIZE;

skip:
	nla_nest_end(skb, crattr);
	return 0;
}
#else
static int vpls_fill_crypto_info(struct sk_buff *skb,
				 const struct vpls_priv *priv)
{
	return 0;
}
#endif

static int vpls_fill_info(struct sk_buff *skb, const struct net_device *dev)
{
	const struct vpls_priv *priv = netdev_priv(dev);

	if (nla_put_u32(skb, IFLA_VPLS_IN_LABEL, priv->input_label) ||
	    nla_put_u32(skb, IFLA_VPLS_OUT_LABEL, priv->output_label) ||
	    nla_put_u32(skb, IFLA_VPLS_TRANSPORT_LABEL, priv->transport_label) ||
	    nla_put_u8 (skb, IFLA_VPLS_TTL, priv->ttl) ||
	    nla_put_u32(skb, IFLA_VPLS_DOMAIN, priv->domain))
		return -EMSGSIZE;

	if (priv->rt_family == AF_INET) {
		if (nla_put_in_addr(skb, IFLA_VPLS_NH, priv->ipv4_addr.s_addr))
			return -EMSGSIZE;
	} else if (priv->rt_family == AF_INET6) {
		if (nla_put_in6_addr(skb, IFLA_VPLS_NH6, &priv->ipv6_addr))
			return -EMSGSIZE;
	}

	return vpls_fill_crypto_info(skb, priv);
}

static struct rtnl_link_ops vpls_link_ops = {
	.changelink     = vpls_changelink,
	.priv_size      = sizeof(struct vpls_priv),
	.fill_info      = vpls_fill_info,
	.validate       = vpls_validate,
	.dellink        = vpls_dellink,
	.newlink        = vpls_newlink,
	.maxtype        = IFLA_VPLS_MAX,
	.policy         = vpls_policy,
	.setup          = vpls_setup,
	.kind           = DRV_NAME,
};

static __init int vpls_init(void)
{
	BUILD_BUG_ON(VPLS_DEFAULT_TTL < ETH_MIN_MTU);
	BUILD_BUG_ON(VPLS_DEFAULT_TTL > ETH_MAX_MTU);
	return rtnl_link_register(&vpls_link_ops);
}

static __exit void vpls_exit(void)
{
	rtnl_link_unregister(&vpls_link_ops);
}

module_init(vpls_init);
module_exit(vpls_exit);

MODULE_AUTHOR("Alexander Nikulin <anikulin@dlink.ru>");
MODULE_DESCRIPTION("Virtual Private LAN Service");
MODULE_ALIAS_RTNL_LINK(DRV_NAME);
MODULE_VERSION(DRV_VERSION);
MODULE_LICENSE("GPL v2");
