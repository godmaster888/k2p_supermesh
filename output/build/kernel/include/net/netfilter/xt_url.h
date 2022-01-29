#ifndef _XT_URL_H
#define _XT_URL_H

#include <linux/types.h>
#include <asm/byteorder.h>

extern int xt_url_alloc_lists(int count);
extern void xt_url_free_lists(void);

/* !! host and path are NOT null-terminated !!
 * DO NOT USE str-functions on those
 */
extern bool xt_url_match_found(__u8 list_ix,
                               const char *host, __u16 host_len,
                               const char *path, __u16 path_len);

/*
 * returns 4 for GET, 5 for POST, HEAD
 * -1 otherwise
 * buffer must be at least 5 bytes
 */
static __always_inline  unsigned int
xt_url_method_offset(void *http)
{
	if (*(__u32 *)http == __constant_htonl(0x47455420))
		return 4;	/* "GET " */
	else if (((char *)http)[4] != ' ')
		return -1;	/* not 4-sym method */
	else if (*(__u32 *)http == __constant_htonl(0x504f5354))
		return 5;	/* "POST" */
	else if (*(__u32 *)http == __constant_htonl(0x48454144))
		return 5;	/* "HEAD" */

	return -1;
}

#endif /* _XT_URL_H */
