#ifndef __SHARED_STRUCTS_H__
#define __SHARED_STRUCTS_H__
#include "ez_common_structs.h"
#include "ez_mod_os.h"


#define MAX_LEN_OF_SSID				32


enum EZDEV_TYPE {
	EZDEV_TYPE_AP = (1 << 0),
	EZDEV_TYPE_STA = (1 << 1),
	EZDEV_TYPE_ADHOC = (1 << 2),
	EZDEV_TYPE_WDS = (1 << 3),
	EZDEV_TYPE_MESH = (1 << 4),
	EZDEV_TYPE_GO = (1 << 5),
	EZDEV_TYPE_GC = (1 << 6),
	EZDEV_TYPE_APCLI = (1 << 7),
	EZDEV_TYPE_REPEATER = (1 << 8),
	EZDEV_TYPE_P2P_DEVICE = (1 << 9),
};

#define EZ_PMK_LEN                    32

#define EZ_MAX_STA_NUM 8

#define EZ_AES_KEY_ENCRYPTION_EXTEND	8 /* 8B for AES encryption extend size */

#define EZ_RAW_KEY_LEN                192

#define EZ_NONCE_LEN                  32

#define EZ_DH_KEY_LEN                 32

#define MAX_EZ_BANDS 						2
#define MAX_NON_EZ_BANDS 						2
#define MAX_DRV_AD_CNT			3

#define MAX_EZ_PEERS_PER_BAND		8
#define EZDEV_NUM_MAX							6

#define EZ_PTK_LEN                    80

#define EZ_MAX_LEN_OF_SSID						32

#define EZ_MAX_DEVICE_SUPPORT 7

#ifndef MAC_ADDR_LEN
#define MAC_ADDR_LEN				6
#endif
#define OPEN_GROUP_MAX_LEN		20


#define GROUPID_LEN_BUF		128

#define AES_KEYWRAP_BLOCK_SIZE		8


/* 2-byte Frame control field */
typedef struct GNU_PACKED {
#ifdef RT_BIG_ENDIAN
	UINT16 Order:1;		/* Strict order expected */
	UINT16 Wep:1;		/* Wep data */
	UINT16 MoreData:1;	/* More data bit */
	UINT16 PwrMgmt:1;	/* Power management bit */
	UINT16 Retry:1;		/* Retry status bit */
	UINT16 MoreFrag:1;	/* More fragment bit */
	UINT16 FrDs:1;		/* From DS indication */
	UINT16 ToDs:1;		/* To DS indication */
	UINT16 SubType:4;	/* MSDU subtype */
	UINT16 Type:2;		/* MSDU type */
	UINT16 Ver:2;		/* Protocol version */
#else
        UINT16 Ver:2;		/* Protocol version */
	UINT16 Type:2;		/* MSDU type, refer to FC_TYPE_XX */
	UINT16 SubType:4;	/* MSDU subtype, refer to  SUBTYPE_XXX */
	UINT16 ToDs:1;		/* To DS indication */
	UINT16 FrDs:1;		/* From DS indication */
	UINT16 MoreFrag:1;	/* More fragment bit */
	UINT16 Retry:1;		/* Retry status bit */
	UINT16 PwrMgmt:1;	/* Power management bit */
	UINT16 MoreData:1;	/* More data bit */
	UINT16 Wep:1;		/* Wep data */
	UINT16 Order:1;		/* Strict order expected */
#endif	/* !RT_BIG_ENDIAN */
} FRAME_CONTROL_EZ, *PFRAME_CONTROL_EZ;


typedef struct  _HEADER_802_11_EZ {
    FRAME_CONTROL_EZ FC;
    USHORT        Duration;
    UCHAR         Addr1[MAC_ADDR_LEN];
    UCHAR         Addr2[MAC_ADDR_LEN];
    UCHAR         Addr3[MAC_ADDR_LEN];
    USHORT        Frag : 4;
    USHORT        Sequence : 12;
} HEADER_802_11_EZ, *PHEADER_802_11_EZ;



#define EZ_MAX_LEN_OF_BSS_TABLE             256 /* 64 */
#define BSS_NOT_FOUND					  0xFFFFFFFF
	

	

	


