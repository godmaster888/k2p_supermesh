
 BOOLEAN ez_is_ap_apcli(ez_dev_t *ezdev);

 BOOLEAN ez_is_ap(ez_dev_t *ezdev);

 BOOLEAN ez_is_cli(ez_dev_t *ezdev);

 unsigned char ez_gen_dh_public_key(
	ez_dev_t *ezdev);



 unsigned char ez_gen_dh_private_key(
	ez_dev_t *ezdev,
	struct _ez_peer_security_info *ez_peer);


 void ez_compute_dh_key(
	ez_dev_t *ezdev,
	struct _ez_peer_security_info *ez_peer);


 void ez_get_sw_encrypted_key(
	struct _ez_security *ez_sec_info,
	struct _ez_peer_security_info *ez_peer,
	unsigned char *a_addr,
	unsigned char *s_addr);


 void ez_calculate_mic(
 	ez_dev_t *ezdev,
	unsigned char *sw_key,
	unsigned char *msg,
	unsigned int msg_len,
	unsigned char *mic);

unsigned short ez_check_for_ez_enable(
	void *msg,
	unsigned long msg_len
	);

 unsigned short ez_probe_request_sanity(
	void *msg,
	unsigned long msg_len,
	struct _ez_peer_security_info *ez_peer);


 unsigned short ez_probe_beacon_response_sanity(
	void *msg,
	unsigned long msg_len,
	struct _ez_peer_security_info *ez_peer);

 unsigned short ez_auth_request_sanity(
	void *msg,
	unsigned long msg_len,
	struct _ez_peer_security_info *ez_peer);

 unsigned short ez_auth_response_sanity(
	void *msg,
	unsigned long msg_len,
	struct _ez_peer_security_info *ez_peer);


 unsigned short ez_assoc_request_sanity(
	unsigned char isReassoc,
	void *msg,
	unsigned long msg_len,
	struct _ez_peer_security_info *ez_peer);

 unsigned short ez_assoc_response_sanity(
	void *msg,
	unsigned long msg_len,
	struct _ez_peer_security_info *ez_peer);


 struct _ez_peer_security_info *ez_peer_table_insert(
	ez_dev_t *ezdev,
 	unsigned char *addr);

void ez_peer_table_delete(
	ez_dev_t *ezdev,
	unsigned char *addr);

struct _ez_peer_security_info *ez_peer_table_search_by_addr(
	ez_dev_t *ezdev,
	unsigned char *addr);

void ez_show_peer_table_info(
	ez_dev_t *ezdev);

void ez_insert_tlv(
	unsigned char tag,
	unsigned char *data,
	unsigned char data_len,
	unsigned char *buffer,
	unsigned long *msg_len);
BOOLEAN ez_apcli_is_link_duplicate(ez_dev_t *ezdev,unsigned char * peer_addr);


void ez_prepare_non_ez_tag(NON_EZ_BAND_INFO_TAG * non_ez_tag, NON_EZ_BAND_PSK_INFO_TAG * non_ez_psk_tag,struct _ez_peer_security_info *ez_peer);
//! Levarage from MP1.0 CL #170037
void ez_prepare_non_man_tag(NON_MAN_INFO_TAG * non_man_tag, struct _ez_peer_security_info *ez_peer);


unsigned char ez_install_ptk(
	struct _ez_peer_security_info *ez_peer,
	unsigned char authenticator);

unsigned char ez_apcli_install_gtk(
	struct _ez_peer_security_info *ez_peer);

BOOLEAN ez_is_loop_formed(struct _ez_peer_security_info *ez_peer);
void ez_show_interface_info(ez_dev_t *ezdev);
	
void ez_show_device_info(device_info_t ez_device_info);

 unsigned char ez_mac_addr_compare(
	unsigned char *addr1,
	unsigned char *addr2);


void ez_allocate_node_number(
	EZ_NODE_NUMBER *node_number,
	ez_dev_t *ezdev);
void ez_apcli_allocate_self_node_number(
	EZ_NODE_NUMBER *node_number, 
	ez_dev_t *ezdev, char *mac_addr);

 void ez_restore_node_number(EZ_NODE_NUMBER *ez_node_number);

