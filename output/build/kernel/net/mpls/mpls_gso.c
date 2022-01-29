/*
 *	MPLS GSO Support
 *
 *	Authors: Simon Horman (horms@verge.net.au)
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 *
 *	Based on: GSO portions of net/ipv4/gre.c
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/err.h>
#include <linux/module.h>
#include <linux/netdev_features.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>

static struct sk_buff *mpls_gso_segment(struct sk_buff *skb,
				       netdev_features_t features)
{
	struct sk_buff *segs = ERR_PTR(-EINVAL);
	sk_buff_data_t mac_offset = skb->mac_header;
	netdev_features_t mpls_features;
	u16 mac_len = skb->mac_len;
	__be16 mpls_protocol;
	unsigned int mpls_hlen;

	skb_reset_network_header(skb);
	mpls_hlen = skb_inner_network_header(skb) - skb_network_header(skb);
	if (unlikely(!pskb_may_pull(skb, mpls_hlen)))
		goto out;

	if (unlikely(skb_shinfo(skb)->gso_type &
				~(SKB_GSO_TCPV4 |
				  SKB_GSO_TCPV6 |
				  SKB_GSO_UDP |
				  SKB_GSO_DODGY |
				  SKB_GSO_TCP_ECN)))
		goto out;

	/* Setup inner SKB. */
	mpls_protocol = skb->protocol;
	skb->protocol = skb->inner_protocol;

	__skb_pull(skb, mpls_hlen);

	skb->mac_len = 0;
	skb_reset_mac_header(skb);

	/* Segment inner packet. */
	mpls_features = skb->dev->mpls_features & netif_skb_features(skb);
	segs = skb_mac_gso_segment(skb, mpls_features);
	if (IS_ERR_OR_NULL(segs)) {
		skb_gso_error_unwind(skb, mpls_protocol, mpls_hlen, mac_offset,
				     mac_len);
		goto out;
	}
	skb = segs;

	mpls_hlen += mac_len;
	do {
		skb->mac_len = mac_len;
		skb->protocol = mpls_protocol;

		skb_reset_inner_network_header(skb);

		__skb_push(skb, mpls_hlen);

		skb_reset_mac_header(skb);
		skb_set_network_header(skb, mac_len);
	} while ((skb = skb->next));

out:
	return segs;
}

static int mpls_gso_send_check(struct sk_buff *skb)
{
	return 0;
}

static struct packet_offload mpls_mc_offload = {
	.type = cpu_to_be16(ETH_P_MPLS_MC),
	.priority = 15,
	.callbacks = {
		.gso_send_check =	mpls_gso_send_check,
		.gso_segment    =	mpls_gso_segment,
	},
};

static struct packet_offload mpls_uc_offload = {
	.type = cpu_to_be16(ETH_P_MPLS_UC),
	.priority = 15,
	.callbacks = {
		.gso_send_check =	mpls_gso_send_check,
		.gso_segment    =	mpls_gso_segment,
	},
};

static int __init mpls_gso_init(void)
{
	pr_info("MPLS GSO support\n");

	dev_add_offload(&mpls_uc_offload);
	dev_add_offload(&mpls_mc_offload);

	return 0;
}

static void __exit mpls_gso_exit(void)
{
	dev_remove_offload(&mpls_uc_offload);
	dev_remove_offload(&mpls_mc_offload);
}

module_init(mpls_gso_init);
module_exit(mpls_gso_exit);

MODULE_DESCRIPTION("MPLS GSO support");
MODULE_AUTHOR("Simon Horman (horms@verge.net.au)");
MODULE_LICENSE("GPL");