typedef struct _EZ_BSS_ENTRY{
	UCHAR MacAddr[MAC_ADDR_LEN];
	UCHAR Bssid[MAC_ADDR_LEN];
	UCHAR Channel;
	UCHAR CentralChannel;	/*Store the wide-band central channel for 40MHz.  .used in 40MHz AP. Or this is the same as Channel. */
	CHAR Rssi;
	UCHAR SsidLen;
	CHAR Ssid[MAX_LEN_OF_SSID];
	UINT32 AKMMap;
	UINT32 PairwiseCipher;	/* Pairwise Key */
	UINT32 GroupCipher; /* Group Key */
#ifdef WH_EZ_SETUP
	 unsigned char support_easy_setup;
	 unsigned int easy_setup_capability;
	 BOOLEAN	 bConnectAttemptFailed;
	BOOLEAN non_ez_beacon;
	 UCHAR open_group_id[OPEN_GROUP_MAX_LEN];
	 UCHAR open_group_id_len;
	 beacon_info_tag_t beacon_info;
#endif /* WH_EZ_SETUP */
} EZ_BSS_ENTRY;
	

	

typedef struct {
	UCHAR			BssNr;
	UCHAR			BssOverlapNr;
	EZ_BSS_ENTRY	   BssEntry[EZ_MAX_LEN_OF_BSS_TABLE];
} EZ_BSS_TABLE, *PEZ_BSS_TABLE;

typedef struct ez_driver_ops_s{
	unsigned char (*RandomByte)(void * ezdev);
	void (*GenRandom)(void * ezdev, UCHAR *macAddr, UCHAR *random);
	void (*DH_PublicKey_Generate) (void * ezdev, UINT8 *GValue, UINT GValueLength,UINT8 *PValue,UINT PValueLength,
		UINT8 *PrivateKey,UINT PrivateKeyLength,UINT8 *PublicKey,UINT *PublicKeyLength);
	void (*RT_DH_SecretKey_Generate) (void * ezdev, UINT8 PublicKey[], UINT PublicKeyLength, UINT8 PValue[], UINT PValueLength, 
			UINT8 PrivateKey[],UINT PrivateKeyLength, UINT8 SecretKey[], UINT *SecretKeyLength);
	void (*RT_SHA256)(void *ezdev, const UINT8 Message[], UINT MessageLen, UINT8 DigestMessage[]);
	VOID (*WpaDerivePTK)(void * ezdev, UCHAR *PMK, UCHAR *ANonce, UCHAR *AA, UCHAR *SNonce, UCHAR *SA, UCHAR *output, UINT len);
	INT (*AES_Key_Unwrap)(void * ezdev, UINT8 CipherText[],UINT CipherTextLength, UINT8 Key[],UINT KeyLength,UINT8 PlainText[],UINT *PlainTextLength);
	void (*ez_install_pairwise_key)(void * ezdev, char *peer_mac, unsigned char *pmk, unsigned char *ptk, unsigned char authenticator);
	void (*ez_apcli_install_group_key)(void * ezdev, char *peer_mac, char *peer_gtk, unsigned char ptk_len);
	int (*wlan_config_get_ht_bw)(void 
*ezdev);
	int (*wlan_config_get_vht_bw)(void 
*ezdev);
	int (*wlan_operate_get_ht_bw)(void 
*ezdev);
	int (*wlan_operate_get_vht_bw)(void 
*ezdev);
	int (*wlan_config_get_ext_cha)(void 
*ezdev);
	int (*wlan_operate_get_ext_cha)(void 
*ezdev);
	int (*get_cli_aid)(void * ezdev, char * peer_mac);
	void (*ez_cancel_timer)(void * ezdev, void * timer_struct);
	void (* ez_set_timer)(void * ezdev, void * timer_struct, unsigned long time);
	BOOLEAN (* ez_is_timer_running)(void * ezdev, void * timer_struct);


	int (* get_apcli_enable)(void *ezdev);
	int (* ApScanRunning)(void *ezdev);

	void (*ez_send_unicast_deauth)(void *ezdev, char *peer_mac);
	void (*ez_restore_channel_config)(void *ezdev);
	void (*UpdateBeaconHandler)(void *ezdev, int reason);
	void (*ez_update_security_setting)(void *ezdev, unsigned char *pmk);
	void (*ez_update_ap_wsc_profile)(void *ezdev);
	void (*APScanCnclAction)(void *ezdev);
	void (*ez_send_loop_detect_pkt)(void *ezdev, unsigned char *pOtherCliMac);
	

	BOOLEAN (*ez_update_ap)(void *ezdev, void *updated_configs);
	BOOLEAN (*ez_update_cli)(void *ezdev, void *updated_configs);
	void (*ez_update_ap_peer_record)(void *ezdev, BOOLEAN band_switched, unsigned char *peer_mac);
	void (*ez_update_cli_peer_record)(void *ezdev, BOOLEAN band_switched, unsigned char *peer_mac);
	void (*	MiniportMMRequest)(void *ezdev, char *out_buf,int frame_len, BOOLEAN need_tx_status);
	void (*NdisGetSystemUpTime)(void * ezdev, ULONG *time);
	INT (*AES_Key_Wrap )(void * ezdev, UINT8 PlainText[],UINT  PlainTextLength,UINT8 Key[],UINT  KeyLength,UINT8 CipherText[],UINT *CipherTextLength);
	INT (*RtmpOSWrielessEventSendExt)(void* ezdev,UINT32 eventType,INT flags,PUCHAR pSrcMac,PUCHAR pData,UINT32 dataLen);
	void (*ez_send_broadcast_deauth)(void *ezdev);
	void (*MgtMacHeaderInit)(void *ezdev, HEADER_802_11_EZ *pHdr80211,UCHAR SubType,UCHAR ToDs,UCHAR *pDA,UCHAR *pSA,UCHAR *pBssid);
	void (*apcli_stop_auto_connect)(void *ezdev, BOOLEAN enable);
	void (*timer_init)(void *ezdev, void* timer, void *callback);
	void (*set_ap_ssid_null)(void *ezdev);
	//void (*ez_set_entry_apcli)(void *ezdev, UCHAR *mac_addr, BOOLEAN is_apcli);
	void *(*ez_get_pentry)(void *ezdev, UCHAR *mac_addr);
	void (*ez_mark_entry_duplicate)(void *ezdev, UCHAR *mac_addr);
	void (*ez_restore_cli_config)(void *ezdev);
	void (*ScanTableInit)(void *ezdev);
	void (*RT_HMAC_SHA1)(void * ezdev, UINT8 Key[], UINT KeyLen, UINT8 Message[], UINT MessageLen, UINT8 MAC[], UINT MACLen);
	BOOLEAN (*is_mlme_running)(void *ezdev);
#if 1
	void (*ez_ApSiteSurvey_by_wdev)(void * ezdev,void * pSsid,UCHAR ScanType,BOOLEAN ChannelSel,BOOLEAN scan_one_channel);
	void (*ez_BssTableSsidSort)(void * ad_obj,void *wdev_obj,EZ_BSS_TABLE *OutTab,
		CHAR Ssid[],UCHAR SsidLen);
	void (*ez_get_scan_table)(void * ad_obj,EZ_BSS_TABLE *ez_scan_tab);
	void (*ez_add_entry_in_apcli_tab)(void * ad_obj, void* wdev_obj, ULONG bss_idx);
	void (*ez_sort_apcli_tab_by_rssi)(void * ad_obj, void* wdev_obj);
	void (*ez_ApCliBssTabInit)(void * ad_obj, void* wdev_obj);
	BOOLEAN (*ez_update_cli_conn)(void * ad_obj, void * ezdev, EZ_BSS_ENTRY *bss_entry);
	void (*ez_update_partial_scan)(void * ez_ad,void * wdev_obj);
	void (*ez_rtmp_set_channel)(void * ad_obj, void * wdev_obj, UINT8 channel);
	void (*wlan_config_set_ht_bw)(void *ezdev, UINT8 ht_bw);
	void (*wlan_config_set_vht_bw)(void *ezdev, UINT8 ht_bw);
	void (*wlan_config_set_ext_cha)(void *ezdev, UINT8 ext_cha);
	INT (*SetCommonHtVht)(void *ezdev);
	void (*ez_reset_entry_duplicate)(void *ezdev, UCHAR *mac_addr);
#endif
}ez_driver_ops_t;


