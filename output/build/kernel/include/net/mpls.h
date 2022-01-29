/*
 * Copyright (c) 2014 Nicira, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 */

#ifndef _NET_MPLS_H
#define _NET_MPLS_H 1

#include <linux/if_ether.h>
#include <linux/netdevice.h>
#include <linux/mpls.h>
#include <linux/notifier.h>
#include <linux/in.h>		/* struct in_addr */
#if IS_ENABLED(CONFIG_IPV6)	/* struct in6_addr */
#include <linux/in6.h>
#endif

#define MPLS_HLEN 4

struct mpls_shim_hdr {
	__be32 label_stack_entry;
};

static inline bool eth_p_mpls(__be16 eth_type)
{
	return eth_type == htons(ETH_P_MPLS_UC) ||
		eth_type == htons(ETH_P_MPLS_MC);
}

static inline struct mpls_shim_hdr *mpls_hdr(const struct sk_buff *skb)
{
	return (struct mpls_shim_hdr *)skb_network_header(skb);
}

static inline struct mpls_shim_hdr mpls_entry_encode(u32 label, u32 ttl,
                                                    u32 tc, bool bos)
{
       struct mpls_shim_hdr result;
       result.label_stack_entry =
               cpu_to_be32((label << MPLS_LS_LABEL_SHIFT) |
                           (tc << MPLS_LS_TC_SHIFT) |
                           (bos ? (1 << MPLS_LS_S_SHIFT) : 0) |
                           (ttl << MPLS_LS_TTL_SHIFT));
       return result;
}

struct mpls_entry_decoded {
	u32 label;
	u8 ttl;
	u8 tc;
	u8 bos;
};

bool mpls_output_possible(const struct net_device *dev);
bool mpls_input_possible_rcu(const struct net_device *dev);

/* must hold RTNL  */
int mpls_label_find_fec(struct net *net, int af, const void *addr);

struct mpls_fec {
	u8	fec_family; /* AF_UNSPEC, AF_INET or AF_INET6 */
	u8	fec_prefixlen;
	union {
		unsigned long fec_addr;
		struct in_addr	ipv4;
#if IS_ENABLED(CONFIG_IPV6)
		struct in6_addr	ipv6;
#endif
	};
};
bool mpls_fec_matches(int af, const void *addr, const struct mpls_fec *fec);

struct mpls_notifier_data {
	struct net *net;
	u32 label;
	/* Ignore if event == RTM_DELROUTE.
	 * true: old -> new (NLM_F_REPLACE)
	 * false: NULL->new
	 */
	bool replaced;
	struct mpls_fec fec;
};
/* chain is notified under RTNL with event = RTM_NEWROUTE/RTM_DELROUTE */
int register_mpls_notifier(struct notifier_block *nb);
int unregister_mpls_notifier(struct notifier_block *nb);

#ifdef CONFIG_MPLS_SUBSTACK

/*
 * returns notifier codes (I dont want
 * to introduce new defines).
 * NOTIFY_DONE - not mine, skb is unchanged
 * NOTIFY_OK - suits me, but I handled an
 *   internal copy. The skb is still reachable.
 * NOTIFY_BAD - bad skb, I took care, mpls_forward()
 *   should forget it.
 * NOTIFY_STOP - it's exclusively mine, forget it also.
 */

typedef int (*mpls_substack_recv)(void *hnd_arg,
				  struct sk_buff *skb,
				  struct net_device *in_dev,
				  struct mpls_entry_decoded *dec);


/* call the reg/unreg under RTNL */
int mpls_substack_add(struct net *net, u32 input_label,
		      void *hnd_arg, mpls_substack_recv recv/*,
		      struct netlink_ext_ack *extack*/);
int mpls_substack_del(struct net *net, u32 input_label,
		      void *hnd_arg/*,
		      struct netlink_ext_ack *extack*/);

#endif

#endif