void ez_allocate_node_number_sta(
	struct _ez_peer_security_info *ez_peer,
	BOOLEAN is_forced);

/*check whether peer node is child node of the own node*/
BOOLEAN ez_is_child_node(
	EZ_NODE_NUMBER own_node_number, 
	EZ_NODE_NUMBER peer_node_number);
BOOLEAN ez_is_same_open_group_id(ez_dev_t *ezdev, char *open_group_id, char open_group_id_len);

BOOLEAN ez_is_other_band_connection_to_same_bss(ez_dev_t *ezdev, beacon_info_tag_t *beacon_info);

void increment_best_ap_rssi_threshold(struct _ez_security *ez_security);


void ez_apcli_force_bssid(
	ez_dev_t *ezdev,
	unsigned char *bssid);


/* Indicates whether connection wait timer is running on any of the interfaces*/
 BOOLEAN ez_conn_wait_timer_running(EZ_ADAPTER *ez_ad);
BOOLEAN ez_is_link_duplicate(struct _ez_peer_security_info *ez_peer);

void ez_wait_for_connection_allow(
	unsigned long time,
	EZ_ADAPTER *ez_ad);

#ifdef EZ_ROAM_SUPPORT
void ez_apcli_check_roaming_status(EZ_ADAPTER *ez_ad);
#endif


#ifdef EZ_ROAM_SUPPORT

BOOLEAN ez_is_roaming_ongoing_hook(EZ_ADAPTER *ez_ad);



struct _ez_peer_security_info * ez_peer_table_search_by_node_number(ez_dev_t *ezdev, EZ_NODE_NUMBER ez_node_number);

struct _ez_peer_security_info *ez_find_link_to_roam_candidate(EZ_ADAPTER *ez_ad,
	ez_dev_t *ezdev, 
	EZ_NODE_NUMBER target_node_number);


PUCHAR ez_get_other_band_bssid(beacon_info_tag_t *beacon_info);
UCHAR ez_get_other_band_channel(beacon_info_tag_t *beacon_info);

void ez_initiate_roam(ez_dev_t *ezdev, PUCHAR roam_bssid, UCHAR roam_channel);

BOOLEAN ez_is_bss_user_configured(beacon_info_tag_t *beacon_info);

#endif

#ifdef EZ_PUSH_BW_SUPPORT
 void update_ap_peer_record(EZ_ADAPTER *ez_ad, void * ezdev, BOOLEAN this_band_changed, BOOLEAN other_band_changed);

 void update_cli_peer_record(EZ_ADAPTER *ez_ad, void * ezdev, BOOLEAN this_band_changed, BOOLEAN other_band_changed);

 void update_peer_record(EZ_ADAPTER *ez_ad, void * ezdev, BOOLEAN this_band_changed, BOOLEAN other_band_changed);
#endif

//! Levarage from MP1.0 CL#170037
void ez_prepare_man_plus_nonman_nonez_device_info_to_app(EZ_ADAPTER *ez_ad, man_plus_nonman_nonez_device_info_to_app_t *dev_info);

void ez_prepare_man_plus_nonman_ez_device_info_to_app(EZ_ADAPTER *ez_ad, man_plus_nonman_ez_device_info_to_app_t *dev_info);

void ez_prepare_triband_nonez_device_info_to_app(EZ_ADAPTER *ez_ad, triband_nonez_device_info_to_app_t *dev_info);

void ez_prepare_triband_ez_device_info_to_app(EZ_ADAPTER *ez_ad, triband_ez_device_info_to_app_t *dev_info);


void ez_prepare_device_info_to_app(EZ_ADAPTER *ez_ad, device_info_to_app_t *dev_info);

BOOLEAN push_and_update_ap_config(EZ_ADAPTER *ez_ad, void * ezdev, updated_configs_t *updated_configs, BOOLEAN group_id_diff);


BOOLEAN push_and_update_cli_config(EZ_ADAPTER *ez_ad, void * ezdev, updated_configs_t *updated_configs, BOOLEAN group_id_diff);

