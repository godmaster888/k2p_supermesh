
EZ_ADAPTER * ez_init_hook(void *driver_ad, void *wdev, unsigned char ap_mode);

unsigned long ez_build_beacon_ie_hook(
	ez_dev_t *ezdev,
	unsigned char *frame_buf);

unsigned long ez_build_probe_request_ie_hook(
	ez_dev_t *ezdev,
	unsigned char *frame_buf);

unsigned long ez_build_probe_response_ie_hook(
	ez_dev_t *ezdev,
	unsigned char *frame_buf);

unsigned long ez_build_auth_request_ie_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	unsigned char *frame_buf);

unsigned long ez_build_auth_response_ie_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	unsigned char *frame_buf);


unsigned long ez_build_assoc_request_ie_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	unsigned char *frame_buf,
	unsigned int frame_buf_len);


unsigned long ez_build_assoc_response_ie_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	unsigned char *ap_gtk,
	unsigned int ap_gtk_len,
	unsigned char *frame_buf);

unsigned char ez_process_probe_request_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	void *msg,
	unsigned long msg_len);

void ez_process_beacon_probe_response_hook(
	ez_dev_t *ezdev,
	void *msg,
	unsigned long msg_len);

unsigned char ez_process_auth_request_hook(
	ez_dev_t *ezdev,
	void *auth_info_obj,
	void *msg,
	unsigned long msg_len);

USHORT ez_process_auth_response_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	void *msg,
	unsigned long msg_len);

unsigned short ez_process_assoc_request_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	UCHAR *easy_setup_mic_valid,
	unsigned char isReassoc,
	void *msg,
	unsigned long msg_len);

unsigned short ez_process_assoc_response_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	void *msg,
	unsigned long msg_len);


void ez_show_information_hook(
	ez_dev_t *ezdev);


INT ez_send_broadcast_deauth_proc_hook(ez_dev_t *ezdev);


unsigned char ez_set_ezgroup_id_hook(
	ez_dev_t *ezdev,
	unsigned char *ez_group_id,
	unsigned int ez_group_id_len,
	unsigned char inf_idx);



unsigned char ez_set_group_id_hook(
	ez_dev_t *ezdev,
	unsigned char *group_id,
	unsigned int group_id_len,
	unsigned char inf_idx);


unsigned char ez_set_gen_group_id_hook(
	ez_dev_t *ezdev,
	unsigned char *gen_group_id,
	unsigned int gen_group_id_len,
	unsigned char inf_idx);

void ez_set_rssi_threshold_hook(
	ez_dev_t *ezdev,
	char rssi_threshold);

void ez_set_max_scan_delay_hook(
	ez_dev_t *ezdev,
	UINT32 max_scan_delay);

void ez_set_api_mode_hook(
	ez_dev_t *ezdev,
	char ez_api_mode);

INT ez_merge_group_hook(ez_dev_t *ezdev, UCHAR *macAddress);

void ez_apcli_force_ssid_hook(
	ez_dev_t *ezdev,
	unsigned char *ssid, 
	unsigned char ssid_len);

void ez_set_force_bssid_hook(
	ez_dev_t *ezdev, 
	UCHAR *mac_addr);

void ez_set_push_bw_hook(ez_dev_t *ezdev, UINT8 same_bw_push);

BOOLEAN ez_is_loop_pkt_rcvd_hook(ez_dev_t *ezdev, 
	UINT8* loop_check_source, UINT8 * loop_check_cli);

void ez_handle_action_txstatus_hook(ez_dev_t *ezdev, UINT8 * Addr);

void set_ssid_psk_hook(ez_dev_t *ezdev, 
	char *ssid1, char *pmk1, char *psk1, 
	char *ssid2, char *pmk2, char *psk2, 
	char *ssid3, char *pmk3, char *psk3, 
	char *EncrypType1, char *EncrypType2, 
	char *AuthMode1, char *AuthMode2);


void ez_apcli_link_down_hook(ez_dev_t *ezdev,unsigned long Disconnect_Sub_Reason);

BOOLEAN ez_update_connection_permission_hook(
	ez_dev_t *ezdev, enum EZ_CONN_ACTION action);


BOOLEAN ez_is_connection_allowed_hook(ez_dev_t *ezdev);


BOOLEAN ez_probe_rsp_join_action_hook(ez_dev_t *ezdev, 
	char *network_weight);

void ez_update_connection_hook(ez_dev_t *ezdev);

void ez_handle_pairmsg4_hook(ez_dev_t *ezdev, UCHAR *peer_mac);
#ifdef EZ_DFS_SUPPORT
BOOLEAN ez_update_channel_from_csa_hook(ez_dev_t *ezdev, UCHAR Channel);
#endif
void ez_roam_hook(ez_dev_t *ezdev, 
	unsigned char bss_support_easy_setup,
	beacon_info_tag_t* bss_beacon_info,
	char *bss_bssid,
	UCHAR bss_channel);

BOOLEAN ez_set_roam_bssid_hook(ez_dev_t *ezdev, UCHAR *roam_bssid);

void ez_reset_roam_bssid_hook(ez_dev_t *ezdev);

channel_info_t *ez_get_channel_hook(ez_dev_t *ezdev);

BOOLEAN ez_get_push_bw_hook(ez_dev_t *ezdev);

BOOLEAN ez_did_ap_fallback_hook(ez_dev_t *ezdev);

BOOLEAN ez_ap_fallback_channel(ez_dev_t *ezdev);



void ez_prepare_security_key_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	unsigned char authenticator);

enum_group_merge_action_t is_group_merge_candidate_hooks(unsigned int easy_setup_capability, 
	ez_dev_t *ezdev, 
	void *temp_bss_entry,
	UCHAR *Bssid);


void ez_process_action_frame_hook(
	ez_dev_t *ezdev,
	UCHAR *peer_mac,
	UCHAR *Msg,
	UINT msg_len);

void ez_peer_table_maintenance_hook(EZ_ADAPTER *ez_ad);


BOOLEAN ez_port_secured_hook(
	ez_dev_t *ezdev,
	UCHAR *peer_mac,
	unsigned char ap_mode);

BOOLEAN check_best_ap_rssi_threshold_hook(ez_dev_t *ezdev, char rssi);


void ez_handle_peer_disconnection_hook(ez_dev_t *ezdev, unsigned char * mac_addr);
void ez_initiate_new_scan_hook(EZ_ADAPTER *ez_ad);


struct _ez_peer_security_info *ez_peer_table_search_by_addr_hook(
	ez_dev_t *ezdev,
	unsigned char *addr);

