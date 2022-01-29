/*
 * mpls in net namespaces
 */

#ifndef __NETNS_MPLS_H__
#define __NETNS_MPLS_H__

#ifdef CONFIG_MPLS_SUBSTACK
#include <linux/types.h> /* struct list_head */
#endif

struct mpls_route;
struct ctl_table_header;

struct netns_mpls {
	int ip_ttl_propagate;
	int default_ttl;
	size_t platform_labels;
	struct mpls_route __rcu * __rcu *platform_label;
#ifdef CONFIG_MPLS_SUBSTACK
	struct list_head substacks;
#endif

	struct ctl_table_header *ctl;
};

#endif /* __NETNS_MPLS_H__ */