typedef struct __non_ez_band_psk_info_tag{
	unsigned char encrypted_psk[EZ_LEN_PSK + EZ_AES_KEY_ENCRYPTION_EXTEND];
}NON_EZ_BAND_PSK_INFO_TAG;

//! Levarage from MP.1.0 CL #170037
typedef struct __non_man_info{
	unsigned char ssid[EZ_MAX_LEN_OF_SSID];
	unsigned char ssid_len;	
	unsigned char psk[EZ_LEN_PSK];
//! Leverage form MP.1.0 CL 170364
	unsigned char encryptype[32];
	unsigned char authmode[32];

#ifdef DOT11R_FT_SUPPORT
		UINT8 FtMdId[FT_MDID_LEN]; // share MDID info so that all devices use same MDID, irrespective of FT enabled or not.
#else
		UINT8 rsvd[2];
#endif
	
} NON_MAN_INFO;


//! Leverage form MP.1.0 CL 170037
typedef struct __non_man_info_tag{
	unsigned char ssid[EZ_MAX_LEN_OF_SSID];
	unsigned char ssid_len;	
	unsigned char encrypted_psk[EZ_LEN_PSK + EZ_AES_KEY_ENCRYPTION_EXTEND];
//! Leverage form MP.1.0 CL 170364
	unsigned char encryptype[32];
	unsigned char authmode[32];

#ifdef DOT11R_FT_SUPPORT
	UINT8 FtMdId[FT_MDID_LEN]; // share MDID info so that all devices use same MDID, irrespective of FT enabled or not.
#else
	UINT8 rsvd[2];
#endif

} NON_MAN_INFO_TAG;