void ez_init_updated_configs_for_adapt(updated_configs_t *updated_config, struct _ez_peer_security_info *ez_peer, ez_dev_t * ezdev);

void ez_init_updated_configs_for_push(updated_configs_t *updated_config, ez_dev_t * ezdev);

void ez_chk_bw_config_different(ez_dev_t * ezdev, struct _ez_peer_security_info *ez_peer, BOOLEAN *pthis_band_changed, BOOLEAN *pOther_band_changed);


enum_config_update_action_t push_and_update_config(EZ_ADAPTER *ez_ad , ez_dev_t *ezdev,
	struct _ez_peer_security_info *ez_peer, 
	BOOLEAN check_for_weight, 
	BOOLEAN from_port_secured,
	BOOLEAN surely_a_group_merge
	);

 char * ez_CheckAuthMode(UINT32 _AKMMap);

 char * ez_CheckEncrypType(UINT32 Cipher);
#ifdef EZ_API_SUPPORT
void ez_port_secured_for_connection_offload(void);
#endif
#ifdef EZ_NETWORK_MERGE_SUPPORT
BOOLEAN ez_get_other_band_info(ez_dev_t * ezdev, void *other_band_config);
struct _ez_peer_security_info *ez_get_other_band_ez_peer(ez_dev_t * ezdev, struct _ez_peer_security_info *ez_peer);

void ez_inform_all_interfaces(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev, inform_other_band_action_t action);

 BOOLEAN ez_ap_basic_config_changed(ez_dev_t * ezdev, updated_configs_t *updated_configs);


BOOLEAN ez_update_other_band_ap(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev, updated_configs_t *updated_configs, BOOLEAN group_id_diff);
BOOLEAN ez_update_this_band_ap(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev, updated_configs_t *updated_configs, BOOLEAN group_id_diff);
#endif

BOOLEAN ez_update_other_band_cli(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev, updated_configs_t *updated_configs, BOOLEAN group_id_diff);

BOOLEAN ez_update_this_band_cli(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev, updated_configs_t *updated_configs, BOOLEAN group_id_diff);

void ez_update_this_band_cli_peer_record(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev);


void ez_switch_wdl_to_other_band(ez_dev_t *ezdev, void *other_band_obj);

void ez_notify_roam(EZ_ADAPTER *ez_ad, 
	struct _ez_peer_security_info * from_ez_peer, 
	BOOLEAN for_roam, ez_custom_data_cmd_t *data, 
	unsigned char datalen);

void ez_update_other_band_cli_peer_record(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev);

void ez_update_other_band_ap_peer_record(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev);

void ez_update_this_band_ap_peer_record(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev);

void ez_set_ap_fallback_context(ez_dev_t *ezdev, BOOLEAN fallback, unsigned char fallback_channel);

ez_dev_t * ez_get_otherband_ezdev(ez_dev_t *ezdev);
ez_dev_t * ez_get_otherband_ap_ezdev(ez_dev_t *ezdev);
ez_dev_t * ez_get_otherband_cli_ezdev(ez_dev_t *ezdev);

void * ez_get_otherband_ad(ez_dev_t *ezdev);

void convert_pmk_string_to_hex(char *sys_pmk_string, char *sys_pmk);

unsigned char ez_set_open_group_id(
	struct _ez_security *ez_sec_info,
	unsigned char *open_group_id,
	unsigned int open_group_id_len,
	unsigned char inf_idx);


/*check whether peer node is child node of the own node*/
BOOLEAN ez_is_weight_same_mod(	PUCHAR own_weight, 
	PUCHAR peer_weight);

int ez_allocate_or_update_band(EZ_ADAPTER *ez_ad, ez_init_params_t *init_params);
void ez_dealloc_non_ez_band(ez_dev_t *ezdev);
void ez_dealloc_band(ez_dev_t *ezdev);
void ez_allocate_or_update_non_ez_band(ez_dev_t *ezdev);

VOID RtmpOsMsDelay(ULONG msec);

