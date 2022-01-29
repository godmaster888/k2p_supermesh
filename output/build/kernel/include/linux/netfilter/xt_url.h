#ifndef _XT_URL_MATCH_H
#define _XT_URL_MATCH_H

#include <linux/types.h>

#define XT_MATCH_URL_MAX_LISTS	4
struct xt_url_match_info {
	__u8 list_ix;
	__u8 invert;
};

#endif /* _XT_URL_MATCH_H */
