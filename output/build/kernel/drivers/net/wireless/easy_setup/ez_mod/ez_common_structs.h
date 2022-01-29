
#ifndef __EZ_COMMON_STRUCTS_H__
#define __EZ_COMMON_STRUCTS_H__

#define EZ_MAX_DEVICE_SUPPORT 7
#ifndef MAC_ADDR_LEN
#define MAC_ADDR_LEN				6
#endif

#define CONFIG_PUSH_VER_SUPPORT 	1	
#define MOBILE_APP_SUPPORT 			1
#define EZ_LEN_PSK			 		64


#ifdef CONFIG_PUSH_VER_SUPPORT

#define NETWORK_WEIGHT_LEN  (MAC_ADDR_LEN + 2)

#else

#define NETWORK_WEIGHT_LEN  (MAC_ADDR_LEN + 1)

#endif

typedef struct GNU_PACKED _ez_node_number {
	UCHAR path_len; //path len is the length of the entire node number including the root_mac
	UCHAR root_mac[MAC_ADDR_LEN];
	UCHAR path[EZ_MAX_DEVICE_SUPPORT];
}EZ_NODE_NUMBER;


typedef enum _enum_ez_api_mode
{
    FULL_OFFLOAD,
	BEST_AP_OFFLOAD,
	CONNECTION_OFFLOAD
}enum_ez_api_mode;

typedef struct GNU_PACKED beacon_info_tag_s
{
	unsigned char network_weight[NETWORK_WEIGHT_LEN];
	EZ_NODE_NUMBER node_number;
	unsigned char other_ap_mac[MAC_ADDR_LEN];
	unsigned char other_ap_channel;
} beacon_info_tag_t;

#endif