void send_action_notify_roam(EZ_ADAPTER *ez_ad, 
					ez_dev_t *ezdev, 
					struct _ez_peer_security_info *ez_peer
					);

void send_action_delay_disconnect(EZ_ADAPTER *ez_ad, 
					ez_dev_t *ezdev, 
					struct _ez_peer_security_info *ez_peer,
					unsigned char delay_disconnect_count
					);
void send_action_update_weight(EZ_ADAPTER *ez_ad,
					unsigned char *mac_addr,
					ez_dev_t *ezdev, 
					unsigned char * network_weight);

BOOLEAN ez_send_action_update_config_for_this_band(EZ_ADAPTER *ez_ad, 
	ez_dev_t *ezdev, 
	updated_configs_t *updated_configs, 
	BOOLEAN group_id_diff, 
	BOOLEAN band_switched, 
	BOOLEAN deauth_non_ez_sta);

BOOLEAN send_action_update_config(EZ_ADAPTER *ez_ad, 
					struct _ez_peer_security_info *ez_peer,
					ez_dev_t *ezdev, 
					updated_configs_t *updated_configs,
					BOOLEAN same_band,
					BOOLEAN group_id_update);

BOOLEAN send_action_custom_data(EZ_ADAPTER *ez_ad, 
					ez_dev_t *ezdev, 
					struct _ez_peer_security_info *ez_peer,
					ez_custom_data_cmd_t *data, 
					unsigned char datalen 
					);

void ez_hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen);


 BOOLEAN is_other_band_rcvd_pkt(ez_dev_t *ezdev,struct sk_buff *pSkb);

 BOOLEAN is_other_band_cli_rcvd_pkt(ez_dev_t *ezdev,struct sk_buff *pSkb);



/*
Determines whether Tx group packet is to be dropped by apcli interface

If both apcli interfaces connected to same root ap, then each apcli will
drop packets recvd on other band ap/apcli & allow any other packet.

*/
BOOLEAN ez_apcli_tx_grp_pkt_drop_hook(ez_dev_t *ezdev,struct sk_buff *pSkb);

/*
Determines whether Tx group packet is to be dropped by Ap interface
for duplicate CLI links ONLY
*/
BOOLEAN ez_ap_tx_grp_pkt_drop_to_ez_apcli(ez_dev_t *ezdev, struct sk_buff *pSkb);

void ez_apcli_uni_tx_on_dup_link(ez_dev_t *ezdev,struct sk_buff *pSkb);

/*
Determines whether Rx group packet is to be dropped by ApCli interface
for duplicate CLI links with NonEz AP
*/
BOOLEAN ez_apcli_rx_grp_pkt_drop(ez_dev_t *ezdev,UCHAR *pDestAddr);

/* Set/Clear Loop chk context on other band CLI */
void ez_set_other_band_cli_loop_chk_info(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev, BOOLEAN test_start);

/* Terminate Loop ck*/
void ez_cancel_loop_chk(ez_dev_t * ezdev);

INT Set_EasySetup_LoopPktSend(
	EZ_ADAPTER* ez_ad,
	RTMP_STRING *arg);

/* Trigger Loop check process when both CLI connected to non-easy root APs*/
void ez_chk_loop_thru_non_ez_ap(EZ_ADAPTER *ez_ad, ez_dev_t *ezdev);

/* Loop Check timeout handler*/
VOID ez_loop_chk_timeout(
	PVOID SystemSpecific1,
	PVOID FunctionContext,
	PVOID SystemSpecific2,
	PVOID SystemSpecific3);

/* Mark duplicate link and clear Loop chk context*/
 void ez_inform_other_band_cli_loop_detect( ez_dev_t * ezdev);


BOOLEAN ez_get_band( ez_dev_t * ezdev);



//void ez_update_ssid_pmk(EZ_ADAPTER *ez_ad, char * ssid, unsigned char ssid_len, unsigned char *pmk, struct _ez_peer_security_info  *from_peer);
void ez_update_ssid_pmk(EZ_ADAPTER *ez_ad, char * ssid1, unsigned char ssid_len1, unsigned char *psk1, unsigned char *pmk1	, 
								char * ssid2, unsigned char ssid_len2, unsigned char *psk2, unsigned char *pmk2, struct _ez_peer_security_info  *from_peer);