typedef struct __ez_triband_sec_config
{
	UINT32 PairwiseCipher;
	UINT32 GroupCipher;    
	UINT32 AKMMap;
	
} EZ_TRIBAND_SEC_CONFIG;


typedef struct __non_ez_band_info_tag{
	unsigned char ssid[EZ_MAX_LEN_OF_SSID];
	unsigned char ssid_len;	
	EZ_TRIBAND_SEC_CONFIG triband_sec;	
	unsigned char encrypted_pmk[EZ_PMK_LEN + EZ_AES_KEY_ENCRYPTION_EXTEND];
	//unsigned char encrypted_psk[EZ_LEN_PSK + EZ_AES_KEY_ENCRYPTION_EXTEND];
#ifdef DOT11R_FT_SUPPORT
	UINT8 FtMdId[FT_MDID_LEN]; // share MDID info so that all devices use same MDID, irrespective of FT enabled or not.
#else
	UINT8 rsvd[2];
#endif

}NON_EZ_BAND_INFO_TAG;

typedef enum _enum_loop_chk_role{
    NONE,
	SOURCE,
	DEST
}enum_loop_chk_role;
struct _ez_roam_info
{
	unsigned char ez_apcli_roam_bssid[MAC_ADDR_LEN];
	unsigned char roam_channel;
	unsigned long timestamp;
};


typedef struct GNU_PACKED _loop_chk_info {
	enum_loop_chk_role loop_chk_role;
	UCHAR source_mac[MAC_ADDR_LEN];
}LOOP_CHK_INFO;

typedef struct GNU_PACKED channel_info_s{
	unsigned char channel;
#ifdef EZ_PUSH_BW_SUPPORT
	unsigned char ht_bw;
	unsigned char vht_bw;
#else
	unsigned char rsvd1;
	unsigned char rsvd2;
#endif
	unsigned char extcha;
}channel_info_t;


typedef struct ez_init_params_s{
	void * ad_obj;
	void * wdev_obj;
	char func_idx;
	enum EZDEV_TYPE ezdev_type;
	UCHAR mac_add[MAC_ADDR_LEN];
	UCHAR ssid[EZ_MAX_LEN_OF_SSID];
	unsigned char ssid_len;
	UCHAR pmk[EZ_PMK_LEN];
	UCHAR psk[EZ_LEN_PSK];
	unsigned int group_id_len;
	unsigned int ez_group_id_len;	//for localy maintain EzGroupID
	unsigned int gen_group_id_len;  //for localy maintain EzGenGroupID

	unsigned char *group_id;
	unsigned char *ez_group_id;		//for localy maintain EzGroupID
	unsigned char *gen_group_id;	//for localy maintain EzGenGroupID	

	unsigned int open_group_id_len;
	unsigned char open_group_id[OPEN_GROUP_MAX_LEN];

	void *driver_ops_lut;
	unsigned char *channel;
	UINT32 os_hz;
	void *ez_scan_timer;
	//void *ez_stop_scan_timer;
	void *ez_scan_pause_timer;
	void *ez_group_merge_timer;
	void *ez_loop_chk_timer;
	void *ez_connect_wait_timer;

	channel_info_t channel_info;
	unsigned char default_group_data_band;
#ifdef IF_UP_DOWN
	UINT8 ez_intf_count_config_ap;
	UINT8 ez_intf_count_config_cli;
	UINT8 non_ez_intf_count_config_ap;
	UINT8 non_ez_intf_count_config_cli;
#endif
} ez_init_params_t;

