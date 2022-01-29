#ifndef _RA_LLDP_PARSE_H_
#define _RA_LLDP_PARSE_H_

#ifdef CONFIG_DRU_SLA_COUNTERS

#include <linux/if_ether.h>
#include <linux/types.h>
#include <linux/times.h>

long get_uptime(void);

#ifdef CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION
#include <linux/vmalloc.h>

typedef struct {
	char ra_lldp_mac[18];
	char ra_lldp_port_desc[255];
	char ra_lldp_system_desc[255];
	char ra_lldp_system_name[255];
	char ra_lldp_port_id[255];
	uint32_t  ra_lldp_ttl; // время жизни пакета lldp
	long ra_lldp_uptime; // uptime, когда пришёл пакет
} lldp_file;

extern lldp_file *ra_lldp_files;

void ra_lldp_parse(unsigned char *data, int port);
#else
extern char ra_lldp_mac[18];
extern char ra_lldp_port_desc[255];
extern char ra_lldp_system_desc[255];
extern char ra_lldp_system_name[255];
extern char ra_lldp_port_id[255];
extern uint32_t  ra_lldp_ttl; // время жизни пакета lldp
extern long ra_lldp_uptime; // uptime, когда пришёл пакет

void ra_lldp_parse(unsigned char *data);
#endif // CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION

#endif
#endif /* _RA_LLDP_PARSE_H_ */