/* we assume the s1 and s2 both are strings.*/


BOOLEAN ezstrcasecmp(RTMP_STRING *s1, RTMP_STRING *s2);

VOID ez_setWdevAuthMode (
    struct __ez_triband_sec_config *pSecConfig, 
    RTMP_STRING *arg);



VOID ez_setWdevEncrypMode (
    struct __ez_triband_sec_config *pSecConfig, 
    RTMP_STRING *arg);

//void ez_update_ssid_pmk(RTMP_ADAPTER *pAd, char * ssid, unsigned char ssid_len, unsigned char *pmk, struct _ez_peer_security_info  *from_peer);
void ez_update_triband_ssid_pmk(EZ_ADAPTER *ez_ad, char * ssid1, unsigned char ssid_len1, unsigned char *pmk1	, unsigned char *psk1,
								char * ssid2, unsigned char ssid_len2, unsigned char *pmk2, unsigned char *psk2,
								char * ssid3, unsigned char ssid_len3, unsigned char *pmk3, unsigned char *psk3,
								char * encryptype1, char *encryptype2,	char * authmode1, char *authmode2);



/*!***************************************************************************
 * This routine build an outgoing frame, and fill all information specified
 * in argument list to the frame body. The actual frame size is the summation
 * of all arguments.
 * input params:
 *		Buffer - pointer to a pre-allocated memory segment
 *		args - a list of <int arg_size, arg> pairs.
 *		NOTE NOTE NOTE!!!! the last argument must be NULL, otherwise this
 *						   function will FAIL!!!
 * return:
 *		Size of the buffer
 * usage:
 *		MakeOutgoingFrame(Buffer, output_length, 2, &fc, 2, &dur, 6, p_addr1, 6,p_addr2, END_OF_ARGS);

 IRQL = PASSIVE_LEVEL
 IRQL = DISPATCH_LEVEL

 ****************************************************************************/
ULONG EzMakeOutgoingFrame(UCHAR *Buffer, ULONG *FrameLen, ...);

/*Unify Utility APIs*/
INT ez_os_alloc_mem(
	VOID *pAd,
	UCHAR **mem,
	ULONG size);

//#error check porting of malloc!!!
VOID ez_os_free_mem(
	PVOID mem);

static inline void NdisGetSystemUpTime(ULONG *time)
{
	*time = jiffies;
}

RTMP_STRING *EzGetAuthModeStr (
   UINT32 authMode);

RTMP_STRING *EzGetEncryModeStr(
    UINT32 encryMode);


void NonEzRtmpOSWrielessEventSend(
	void *ad,
	int band_id,
	UINT32 eventType,
	INT flags,
	PUCHAR pSrcMac,
	PUCHAR pData,
	UINT32 dataLen);

void EzRtmpOSWrielessEventSend(
	ez_dev_t * ezdev,
	UINT32 eventType,
	INT flags,
	PUCHAR pSrcMac,
	PUCHAR pData,
	UINT32 dataLen);



VOID EzActHeaderInit(
   	PHEADER_802_11 pHdr80211,
    UCHAR *da,
    UCHAR *sa,
    UCHAR *bssid);

void EzStartGroupMergeTimer(ez_dev_t* ezdev);

void ez_init_triband_config(void);

void ez_timer_init(ez_dev_t *ezdev, void *timer, void *callback);




/*this function is called to push own weight to all connected devices.*/
void update_and_push_weight(ez_dev_t *ezdev, char *peer_mac, unsigned char * network_weight);



INT Custom_EventHandle(ez_dev_t *ezdev, ez_custom_data_cmd_t *data, unsigned char datalen);

void ez_initiate_new_scan(EZ_ADAPTER *ez_ad);

BOOLEAN ez_is_triband_hook(void);

void ez_init_other_band_backup(ez_dev_t *ezdev, ez_dev_t *cli_ezdev);