typedef struct GNU_PACKED interface_info_tag_s
{
	unsigned char ssid[EZ_MAX_LEN_OF_SSID];
	unsigned char ssid_len;
	channel_info_t channel_info;
	unsigned char ap_mac_addr[MAC_ADDR_LEN];//! this band AP MAC
	unsigned char cli_mac_addr[MAC_ADDR_LEN];//! this band CLI MAC
	unsigned char link_duplicate;//! when seen in CLI context it meens that other band CLI is also connected to same repeater, if seen in AP context(ez_peer) it means that both CLIs of other repeater are connected to me
#ifdef DOT11R_FT_SUPPORT
	UINT8 FtMdId[FT_MDID_LEN]; // share MDID info so that all devices use same MDID, irrespective of FT enabled or not.
#else
	UINT8 rsvd[2];
#endif
} interface_info_tag_t;
typedef struct GNU_PACKED interface_info_s{
	interface_info_tag_t shared_info;
	unsigned char cli_peer_ap_mac[MAC_ADDR_LEN];//! mac address of AP to which my cli connects, will be mostly used in CLI wdev context
	BOOLEAN non_easy_connection;
	unsigned char interface_activated;
	unsigned char pmk[EZ_PMK_LEN];
	unsigned char psk[EZ_LEN_PSK];
}interface_info_t;
typedef struct GNU_PACKED _ez_group_id {
	unsigned char ucFlags;
	unsigned int open_group_id_len;
	unsigned char open_group_id[OPEN_GROUP_MAX_LEN];
	unsigned int ez_group_id_len;	//for localy maintain EzGroupID
	unsigned char ez_group_id[GROUPID_LEN_BUF + 1]; 	//for localy maintain EzGroupID
}ez_group_id_t;

struct _ez_security {
#ifdef EZ_API_SUPPORT
	enum_ez_api_mode ez_api_mode;
#endif
	//OS_NDIS_SPIN_LOCK		ez_apcli_list_sem_lock;
#if 0
	LIST_HEADER			ez_apcli_list;
#endif

	void * ez_scan_timer;
	//void * ez_stop_scan_timer;
	void * ez_scan_pause_timer;
	void * ez_group_merge_timer;

	OS_NDIS_SPIN_LOCK	ez_scan_pause_timer_lock;
	signed char best_ap_rssi_threshold;
	unsigned int capability;
	unsigned int group_id_len;
	unsigned int ez_group_id_len;	//for localy maintain EzGroupID
	unsigned int gen_group_id_len;  //for localy maintain EzGenGroupID
	unsigned char *group_id;
	unsigned char *ez_group_id;		//for localy maintain EzGroupID
	unsigned char *gen_group_id;	//for localy maintain EzGenGroupID	
	unsigned char self_dh_random_seed[EZ_RAW_KEY_LEN]; /* do NOT change after configured */
	unsigned char self_pke[EZ_RAW_KEY_LEN];
	unsigned char self_pkr[EZ_RAW_KEY_LEN];
	unsigned char self_nonce[EZ_NONCE_LEN];
	unsigned char keep_finding_provider;
	unsigned char first_scan;
	unsigned char client_count;
	unsigned char go_internet;
	unsigned char user_configured;
	signed char rssi_threshold;

	unsigned char merge_peer_addr[MAC_ADDR_LEN];
#ifdef DISCONNECT_ON_CONFIG_UPDATE
	unsigned char force_connect_bssid[MAC_ADDR_LEN];
	unsigned long force_bssid_timestamp;
#endif
	unsigned char weight_update_going_on; //! flag will be set when force weight is pushed, normal action frame will not be processed in this case
	unsigned char do_not_restart_interfaces;//! flag will be set while calling rtmp_set_channel to avoid CLI down/up
	unsigned char delay_disconnect_count;//! to increase beacon miss duration
    interface_info_t this_band_info;//! ssid, pmk, mac address, and CLI peer MAC address, information of wdev to which wdev correspond
	interface_info_t other_band_info_backup;//! ssid, pmk, mac address, and CLI peer MAC address, information of wdev to which wdev correspond
	BOOLEAN ap_did_fallback;
	unsigned char fallback_channel;
	BOOLEAN ez_apcli_immediate_connect;
	BOOLEAN ez_connection_permission_backup;
	BOOLEAN ez_is_connection_allowed;
	unsigned char ez_apcli_force_ssid[EZ_MAX_LEN_OF_SSID];
	unsigned char ez_apcli_force_ssid_len;
	unsigned char ez_apcli_force_bssid[MAC_ADDR_LEN];
	unsigned char ez_apcli_force_channel;
	
	unsigned int open_group_id_len;
	unsigned char open_group_id[OPEN_GROUP_MAX_LEN];
#ifdef EZ_ROAM_SUPPORT
	struct _ez_roam_info ez_roam_info;
	UCHAR ez_ap_roam_blocked_mac[MAC_ADDR_LEN];
#endif

#ifdef EZ_DUAL_BAND_SUPPORT
	BOOLEAN internal_force_connect_bssid;
	BOOLEAN internal_force_connect_bssid_timeout;
	unsigned long force_connect_bssid_time_stamp;
    LOOP_CHK_INFO loop_chk_info;
	void * ez_loop_chk_timer;
	BOOLEAN first_loop_check;
	BOOLEAN dest_loop_detect;
#endif
	unsigned char ez_action_type;
	UINT32 ez_scan_delay;
	UINT32 ez_max_scan_delay;
	BOOLEAN ez_scan_same_channel;
	ULONG	ez_scan_same_channel_timestamp;
	ULONG	ez_wps_reconnect_timestamp;
	BOOLEAN disconnect_by_ssid_update;

