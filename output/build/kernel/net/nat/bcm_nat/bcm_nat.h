#ifndef _BCM_NAT_WANTED
#define _BCM_NAT_WANTED

#ifndef __LINUX_NETFILTER_H
/* avoid invalid include sequence */
#include <net/ip.h>
#include <net/route.h>
#include <net/neighbour.h>
#endif


//#define DEBUG

/*
 * fastnat must full skip mcast/bcast traffic
 * allow only tcp with established/reply pkts (fix mss correction bug in syn pkts)
 * or udp proto for processing in software offload
 */
#define FASTNAT_SKIP_TYPE(skb)		((skb)->pkt_type == PACKET_BROADCAST || (skb)->pkt_type == PACKET_MULTICAST)
#define FASTNAT_SKIP_PROTO(protonum)	(protonum != IPPROTO_UDP && protonum != IPPROTO_TCP)
#define FASTNAT_ESTABLISHED(ctinfo)	(ctinfo == IP_CT_ESTABLISHED || ctinfo == IP_CT_ESTABLISHED_REPLY)

/*
 * cb fastforward flag
 * offcet do not be replaced in any code (as hw_nat, strongly check this, now find max cb offset in wifi drivers = 41)
 */
#define NF_FAST_ROUTE		42

/*
 * conntrack fastforward flags
 * please undefine NF_STOP in linux/netfilter.h if fastnat use
 */
#define	NF_FAST_NAT_DENY	1
#define NF_FAST_NAT		5
#define NF_STOP			6

/* see net/netfilter/nf_conntrack_core.c */
extern unsigned int nf_conntrack_fastnat;
extern unsigned int nf_conntrack_fastroute;

/* kernel */
extern int nf_ct_ipv4_gather_frags(struct sk_buff *skb, u_int32_t user);
extern int ip_rcv_finish(struct sk_buff *skb);

/* bcm */

/* forward decl types */
struct nf_conn;
struct nf_conntrack_l3proto;
struct nf_conntrack_l4proto;
enum ip_conntrack_info;

#if 0
extern int skb_is_ready(struct sk_buff *skb);
#else
/*
 * check SKB really accesseble
 */
static inline int skb_is_ready(struct sk_buff *skb)
{
	if (skb_cloned(skb) && !skb->sk)
		return 0;
	return 1;
}
#endif
extern int is_pure_routing(struct nf_conn *ct);

extern int bcm_fast_path(struct sk_buff *skb);
extern int bcm_do_fastroute(struct nf_conn *ct, struct sk_buff *skb, unsigned int hooknum, int set_reply);
extern int bcm_do_fastnat(struct nf_conn *ct, enum ip_conntrack_info ctinfo, struct sk_buff *skb,
			  struct nf_conntrack_l3proto *l3proto, struct nf_conntrack_l4proto *l4proto);

#endif
