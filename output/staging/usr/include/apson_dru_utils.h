#ifndef DLINK_SOFTWARE
#error do not include it if you do not know what it is
#endif


#ifndef APSON_DRU_UTILS_H
#define APSON_DRU_UTILS_H


#include <net/if.h>
extern int dru_wait_br_mac(int sock, struct ifreq *ifr, int seconds);
extern int dru_wait_ifaddr4(int sock4, struct ifreq *ifr, int seconds);

extern int dru_mac_conf_is_allowed(unsigned char mac[6]);

#endif