	unsigned char default_ssid[EZ_MAX_LEN_OF_SSID];
	unsigned char default_ssid_len;
	unsigned char default_pmk[EZ_PMK_LEN];
	unsigned char default_pmk_valid;
	ULONG partial_scan_time_stamp;
	BOOLEAN bPartialScanRunning;
};

typedef struct ez_dev_s{
	void *ez_ad;
	void *driver_ad;
	void *wdev;
	enum EZDEV_TYPE ezdev_type;
	unsigned char *channel;
	char own_mac_addr[MAC_ADDR_LEN];
	char if_addr[MAC_ADDR_LEN];
	char bssid[MAC_ADDR_LEN];
	unsigned char ez_band_idx;
	struct _ez_security ez_security;
	unsigned int os_hz;
	UINT8 CfgSsid[MAX_LEN_OF_SSID];
	UINT8 CfgSsidLen;
	UINT8 CfgApCliBssid[MAC_ADDR_LEN];
	UINT8 attempted_candidate_index;
	UINT8 support_ez_setup;
	ez_driver_ops_t *driver_ops;	
	OS_NDIS_SPIN_LOCK *ez_peer_table_lock;
	void *ez_connect_wait_timer_backup;
} ez_dev_t;
typedef struct GNU_PACKED weight_defining_link_s {
	void *ezdev;
	ULONG time_stamp;
	ULONG ap_time_stamp;
	UCHAR peer_mac[MAC_ADDR_LEN];
	UCHAR peer_ap_mac[MAC_ADDR_LEN];
}weight_defining_link_t;

typedef struct GNU_PACKED device_info_s{

	unsigned char network_weight[NETWORK_WEIGHT_LEN];
	weight_defining_link_t weight_defining_link;
	EZ_NODE_NUMBER ez_node_number;
} device_info_t;
typedef struct non_ez_driver_ops_s{
	//unsigned char (*RandomByte)(void *);
	int (*RtmpOSWrielessEventSendExt)(	void *ad,int band_id,UINT32 eventType,INT flags,PUCHAR pSrcMac,PUCHAR pData,UINT32 dataLen);
	void (*ez_update_non_ez_ap)(void * ad_obj, NON_EZ_BAND_INFO_TAG *non_ez_and_info_tag, void *non_ez_band_info, void *updated_configs, int band_id);
	void (*HwCtrlWifiSysRestart)(void * ad);
	void (*ez_send_broadcast_deauth)(void *ad, void *wdev);
	void (*ez_init_non_ez_ap)(void *ad, void *wdev, void * non_ez_band_info);
	void (*restart_ap)(void *wdev);
}non_ez_driver_ops_t;

typedef struct __non_ez_band_info{
	void *ez_ad;
	void *pAd;
	CHAR func_idx; 
	void *non_ez_ap_wdev;
	void *non_ez_cli_wdev;
	non_ez_driver_ops_t lut_driver_ops;
	unsigned char ssid[EZ_MAX_LEN_OF_SSID];
	unsigned char ssid_len;	
	EZ_TRIBAND_SEC_CONFIG triband_sec;	
	BOOLEAN need_restart;
	unsigned char pmk[EZ_PMK_LEN];
	unsigned char *channel;
	unsigned char psk[EZ_LEN_PSK];
#ifdef DOT11R_FT_SUPPORT
		UINT8 FtMdId[FT_MDID_LEN]; // share MDID info so that all devices use same MDID, irrespective of FT enabled or not.
#else
		UINT8 rsvd[2];
#endif
	
} NON_EZ_BAND_INFO;

typedef struct psk_to_app_s
{
	unsigned char psk1[EZ_LEN_PSK];
	unsigned char psk2[EZ_LEN_PSK];
} psk_to_app_t;

