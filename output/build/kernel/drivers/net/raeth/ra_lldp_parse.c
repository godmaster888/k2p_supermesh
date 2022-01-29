#ifdef CONFIG_DRU_SLA_COUNTERS

#include "ra_lldp_parse.h"

long get_uptime(void)
{
	long uptime;
	struct timespec tp;
	ktime_get_ts(&tp);
	monotonic_to_bootbased(&tp);
	uptime = tp.tv_sec + (tp.tv_nsec ? 1 : 0);
	return uptime;

}

#ifdef CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION
// массив структур для хранения информации lldp по портам
lldp_file *ra_lldp_files;
void ra_lldp_parse(unsigned char *data, int port)
#else
char ra_lldp_mac[18];
char ra_lldp_port_desc[255];
char ra_lldp_system_desc[255];
char ra_lldp_system_name[255];
char ra_lldp_port_id[255];
uint32_t  ra_lldp_ttl; // время жизни пакета lldp
long ra_lldp_uptime; // uptime, когда пришёл пакет

void ra_lldp_parse(unsigned char *data)
#endif // CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION

{
#define VALUABLE_PARAMS_COUNT 2
	uint8_t type;
	uint16_t len;
	char tlv[2];
	uint8_t octet[ETH_ALEN];
	char *mac_ptr;
	int i;

#ifdef CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION

	// выделение памяти по количеству портов
	if (!ra_lldp_files) {
		ra_lldp_files = (lldp_file *)vmalloc(sizeof(lldp_file) * 6);

		if (ra_lldp_files) {
			//инициализация пустыми значениями
			for (i = 0; i < 6; i++) {
				memset((ra_lldp_files + i)->ra_lldp_mac, 0, sizeof((ra_lldp_files + i)->ra_lldp_mac));
				memset((ra_lldp_files + i)->ra_lldp_port_desc, 0, sizeof((ra_lldp_files + i)->ra_lldp_port_desc));
				memset((ra_lldp_files + i)->ra_lldp_system_desc, 0, sizeof((ra_lldp_files + i)->ra_lldp_system_desc));
				memset((ra_lldp_files + i)->ra_lldp_system_name, 0, sizeof((ra_lldp_files + i)->ra_lldp_system_name));
				memset((ra_lldp_files + i)->ra_lldp_port_id, 0, sizeof((ra_lldp_files + i)->ra_lldp_port_id));
				(ra_lldp_files + i)->ra_lldp_ttl = 0;
			}
		} else
			return;
	}

	//обнуление данных в элементе, соответствующем обрабатываемому порту
	memset((ra_lldp_files + port)->ra_lldp_mac, 0, sizeof((ra_lldp_files + port)->ra_lldp_mac));
	memset((ra_lldp_files + port)->ra_lldp_port_desc, 0, sizeof((ra_lldp_files + port)->ra_lldp_port_desc));
	memset((ra_lldp_files + port)->ra_lldp_system_desc, 0, sizeof((ra_lldp_files + port)->ra_lldp_system_desc));
	memset((ra_lldp_files + port)->ra_lldp_system_name, 0, sizeof((ra_lldp_files + port)->ra_lldp_system_name));
	memset((ra_lldp_files + port)->ra_lldp_port_id, 0, sizeof((ra_lldp_files + port)->ra_lldp_port_id));
	(ra_lldp_files + port)->ra_lldp_ttl = 0;
#else
	//обнуление данных в переменных
	memset(ra_lldp_mac, 0, sizeof(ra_lldp_mac));
	memset(ra_lldp_port_desc, 0, sizeof(ra_lldp_port_desc));
	memset(ra_lldp_system_desc, 0, sizeof(ra_lldp_system_desc));
	memset(ra_lldp_system_name, 0, sizeof(ra_lldp_system_name));
	memset(ra_lldp_port_id, 0, sizeof(ra_lldp_port_id));
	ra_lldp_ttl = 0;
#endif // CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION

	type = 0;
	len = 0;

	memset(tlv, 0, sizeof(tlv));
	memset(octet, 0, sizeof(octet));

	mac_ptr = NULL;

	do {
		// Parse TLV: get Type and Length
		memcpy(tlv, data, sizeof(tlv));
		type = (uint8_t) tlv[0] >> 1;
		len = (ntohs((*(uint16_t *) tlv)) << 7);
		len = len >> 7; //WARNING! Do not set in one line with the previous one!
		// Analyze Value
		data += 2;

		switch (type) {
			case 1: {
				// Chassic
				uint8_t chassic_subtype = *(uint8_t *) data;

				// chassis subtype = 4 is MAC
				if (chassic_subtype == 4) {
					// используется (data + 1), т.к. первый байт данных TTL содержит значение chassic subtype
					mac_ptr = data + 1;

					for (i = 0; i < ETH_ALEN; i++) {
						octet[i] = *(uint8_t *) mac_ptr;
						mac_ptr++;
					}

#ifdef CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION
					snprintf((ra_lldp_files + port)->ra_lldp_mac, sizeof((ra_lldp_files + port)->ra_lldp_mac), "%02X:%02X:%02X:%02X:%02X:%02X", octet[0], octet[1], octet[2], octet[3], octet[4], octet[5]);
#else
					snprintf(ra_lldp_mac, sizeof(ra_lldp_mac), "%02X:%02X:%02X:%02X:%02X:%02X", octet[0], octet[1], octet[2], octet[3], octet[4], octet[5]);
#endif // CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION
				}
			}
			break;

			case 2: {
				// Port Number
				uint8_t port_id_subtype = *(uint8_t *) data;

				// port ID subtype = 1 is interface alias
				if (port_id_subtype == 1) {
#ifdef CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION
					// используются (data + 1) и (len - 1), так как первый байт данных TLV содержит port ID subtype
					memcpy((ra_lldp_files + port)->ra_lldp_port_id, data + 1, len - 1);
#else
					memcpy(ra_lldp_port_id, data + 1, len - 1);
#endif
				}
			}
			break;

			case 3: {
				// Time to live
#ifdef CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION
				(ra_lldp_files + port)->ra_lldp_ttl = ntohs(*(uint16_t *) data);
#else
				ra_lldp_ttl = ntohs(*(uint16_t *) data);
#endif
			}
			break;

			case 4: {
				// Port Description
#ifdef CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION
				memcpy((ra_lldp_files + port)->ra_lldp_port_desc, data, len);
#else
				memcpy(ra_lldp_port_desc, data, len);
#endif // CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION
			}
			break;

			case 5: {
				// System Name
#ifdef CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION
				memcpy((ra_lldp_files + port)->ra_lldp_system_name, data, len);
#else
				memcpy(ra_lldp_system_name, data, len);
#endif
			}
			break;

			case 6: {
				// System Description
#ifdef CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION
				memcpy((ra_lldp_files + port)->ra_lldp_system_desc, data, len);
#else
				memcpy(ra_lldp_system_desc, data, len);
#endif // CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION
			}
			break;

			default:
				break;
		}

		data += len;
	} while ((*(uint16_t *) tlv) != 0x0000);

#ifdef CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION
	(ra_lldp_files + port)->ra_lldp_uptime = get_uptime();
#else
	ra_lldp_uptime = get_uptime();
#endif // CONFIG_RAETH_INCOMING_PORT_NUM_DETECTION

}
#endif
