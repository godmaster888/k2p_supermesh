/**************************************************************************
 *
 * Copyright (c) 2015 D-Link Corporation
 * All Rights Reserved
 *
 * Author: Konstantin Vasin
 * Abstract: libnetscan is the library for search DHCP and PPPoE servers.
 *
 * This library is the proprietary software of D-Link Corporation.
 *
 * ***********************************************************************/
#ifndef _NETSCAN_H_
#define _NETSCAN_H_

#include <jansson.h>

extern int scan_dhcp(const char *ifname, unsigned int time, json_t *result);
extern int scan_pppoe(const char *ifname, unsigned int time, json_t *result);

//return only scan result (wrappers)
extern int scan_pppoe_result(const char *ifname, unsigned int time);
extern int scan_dhcp_result(const char *ifname, unsigned int time);

#endif