struct _ez_peer_security_info {
	void *ad;
	ez_dev_t *ezdev;
	unsigned char ez_band_idx;
	unsigned int capability;
	unsigned int group_id_len;
	unsigned int gen_group_id_len;
	unsigned int gtk_len;
	unsigned char *group_id;
	unsigned char *gen_group_id;
	unsigned char *gtk;
	unsigned char mac_addr[MAC_ADDR_LEN];
	unsigned char peer_pke[EZ_RAW_KEY_LEN];
	unsigned char peer_nonce[EZ_NONCE_LEN];
	unsigned char dh_key[EZ_DH_KEY_LEN];
	unsigned char sw_key[EZ_PTK_LEN];
	//unsigned char pmk[EZ_PMK_LEN];
	unsigned char valid;

	unsigned char port_secured;
	unsigned int open_group_id_len;
	unsigned char open_group_id[OPEN_GROUP_MAX_LEN];

#ifdef EZ_DUAL_BAND_SUPPORT
	device_info_t device_info;// when device info is used in ez_peer, only node number is expected to give correct information, others are session variable which does not hold any significance after comparision
	interface_info_t this_band_info;
	interface_info_t other_band_info;
#else
	unsigned char ssid_len;
	unsigned char ssid[EZ_MAX_LEN_OF_SSID];
	unsigned char other_band_ssid_len;
	unsigned char other_band_ssid[EZ_MAX_LEN_OF_SSID];
	unsigned char other_band_pmk[PMK_LEN];
	unsigned char network_weight[NETWORK_WEIGHT_LEN];
	unsigned char ap_mac_addr[MAC_ADDR_LEN];
	unsigned char target_channel;
	unsigned char ht_bw;
	unsigned char vht_bw;
	unsigned char ext_cha_offset;
#endif	

	unsigned long creation_time;
	unsigned char ez_peer_table_index;
	BOOLEAN delete_in_differred_context;
	BOOLEAN ez_disconnect_due_roam;
	NON_EZ_BAND_INFO_TAG non_ez_band_info[MAX_NON_EZ_BANDS];
	NON_EZ_BAND_PSK_INFO_TAG non_ez_psk_info[MAX_NON_EZ_BANDS];
	//! Levarage from MP1.0 CL#170037
	NON_MAN_INFO_TAG non_man_info;
};



typedef struct __ez_band_info{
	void *pAd;
	CHAR func_idx; 
	ez_dev_t ap_ezdev;
	ez_dev_t cli_ezdev;
	struct _ez_peer_security_info ez_peer_table[MAX_EZ_PEERS_PER_BAND];
	OS_NDIS_SPIN_LOCK ez_peer_table_lock;
	ez_driver_ops_t lut_driver_ops;
} EZ_BAND_INFO;

typedef struct __ez_adapter{
	void *sanity_check1;
	unsigned char band_count;
	EZ_BAND_INFO ez_band_info[MAX_EZ_BANDS];
	unsigned char non_ez_band_count;
#ifdef IF_UP_DOWN
	unsigned char ez_intf_count_config_ap;  // no. of ez interfaces configured
	unsigned char non_ez_intf_count_config_ap; // no. of non ez interfaces configured
	unsigned char ez_intf_count_config_cli;  // no. of ez interfaces configured
	unsigned char non_ez_intf_count_config_cli; // no. of non ez interfaces configured

	unsigned char ez_intf_count_current_ap;  // no. of ez interfaces active currently
	unsigned char non_ez_intf_count_current_ap; // no. of non ez interfaces active currently
	unsigned char ez_intf_count_current_cli;  // no. of ez interfaces active currently
	unsigned char non_ez_intf_count_current_cli; // no. of non ez interfaces active currently

	unsigned char ez_all_intf_up_once;
#endif

	//void *sanity_check1;
	NON_EZ_BAND_INFO non_ez_band_info[MAX_NON_EZ_BANDS];
	//void *sanity_check2;
#ifndef RT_CFG80211_SUPPORT
	unsigned int backhaul_channel;
	unsigned int front_end_channel;
#else
	unsigned int u4ConfigPushTriggered;
	unsigned int u4Reserved;
#endif
#ifdef EZ_PUSH_BW_SUPPORT
	BOOLEAN push_bw_config;
#endif
#ifdef EZ_API_SUPPORT
		enum_ez_api_mode ez_api_mode;
#endif
	//void *sanity_check3;

	UINT8 ez_roam_time;
	unsigned char ez_delay_disconnect_count;
	UINT8 ez_wait_for_info_transfer;
	UINT8 ez_wdl_missing_time;
	UINT32 ez_force_connect_bssid_time;
	//void *sanity_check4;
	UINT8 ez_peer_entry_age_out_time;
	UINT8 ez_scan_same_channel_time;
	UINT32 ez_partial_scan_time;
	signed char best_ap_rssi_threshld[10];
	unsigned char best_ap_rssi_threshld_max;
	UINT32 max_scan_delay;
	//void *sanity_check5;
#if 1
	void *ez_connect_wait_timer;
	ez_dev_t *ez_connect_wait_ezdev;
#endif
	unsigned long ez_connect_wait_timer_value;
	unsigned long ez_connect_wait_timer_timestamp;
	unsigned char configured_status; /* 0x01 - un-configured, 0x02 - configured */
	device_info_t device_info;//! network weight, node number and weight defining link info, all interface should have same content in htis structure
	//void *sanity_check6;
#ifdef EZ_DFS_SUPPORT
	char dedicated_man_ap;
#endif
	char Peer2p4mac[MAC_ADDR_LEN];
	//! Levarage from MP1.0 CL#170037
	unsigned char is_man_nonman;
	NON_MAN_INFO non_man_info;	


#ifdef DUAL_CHIP		
		OS_NDIS_SPIN_LOCK ez_handle_disconnect_lock;
		OS_NDIS_SPIN_LOCK ez_beacon_update_lock;
		OS_NDIS_SPIN_LOCK ez_miniport_lock;
		OS_NDIS_SPIN_LOCK ez_set_channel_lock;
		OS_NDIS_SPIN_LOCK ez_set_peer_lock;
		OS_NDIS_SPIN_LOCK ez_conn_perm_lock;
		OS_NDIS_SPIN_LOCK ez_mlme_sync_lock;
#endif	
	

	ez_dev_t *ezdev_list[EZDEV_NUM_MAX];
	
	BOOLEAN SingleChip;
	
	//void *sanity_check7;
#if 1
		void *drv_ad[MAX_DRV_AD_CNT];
		UINT8 drv_ad_cnt;
#else
	void * first_ad;
	void * second_ad;
	void *third_ad;
	unsigned char unique_ad_count;
#endif
	UINT32 debug;
	void *sanity_check;
	
//! Levarage from MP1.0 CL 170210
//! repeater device flag removed from ez_adapter
	unsigned char default_group_data_band;
} EZ_ADAPTER;

