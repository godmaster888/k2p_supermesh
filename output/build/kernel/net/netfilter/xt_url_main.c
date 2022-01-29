/* Kernel module to match HTTP URL's. */

/* Copyright (C) 2015 farisey <anikulin@dlink.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/string.h>
#include <linux/ip.h>

#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
#define XT_URL_IPV6
#endif

#ifdef XT_URL_IPV6
#include <linux/ipv6.h>
#include <net/ipv6.h>
#endif

#include <linux/tcp.h>

#include <linux/netfilter/xt_url.h>
#include <net/netfilter/xt_url.h>
#include <linux/netfilter/x_tables.h>
#include <net/netfilter/nf_conntrack.h>		/* xt_url_loaded */

//#include <linux/netfilter_ipv4/ip_tables.h>
//#include <linux/netfilter_ipv6/ip6_tables.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("farisey <anikulin@dlink.ru>");
MODULE_DESCRIPTION("Xtables: URL match");
MODULE_ALIAS("ipt_url");
#ifdef XT_URL_IPV6
MODULE_ALIAS("ip6t_url");
#endif

static unsigned url_lists = 1;
module_param(url_lists, uint, 0644);
MODULE_PARM_DESC(url_lists, "Number of URL lists (4 max)");

/*
#define URL_MATCH_TRUE	(true  ^ info->invert)
#define URL_MATCH_FALSE	(false ^ info->invert)
*/
/*
#define MAKE_U32(a,b,c,d)			\
	(					\
		(((__u32)(a)) << 24) |		\
		(((__u32)(b)) << 16) |		\
		(((__u32)(c)) <<  8) |		\
		(((__u32)(d)) <<  0) |		\
	)
*/

/*
 * +len - ended with space
 * -len - followed by \r\n
 */
static __always_inline int extract_len(const char *token)
{
	int res = 0;

	do {
		if (*token == ' ')
			return res;

		if (*(__u16 *)token == __constant_htons(0x0d0a))
			return -res;

		token++;
		res++;
	} while (1);

	return 0; /* fake */
}
static __always_inline const char *find_host(const char *from, const char *end)
{
	end -= 6; /* needed for "Host: " */

	for (; from < end; from = strstr(from, "\r\n") + 2) {
		if (*(__u32 *)from != __constant_htonl(0x486f7374))
			continue; /* not "Host" */

		if (*(__u16 *)(from + 4) != __constant_htons(0x3a20))
			continue; /* not ": " */

		from += 6; /* skip "Host: " */

		/* skip spaces */
		while (*from == ' ') from++;

		return from;
	}

	return NULL;
}

static bool
url_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_url_match_info *info = par->matchinfo;
	const struct iphdr *ip;
#ifdef XT_URL_IPV6
	const struct ipv6hdr *ipv6;
	u8 nexthdr;
	__be16 frag_off;
#endif
	const struct tcphdr *th;
	struct tcphdr _tcph;
	char *http;
	int http_len;
	char *path, *host;
	int path_len, host_len;

	switch (par->family) {
		case NFPROTO_IPV4:
			ip = ip_hdr(skb);

			if (ip->protocol != IPPROTO_TCP)
				return false;

			break;

#ifdef XT_URL_IPV6

		case NFPROTO_IPV6:
			ipv6 = ipv6_hdr(skb);

			nexthdr = ipv6->nexthdr;
			ipv6_skip_exthdr(skb, sizeof(*ipv6), &nexthdr, &frag_off);

			if (nexthdr != IPPROTO_TCP)
				return false;

			break;
#endif

		default:
			return false;/* impossible */
	}

	th = skb_header_pointer(skb, par->thoff, sizeof(_tcph), &_tcph);

	if (unlikely(th == NULL))
		goto dropit;

	http = (char *)th + (th->doff << 2);
	http_len = (int)skb->len - (http - (char *)skb->data);

	if (http_len <= 16)
		return false;//URL_MATCH_FALSE;

	/* check \r\n\r\n at the end */
	if (*(__u32 *)(http + (http_len - 4)) != __constant_htonl(0x0d0a0d0a))
		return false;//URL_MATCH_FALSE;

	/* path */
	path_len = xt_url_method_offset(http);

	if (path_len < 0)
		return false;

	path = http + path_len;

	/* skip spaces */
	while (*path == ' ') path++;

	path_len = extract_len(path);

	if (unlikely(path_len <= 0))
		goto dropit;

	/* find Host starting from 2nd string */
	host = (char *)find_host(strstr(path + path_len + 1, "\r\n") + 2,
	                         http + http_len);

	if (unlikely(!host))
		goto dropit;

	host_len = -extract_len(host);

	if (unlikely(host_len <= 0))
		goto dropit;

	/*
		return xt_url_match_found(info->list_ix, host, host_len,
		                          path, path_len) ?
		       URL_MATCH_TRUE : URL_MATCH_FALSE;
	*/
	return xt_url_match_found(info->list_ix, host, host_len,
	                          path, path_len) ^ info->invert;

dropit:
	par->hotdrop = true;
	return false;
}

static int url_mt_check(const struct xt_mtchk_param *par)
{
	const struct xt_url_match_info *info = par->matchinfo;

	if (info->list_ix >= url_lists)
		return -ERANGE;

	return 0;
}

static struct xt_match url_mt_reg[] __read_mostly = {
	{
		.name		= "url",
		.family		= NFPROTO_IPV4,
		.match		= url_mt,
		.checkentry	= url_mt_check,
		.matchsize	= sizeof(struct xt_url_match_info),
		//.proto		= IPPROTO_TCP, //this needs -m tcp before -m url
		.me		= THIS_MODULE,
	},
#ifdef XT_URL_IPV6
	{
		.name		= "url",
		.family		= NFPROTO_IPV6,
		.match		= url_mt,
		.checkentry	= url_mt_check,
		.matchsize	= sizeof(struct xt_url_match_info),
		//.proto		= IPPROTO_TCP, //this needs -m tcp before -m url
		.me		= THIS_MODULE,
	},
#endif
};

static int __init url_mt_init(void)
{
	int n;

	if (url_lists > XT_MATCH_URL_MAX_LISTS) {
		printk(KERN_ERR
		       "Too many (%u) URL lists requested, %u is max\n",
		       url_lists, XT_MATCH_URL_MAX_LISTS);
		return -EINVAL;
	}

	if (!url_lists) url_lists = 1;

	n = xt_url_alloc_lists(url_lists);

	if (n < 0) return n;

	n = xt_register_matches(url_mt_reg, ARRAY_SIZE(url_mt_reg));

	if (n < 0) {
		xt_url_free_lists();
		return n;
	}

	if (!n)
		xt_url_loaded = 1;

	return n;
}

static void __exit url_mt_exit(void)
{
	xt_url_loaded = 0;
	xt_unregister_matches(url_mt_reg, ARRAY_SIZE(url_mt_reg));
	xt_url_free_lists();
}

module_init(url_mt_init);
module_exit(url_mt_exit);