typedef struct GNU_PACKED updated_configs_s{
	unsigned char mac_addr[MAC_ADDR_LEN];//! mac addr of peer from which we received configs
	BOOLEAN context_linkdown;
	unsigned char *group_id;
	unsigned int group_id_len;
	unsigned char *gen_group_id;
	unsigned int gen_group_id_len;
	unsigned int open_group_id_len;
	unsigned char open_group_id[OPEN_GROUP_MAX_LEN];
	device_info_t device_info;//! updated device info
	interface_info_t this_band_info;//! updated
	interface_info_t other_band_info;
	NON_EZ_BAND_INFO_TAG non_ez_info[2];
	NON_EZ_BAND_PSK_INFO_TAG non_ez_psk_info[2];
	BOOLEAN need_ez_update;
	BOOLEAN need_non_ez_update_ssid[2];
	BOOLEAN need_non_ez_update_psk[2];
	BOOLEAN need_non_ez_update_secconfig[2];
	//! Levarage from MP1.0 CL #170037
	NON_MAN_INFO_TAG non_man_info;

}updated_configs_t;

struct ez_GUI_info 
{
    unsigned char EzEnable;
    unsigned char EzConfStatus;
    unsigned char EzGroupID[248];
    unsigned char EzGenGroupId[32];
	unsigned char EzOpenGroupID[20];
    unsigned char ApCliEzConfStatus;
    unsigned char ApCliEzGroupID[248];
	unsigned char ApCliEzGenGroupId[32];
	unsigned char ApCliEzOpenGroupID[20];
	unsigned char ApCliHideSSID[32];
    unsigned char ApCliAuthMode[13];
    unsigned char ApCliEncrypType[7];
    unsigned char ApCliWPAPSK[64];
	unsigned char ApCliSsid[32];
    BOOLEAN ApCliEnable;
    unsigned char ApCliEzEnable;
};

typedef struct EZ_UPDATE_SSID_PSK_MSG
{
	char ssid1[MAX_LEN_OF_SSID + 1];
	char ssid2[MAX_LEN_OF_SSID + 1];
	char ssid3[MAX_LEN_OF_SSID + 1];
	char psk1[EZ_LEN_PSK + 1];
	char psk2[EZ_LEN_PSK + 1];
	char psk3[EZ_LEN_PSK + 1];
	char pmk1[EZ_PMK_LEN];
	char pmk2[EZ_PMK_LEN];
	char pmk3[EZ_PMK_LEN];
	char EncrypType1[32];
	char EncrypType2[32];
	char AuthMode1[32];
	char AuthMode2[32];

}ez_update_ssid_psk_msg_t;
#endif

