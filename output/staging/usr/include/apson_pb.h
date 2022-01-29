/*
 * "apson_pb.c" is the wrapper interface for APSON module. It can clearly separate nanopb
 * library and our module for decreasing cost of future mataining. Using this api needs to
 * include "apson_pb.h" There will be the alltype data example apson_pb_example()
 * to make users know how to use. Otherwise, users also can refer to apsonpb document.
*/

#ifndef APSON_PB
#define APSON_PB


#ifndef _MESH_MODULE_H    /* modified for mesh module */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif /* _MESH_MODULE_H */


#include "./nanopb/pb_encode.h"
#include "./nanopb/pb_decode.h"



//+++ [Add the new message]step 1: Include the new header file
#include "./nanopb/apsonpbapi.pb.h"
#include "./nanopb/apson_register.pb.h"
#include "./nanopb/meshd.pb.h"
#include "./nanopb/apson_config.pb.h"
#include "./nanopb/profile.pb.h"
#include "./nanopb/apson_client.pb.h"
#include "./nanopb/apson_db.pb.h"
#include "./nanopb/apson_stats.pb.h"
#include "./nanopb/config.pb.h"
#include "./nanopb/auto_asgmt.pb.h"
#ifdef _SUPER_MESH_PRO_
#include "./nanopb/sp_config.pb.h"
#endif

//+++ [Add the new message]step 3: Wrap initialized default & zero define
/*wrapped initialization-default*/
#define EX_SUBMSG_INIT_DEFAULT                  Example_SubMessage_init_default
#define EX_ALLTYPES_INIT_DEFAULT                Example_alltypes_init_default
#define AP_REGISTER_INFO_INIT_DEFAULT           apson_register_info_init_default
#define MSD_CONFIG_INIT_DEFAULT                 meshd_config_init_default
#define AP_CONFIG_MSG_INIT_DEFAULT              apson_config_msg_init_default
#define AP_CONFIG_WLAN_INIT_DEFAULT             apson_config_wlan_init_default
#define MSD_SELF_INIT_DEFAULT                   meshd_self_s_init_default                
#define MSD_AP_LIST_INIT_DEFAULT                meshd_ap_list_s_init_default             
#define MSD_MESH_DEV_INIT_DEFAULT               meshd_mesh_dev_s_init_default           
#define MSD_MESH_IE_INIT_DEFAULT                meshd_mesh_ie_s_init_default
#define MSD_TIMEVAL_INIT_DEFAULT                meshd_timeval_s_init_default
#define PF_PROFILE_INIT_DEFAULT                 profile_profile_s_init_default           
#define PF_APSON_INIT_DEFAULT                   profile_apson_s_init_default             
#define PF_MESH_INIT_DEFAULT                    profile_mesh_s_init_default              
#define PF_MCS_TO_DETARATE_INIT_DEFAULT         profile_mcs_to_datarate_s_init_default   
#define PF_SNR_TO_MCS_INIT_DEFAULT              profile_snr_to_mcs_s_init_default
#define APSON_CLIENT_ACT_INIT_DEFAULT           apson_client_activity_init_default
#define AP_DB_WLAN_BIN_STATS_INIT_DEFAULT       apson_db_wlan_bin_stats_init_default
#define AP_DB_STA_BIN_STATS_INIT_DEFAULT        apson_db_station_bin_stats_init_default
#define AP_DB_AP_SYS_STATS_INIT_DEFAULT         apson_db_ap_system_stats_init_default
#define AP_DB_AP_RADIO_STATS_INIT_DEFAULT       apson_db_ap_radio_stats_init_default
#define AP_DB_AP_RADIO_STATS_LIST_INIT_DEFAULT  apson_db_ap_radio_stats_list_init_default
#define AP_DB_MA_WLAN_STATS_INIT_DEFAULT        apson_db_member_ap_wlan_stats_init_default
#define AP_DB_MA_WLAN_STATS_LIST_INIT_DEFAULT   apson_db_member_ap_wlan_stats_list_init_default
#define AP_DB_MA_STA_STATS_INIT_DEFAULT         apson_db_member_ap_station_stats_init_default
#define AP_DB_MA_STA_STATS_LIST_INIT_DEFAULT    apson_db_member_ap_station_stats_list_init_default
#define AP_DB_AP_STA_STATS_INIT_DEFAULT         apson_db_ap_station_stats_init_default
#define AP_DB_AP_STA_STATS_LIST_INIT_DEFAULT    apson_db_ap_station_stats_list_init_default 
#define AP_DB_AP_MESH_STATS_INIT_DEFAULT        apson_db_ap_mesh_stats_init_default
#define AP_DB_AP_MESH_DL_WLAN_INIT_DEFAULT      apson_db_ap_mesh_downlink_wlan_init_default
#define AP_DB_AP_MESH_NB_STATS_INIT_DEFAULT     apson_db_ap_mesh_neighbor_stats_init_default
#define AP_DB_AP_MESH_NB_STATS_LIST_INIT_DEFAULT                apson_db_ap_mesh_neighbor_stats_list_init_default  
#define AP_DB_AP_MESH_DL_AP_STATS_INIT_DEFAULT                  apson_db_ap_mesh_downlink_ap_stats_init_default
#define AP_DB_AP_MESH_DL_AP_STATS_LIST_INIT_DEFAULT             apson_db_ap_mesh_downlink_ap_stats_list_init_default
#define AP_DB_AP_WLAN_ACTIVITY_STATUS_INIT_DEFAULT              apson_db_ap_wlan_activity_status_init_default
#define AP_DB_AP_WLAN_LIST_STATUS_INIT_DEFAULT                  apson_db_ap_wlan_list_status_init_default
#define AP_DB_AP_WLAN_STATS_INIT_DEFAULT                        apson_db_ap_wlan_stats_init_default
#define AP_DB_AP_WLAN_STATS_LIST_INIT_DEFAULT                   apson_db_ap_wlan_stats_list_init_default
#define AP_DB_MESH_MODULE_MSG_CLIENT_ASSOCIATE_INIT_DEFAULT     apson_db_mesh_module_msg_client_associate_init_default
#define AP_DB_MESH_MODULE_MSG_CLIENT_AUTHORIZED_INIT_DEFAULT    apson_db_mesh_module_msg_client_authorized_init_default 
#define AP_DB_MESH_MODULE_MSG_IP_ASSIGNED_INIT_DEFAULT          apson_db_mesh_module_msg_ip_assigned_init_default
#define AP_DB_MESH_MODULE_MSG_CLIENT_DISCONNECT_INIT_DEFAULT    apson_db_mesh_module_msg_client_disconnect_init_default
#define AP_DB_MESH_MODULE_MSG_UL_CONNECT_INIT_DEFAULT           apson_db_mesh_module_msg_uplink_connect_init_default
#define AP_DB_MESH_MODULE_MSG_UL_AUTHORIZED_INIT_DEFAULT        apson_db_mesh_module_msg_uplink_authorized_init_default
#define AP_DB_MESH_MODULE_MSG_UL_DISCONNECT_INIT_DEFAULT        apson_db_mesh_module_msg_uplink_disconnect_init_default
#define AP_DB_MESH_MODULE_MSG_WEAK_CLIENT_INFO_INIT_DEFAULT     apson_db_mesh_module_msg_weak_client_info_init_default
#define AP_DB_MESH_MODULE_CMD_SET_MONITOR_CLIENT_INIT_DEFAULT   apson_db_mesh_module_cmd_set_monitor_client_init_default
#define AP_DB_MESH_MODULE_MSG_MONITOR_CLIENT_INFO_INIT_DEFAULT  apson_db_mesh_module_msg_monitor_client_info_init_default
#define AP_DB_MODULE_CMD_SET_CLIENT_CRITERIA_INIT_DEFAULT       apson_db_module_cmd_set_client_criteria_init_default
#define AP_DB_APSON_MESH_INFO_INIT_DEFAULT                      apson_db_apson_mesh_info_init_default
#define AP_STATS_REBOOT_AP_INIT_DEFAULT                 ap_stats_reboot_ap_init_default       
#define AP_STATS_KICKOUT_CLIENT_INIT_DEFAULT            ap_stats_kickout_client_init_default  
#define AP_STATS_SPECIFIC_AP_IMG_INFO_INIT_DEFAULT      ap_stats_specific_target_ap_image_info_init_default
#define AP_STATS_LAST_RELEASED_IMG_INFO_INIT_DEFAULT    ap_stats_last_released_image_info_init_default
#define AP_STATS_AP_IMG_UPGRADE_INIT_DEFAULT            ap_stats_ap_image_upgrade_init_default
#define AP_STATS_AP_STATS_INIT_DEFAULT                  ap_stats_ap_stats_init_default
#define AP_STATS_WLAN_STATS_INIT_DEFAULT                ap_stats_wlan_stats_init_default
#define AP_STATS_STA_STATS_INIT_DEFAULT                 ap_stats_sta_stats_init_default
#define AP_STATS_MESH_STATS_INIT_DEFAULT                ap_stats_mesh_stats_init_default
#define AP_STATS_SYS_WIDTH_STATS_INIT_DEFAULT           ap_stats_system_width_stats_init_default
#define AP_DB_AP_LIST_INIT_DEFAULT                      apson_db_ap_list_init_default
#define CFG_INIT_DEFAULT                                config_init_default
#define CFG_APSON_INIT_DEFAULT                          apson_s_init_default
#define CFG_WLAN_INFO_INIT_DEFAULT                      wlan_info_s_init_default
#define CFG_WLAN_INIT_DEFAULT                           wlan_s_init_default
#define CFG_RADIO_INIT_DEFAULT                          radio_s_init_default
#define CFG_MESH_INIT_DEFAULT                           mesh_s_init_default
#define CFG_PROFILE_INIT_DEFAULT                        profile_s_init_default
#define CFG_TM_INFO_INIT_DEFAULT                        time_info_s_init_default
#define CFG_SCHE_INFO_INIT_DEFAULT                      schedule_info_s_init_default
#define CFG_SCHE_LIST_INIT_DEFAULT                      schedule_list_s_init_default
#define CFG_WPS_INIT_DEFAULT                            wps_s_init_default
#define CFG_MAC_INFO_INIT_DEFAULT                       mac_info_s_init_default
#define CFG_MAC_FLT_INIT_DEFAULT                        mac_filter_s_init_default
#define CFG_MAC_DEL_FLT_INIT_DEFAULT                    mac_del_filter_init_default
#define CFG_TM_SET_INIT_DEFAULT                         time_setting_s_init_default
#define CFG_DEV_SET_INIT_DEFAULT                        dev_setting_s_init_default
#define CFG_RADIO_INFO_INIT_DEFAULT                     radio_info_s_init_default
#define AP_STATS_BIN_STATS_INIT_DEFAULT                 ap_stats_bin_stats_init_default
#define AP_STATS_BIN_DATA_INIT_DEFAULT                  ap_stats_bin_data_init_default
#define APSON_MESH_DL_AP_ACT_INIT_DEFAULT               apson_db_mesh_dl_ap_activity_init_default
#define AUTO_ASGMT_SITE_SURVEY_INFO_INIT_DEFAULT        auto_asgmt_site_survey_info_init_default
#define AP_DB_CLIENT_BEACON_MEASURE_INIT_DEFAULT        apson_db_client_beacon_measure_init_default
#define AP_DB_BEACON_REPORT_INIT_DEFAULT                apson_db_beacon_report_init_default

#ifdef _SUPER_MESH_PRO_
#define SP_CFG_WLAN_INFO_INIT_DEFAULT           sp_config_wlan_info_init_default
#define SP_CFG_WLAN_INIT_DEFAULT                sp_config_wlan_init_default
#define SP_CFG_WLAN_BSC_SET_INIT_DEFAULT        sp_config_wlan_bsc_set_init_default
#define SP_CFG_WLAN_SCY_SET_INIT_DEFAULT        sp_config_wlan_scy_set_init_default
#define SP_CFG_WLAN_ADVS_SET_INIT_DEFAULT       sp_config_wlan_advs_set_init_default
#define SP_CFG_RADIO_INIT_DEFAULT               sp_config_radio_init_default
#define SP_CFG_RADIO_BASIC_SET_INIT_DEFAULT     sp_config_radio_basic_set_init_default
#define SP_CFG_RADIO_RSRC_INIT_DEFAULT          sp_config_radio_rsrc_init_default
#define SP_CFG_MESH_INIT_DEFAULT                sp_config_mesh_init_default
#define SP_CFG_CAPTIVE_PORTAL_INIT_DEFAULT      sp_config_captive_portal_init_default
#define SP_CFG_CP_SETTING_INIT_DEFAULT          sp_config_cp_setting_init_default
#define SP_CFG_IP_FILTER_INIT_DEFAULT           sp_config_ip_filter_init_default
#define SP_CFG_MAC_BYPASS_INIT_DEFAULT          sp_config_mac_bypass_init_default
#define SP_CFG_ACL_INIT_DEFAULT                 sp_config_acl_init_default
#define SP_CFG_RADIO_SETTING_INIT_DEFAULT       sp_config_radio_setting_init_default
#define SP_CFG_CLIENT_ACL_LIST_INIT_DEFAULT     sp_config_client_acl_list_init_default
#define SP_CFG_QOS_INIT_DEFAULT                 sp_config_qos_init_default
#define SP_CFG_QOS_BSC_SET_INIT_DEFAULT         sp_config_qos_bsc_set_init_default
#define SP_CFG_QOS_RULE_INIT_DEFAULT            sp_config_qos_rule_init_default
#define SP_CFG_VLAN_INIT_DEFAULT                sp_config_vlan_init_default
#define SP_CFG_VLAN_LST_INIT_DEFAULT            sp_config_vlan_lst_init_default
#define SP_CFG_PVID_SET_INIT_DEFAULT            sp_config_pvid_set_init_default
#define SP_CFG_ARP_FILTER_INIT_DEFAULT          sp_config_arp_filter_init_default
#define SP_CFG_GATEWAY_LIST_INIT_DEFAULT        sp_config_gateway_list_init_default
#define SP_CFG_TM_SCHE_INIT_DEFAULT             sp_config_tm_sche_init_default
#define SP_CFG_TM_RULE_LST_INIT_DEFAULT         sp_config_tm_rule_lst_init_default
#define SP_CFG_WLAN_PARTITION_INIT_DEFAULT      sp_config_wlan_partition_init_default
#define SP_CFG_BAND_INFO_INIT_DEFAULT           sp_config_band_info_init_default
#define SP_CFG_DEVICE_MGMT_INIT_DEFAULT         sp_config_device_mgmt_init_default
#define SP_CFG_CONFIG_INIT_DEFAULT              sp_config_config_init_default
#define SP_CFG_AP_ARRAY_SETTING_INIT_DEFAULT    sp_config_ap_array_setting_init_default
#define SP_CFG_MESH_LOCAL_SET_INIT_DEFAULT      sp_config_mesh_local_set_init_default
#endif

/*wrapped initialization-zero*/
#define EX_SUBMSG_INIT_ZERO                 Example_SubMessage_init_zero
#define EX_ALLTYPES_INIT_ZERO               Example_alltypes_init_zero
#define AP_REGISTER_INFO_INIT_ZERO          apson_register_info_init_zero
#define MSD_CONFIG_INIT_ZERO                meshd_config_init_zero
#define AP_CONFIG_MSG_INIT_ZERO             apson_config_msg_init_zero
#define AP_CONFIG_WLAN_INIT_ZERO            apson_config_wlan_init_zero
#define MSD_SELF_INIT_ZERO                  meshd_self_s_init_zero
#define MSD_AP_LIST_INIT_ZERO               meshd_ap_list_s_init_zero
#define MSD_MESH_DEV_INIT_ZERO              meshd_mesh_dev_s_init_zero
#define MSD_MESH_IE_INIT_ZERO               meshd_mesh_ie_s_init_zero
#define MSD_TIMEVAL_INIT_ZERO               meshd_timeval_s_init_zero
#define PF_PROFILE_INIT_ZERO                profile_profile_s_init_zero              
#define PF_APSON_INIT_ZERO                  profile_apson_s_init_zero                
#define PF_MESH_INIT_ZERO                   profile_mesh_s_init_zero                 
#define PF_MCS_TO_DATARATE_INIT_ZERO        profile_mcs_to_datarate_s_init_zero      
#define PF_SNR_TO_MCS_INIT_ZERO             profile_snr_to_mcs_s_init_zero   
#define APSON_CLIENT_ACT_INIT_ZERO          apson_client_activity_init_zero
#define AP_DB_WLAN_BIN_STATS_INIT_ZERO          apson_db_wlan_bin_stats_init_zero
#define AP_DB_STA_BIN_STATS_INIT_ZERO           apson_db_station_bin_stats_init_zero
#define AP_DB_AP_SYS_STATS_INIT_ZERO            apson_db_ap_system_stats_init_zero
#define AP_DB_AP_RADIO_STATS_INIT_ZERO          apson_db_ap_radio_stats_init_zero
#define AP_DB_AP_RADIO_STATS_LIST_INIT_ZERO     apson_db_ap_radio_stats_list_init_zero
#define AP_DB_MA_WLAN_STATS_INIT_ZERO           apson_db_member_ap_wlan_stats_init_zero
#define AP_DB_MA_WLAN_STATS_LIST_INIT_ZERO      apson_db_member_ap_wlan_stats_list_init_zero
#define AP_DB_MA_STA_STATS_INIT_ZERO            apson_db_member_ap_station_stats_init_zero
#define AP_DB_MA_STA_STATS_LIST_INIT_ZERO       apson_db_member_ap_station_stats_list_init_zero
#define AP_DB_AP_STA_STATS_INIT_ZERO            apson_db_ap_station_stats_init_zero
#define AP_DB_AP_STA_STATS_LIST_INIT_ZERO      apson_db_ap_station_stats_list_init_zero 
#define AP_DB_AP_MESH_STATS_INIT_ZERO          apson_db_ap_mesh_stats_init_zero
#define AP_DB_AP_MESH_DL_WLAN_INIT_ZERO         apson_db_ap_mesh_downlink_wlan_init_zero
#define AP_DB_AP_MESH_NB_STATS_INIT_ZERO        apson_db_ap_mesh_neighbor_stats_init_zero
#define AP_DB_AP_MESH_NB_STATS_LIST_INIT_ZERO   apson_db_ap_mesh_neighbor_stats_list_init_zero
#define AP_DB_AP_MESH_DL_AP_STATS_INIT_ZERO     apson_db_ap_mesh_downlink_ap_stats_init_zero
#define AP_DB_AP_MESH_DL_AP_STATS_LIST_INIT_ZERO                apson_db_ap_mesh_downlink_ap_stats_list_init_zero
#define AP_DB_AP_WLAN_ACTIVITY_STATUS_INIT_ZERO                 apson_db_ap_wlan_activity_status_init_zero
#define AP_DB_AP_WLAN_LIST_STATUS_INIT_ZERO                     apson_db_ap_wlan_list_status_init_zero
#define AP_DB_AP_WLAN_STATS_INIT_ZERO                           apson_db_ap_wlan_stats_init_zero
#define AP_DB_AP_WLAN_STATS_LIST_INIT_ZERO                      apson_db_ap_wlan_stats_list_init_zero
#define AP_DB_MESH_MODULE_MSG_CLIENT_ASSOCIATE_INIT_ZERO        apson_db_mesh_module_msg_client_associate_init_zero
#define AP_DB_MESH_MODULE_MSG_CLIENT_AUTHORIZED_INIT_ZERO       apson_db_mesh_module_msg_client_authorized_init_zero
#define AP_DB_MESH_MODULE_MSG_IP_ASSIGNED_INIT_ZERO             apson_db_mesh_module_msg_ip_assigned_init_zero
#define AP_DB_MESH_MODULE_MSG_CLIENT_DISCONNECT_INIT_ZERO       apson_db_mesh_module_msg_client_disconnect_init_zero
#define AP_DB_MESH_MODULE_MSG_UL_CONNECT_INIT_ZERO              apson_db_mesh_module_msg_uplink_connect_init_zero
#define AP_DB_MESH_MODULE_MSG_UL_AUTHORIZED_INIT_ZERO           apson_db_mesh_module_msg_uplink_authorized_init_zero
#define AP_DB_MESH_MODULE_MSG_UL_DISCONNECT_INIT_ZERO           apson_db_mesh_module_msg_uplink_disconnect_init_zero
#define AP_DB_MESH_MODULE_MSG_WEAK_CLIENT_INFO_INIT_ZERO        apson_db_mesh_module_msg_weak_client_info_init_zero
#define AP_DB_MESH_MODULE_CMD_SET_MONITOR_CLIENT_INIT_ZERO      apson_db_mesh_module_cmd_set_monitor_client_init_zero
#define AP_DB_MESH_MODULE_MSG_MONITOR_CLIENT_INFO_INIT_ZERO     apson_db_mesh_module_msg_monitor_client_info_init_zero
#define AP_DB_MODULE_CMD_SET_CLIENT_CRITERIA_INIT_ZERO          apson_db_module_cmd_set_client_criteria_init_zero
#define AP_DB_APSON_MESH_INFO_INIT_ZERO                         apson_db_apson_mesh_info_init_zero
#define AP_STATS_REBOOT_AP_INIT_ZERO                ap_stats_reboot_ap_init_zero
#define AP_STATS_KICKOUT_CLIENT_INIT_ZERO           ap_stats_kickout_client_init_zero
#define AP_STATS_SPECIFIC_AP_IMG_INFO_INIT_ZERO     ap_stats_specific_target_ap_image_info_init_zero 
#define AP_STATS_LAST_RELEASED_IMG_INFO_INIT_ZERO   ap_stats_last_released_image_info_init_zero 
#define AP_STATS_AP_IMG_UPGRADE_INIT_ZERO           ap_stats_ap_image_upgrade_init_zero   
#define AP_STATS_AP_STATS_INIT_ZERO                 ap_stats_ap_stats_init_zero           
#define AP_STATS_WLAN_STATS_INIT_ZERO               ap_stats_wlan_stats_init_zero         
#define AP_STATS_STA_STATS_INIT_ZERO                ap_stats_sta_stats_init_zero          
#define AP_STATS_MESH_STATS_INIT_ZERO               ap_stats_mesh_stats_init_zero         
#define AP_STATS_SYS_WIDTH_STATS_INIT_ZERO          ap_stats_system_width_stats_init_zero
#define AP_DB_AP_LIST_INIT_ZERO                     apson_db_ap_list_init_zero
#define CFG_INIT_ZERO                               config_init_zero
#define CFG_APSON_INIT_ZERO                         apson_s_init_zero
#define CFG_WLAN_INFO_INIT_ZERO                     wlan_info_s_init_zero
#define CFG_WLAN_INIT_ZERO                          wlan_s_init_zero
#define CFG_RADIO_INIT_ZERO                         radio_s_init_zero
#define CFG_MESH_INIT_ZERP                          mesh_s_init_zero
#define CFG_PROFILE_INIT_ZERO                       profile_s_init_zero
#define CFG_TM_INFO_INIT_ZERO                       time_info_s_init_zero
#define CFG_SCHE_INFO_INIT_ZERO                     schedule_info_s_init_zero
#define CFG_SCHE_LIST_INIT_ZERO                     schedule_list_s_init_zero
#define CFG_WPS_INIT_ZERO                           wps_s_init_zero
#define CFG_MAC_INFO_INIT_ZERO                      mac_info_s_init_zero
#define CFG_MAC_FLT_INIT_ZERO                       mac_filter_s_init_zero
#define CFG_MAC_DEL_FLT_INIT_ZERO                   mac_del_filter_init_zero
#define CFG_TM_SET_INIT_ZERO                        time_setting_s_init_zero
#define CFG_DEV_SET_INIT_ZERO                       dev_setting_s_init_zero
#define CFG_RADIO_INFO_INIT_ZERO                    radio_info_s_init_zero
#define AP_STATS_BIN_STATS_INIT_ZERO                ap_stats_bin_stats_init_zero
#define AP_STATS_BIN_DATA_INIT_ZERO                 ap_stats_bin_data_init_zero
#define APSON_MESH_DL_AP_ACT_INIT_ZERO              apson_db_mesh_dl_ap_activity_init_zero
#define AUTO_ASGMT_SITE_SURVEY_INFO_INIT_ZERO       auto_asgmt_site_survey_info_init_zero
#define AP_DB_CLIENT_BEACON_MEASURE_INIT_ZERO       apson_db_client_beacon_measure_init_zero
#define AP_DB_BEACON_REPORT_INIT_ZERO               apson_db_beacon_report_init_zero

#ifdef _SUPER_MESH_PRO_
#define SP_CFG_WLAN_INFO_INIT_ZERO          sp_config_wlan_info_init_zero
#define SP_CFG_WLAN_INIT_ZERO               sp_config_wlan_init_zero
#define SP_CFG_WLAN_BSC_SET_INIT_ZERO       sp_config_wlan_bsc_set_init_zero
#define SP_CFG_WLAN_SCY_SET_INIT_ZERO       sp_config_wlan_scy_set_init_zero
#define SP_CFG_WLAN_ADVS_SET_INIT_ZERO      sp_config_wlan_advs_set_init_zero
#define SP_CFG_RADIO_INIT_ZERO              sp_config_radio_init_zero
#define SP_CFG_RADIO_BASIC_SET_INIT_ZERO    sp_config_radio_basic_set_init_zero
#define SP_CFG_RADIO_RSRC_INIT_ZERO         sp_config_radio_rsrc_init_zero
#define SP_CFG_MESH_INIT_ZERO               sp_config_mesh_init_zero
#define SP_CFG_CAPTIVE_PORTAL_INIT_ZERO     sp_config_captive_portal_init_zero
#define SP_CFG_CP_SETTING_INIT_ZERO         sp_config_cp_setting_init_zero
#define SP_CFG_IP_FILTER_INIT_ZERO          sp_config_ip_filter_init_zero
#define SP_CFG_MAC_BYPASS_INIT_ZERO         sp_config_mac_bypass_init_zero
#define SP_CFG_ACL_INIT_ZERO                sp_config_acl_init_zero
#define SP_CFG_RADIO_SETTING_INIT_ZERO      sp_config_radio_setting_init_zero
#define SP_CFG_CLIENT_ACL_LIST_INIT_ZERO    sp_config_client_acl_list_init_zero
#define SP_CFG_QOS_INIT_ZERO                sp_config_qos_init_zero
#define SP_CFG_QOS_BSC_SET_INIT_ZERO        sp_config_qos_bsc_set_init_zero
#define SP_CFG_QOS_RULE_INIT_ZERO           sp_config_qos_rule_init_zero
#define SP_CFG_VLAN_INIT_ZERO               sp_config_vlan_init_zero
#define SP_CFG_VLAN_LST_INIT_ZERO           sp_config_vlan_lst_init_zero
#define SP_CFG_PVID_SET_INIT_ZERO           sp_config_pvid_set_init_zero
#define SP_CFG_ARP_FILTER_INIT_ZERO         sp_config_arp_filter_init_zero
#define SP_CFG_GATEWAY_LIST_INIT_ZERO       sp_config_gateway_list_init_zero
#define SP_CFG_TM_SCHE_INIT_ZERO            sp_config_tm_sche_init_zero
#define SP_CFG_TM_RULE_LST_INIT_ZERO        sp_config_tm_rule_lst_init_zero
#define SP_CFG_WLAN_PARTITION_INIT_ZERO     sp_config_wlan_partition_init_zero
#define SP_CFG_BAND_INFO_INIT_ZERO          sp_config_band_info_init_zero
#define SP_CFG_DEVICE_MGMT_INIT_ZERO        sp_config_device_mgmt_init_zero
#define SP_CFG_CONFIG_INIT_ZERO             sp_config_config_init_zero
#define SP_CFG_AP_ARRAY_SETTING_INIT_ZERO   sp_config_ap_array_setting_init_zero
#define SP_CFG_MESH_LOCAL_SET_INIT_ZERO     sp_config_mesh_local_set_init_zero
#endif



//+++ [Add the new message]step 2: Add the new message name into enumeration including submessage
/*Define the message name*/
typedef enum{
    PB_EXAMPLE_ALLTYPES = 0,                            /* 1.  apsonpbapi.proto: alltypes(pb example) */
    PB_APSON_REGISTER_INFO,                             /* 2.  apson_register.proto: info */
    PB_MESHD_CONFIG,                                    /* 3.  meshd.proto: config */
    PB_APSON_CONFIG_MSG,                                /* 4.  apson_config.proto: msg */
    PB_APSON_CONFIG_WLAN,                               /* 5.  apson_config.proto: wlan */
    PB_MSD_SELF,                                        /* 6.  meshd.proto: self_s */
    PB_MSD_AP_LIST,                                     /* 7.  meshd.proto: ap_list_s */
    PB_MSD_MESH_DEV,                                    /* 8.  meshd.proto: mesh_dev_s */
    PB_MSD_MESH_IE,                                     /* 9.  meshd.proto: mesh_ie_s */
    PB_MSD_TIMEVAL,                                     /* 10. meshd.proto: timeval_s */
    PB_PROFILE,                                         /* 11. profile.proto: profile_s */
    PB_PF_APSON,                                        /* 12. profile.proto: apson_s */
    PB_PF_MESH,                                         /* 13. profile.proto: mesh_s */
    PB_PF_MCS_DATARATE,                                 /* 14. profile.proto: mcs_to_datarate_s */
    PB_PF_SNR_MCS,                                      /* 15. profile.proto: snr_to_mcs_s */
    PB_APSON_CLIENT_ACTIVITY,                           /* 16. apson_client.proto: activity */
    PB_AP_DB_WLAN_BIN_STATS,                            /* 17. apson_db.proto: wlan_bin_stats */
    PB_AP_DB_STA_BIN_STATS,                             /* 18. apson_db.proto: station_bin_stats */
    PB_AP_DB_AP_SYS_STATS,                              /* 19. apson_db.proto: ap_system_stats */
    PB_AP_DB_AP_RADIO_STATS,                            /* 20. apson_db.proto: ap_radio_stats */
    PB_AP_DB_AP_RADIO_STATS_LIST,                       /* 21. apson_db.proto: ap_radio_stats_list */
    PB_AP_DB_MA_WLAN_STATS,                             /* 22. apson_db.proto: member_ap_wlan_stats */
    PB_AP_DB_MA_WLAN_STATS_LIST,                        /* 23. apson_db.proto: member_ap_wlan_stats_list */
    PB_AP_DB_MA_STA_STATS,                              /* 24. apson_db.proto: member_ap_station_stats */
    PB_AP_DB_MA_STA_STATS_LIST,                         /* 25. apson_db.proto: member_ap_station_stats_list */
    PB_AP_DB_AP_STA_STATS,                              /* 26. apson_db.proto: ap_station_stats */
    PB_AP_DB_AP_STA_STATS_LIST,                         /* 27. apson_db.proto: ap_station_stats_list */
    PB_AP_DB_AP_MESH_STATS,                             /* 28. apson_db.proto: ap_mesh_stats */
    PB_AP_DB_AP_MESH_DL_WLAN,                           /* 29. apson_db.proto: ap_mesh_downlink_wlan */
    PB_AP_DB_AP_MESH_NB_STATS,                          /* 30. apson_db.proto: ap_mesh_neighbor_stats */
    PB_AP_DB_AP_MESH_NB_STATS_LIST,                     /* 31. apson_db.proto: ap_mesh_neighbor_stats_list */
    PB_AP_DB_AP_MESH_DL_AP_STATS,                       /* 32. apson_db.proto: ap_mesh_downlink_ap_stats */
    PB_AP_DB_AP_MESH_DL_AP_STATS_LIST,                  /* 33. apson_db.proto: ap_mesh_downlink_ap_stats_list */
    PB_AP_DB_AP_WLAN_ACTIVITY_STATUS,                   /* 34. apson_db.proto: ap_wlan_activity_status */
    PB_AP_DB_AP_WLAN_LIST_STATUS,                       /* 35. apson_db.proto: ap_wlan_list_status */
    PB_AP_DB_AP_WLAN_STATS,                             /* 36. apson_db.proto: ap_wlan_stats */
    PB_AP_DB_AP_WLAN_STATS_LIST,                        /* 37. apson_db.proto: ap_wlan_stats_list */
    PB_AP_DB_MESH_MODULE_MSG_CLIENT_ASSOCIATE,          /* 38. apson_db.proto: mesh_module_msg_client_associate */
    PB_AP_DB_MESH_MODULE_MSG_CLIENT_AUTHORIZED,         /* 39. apson_db.proto: mesh_module_msg_client_authorized */
    PB_AP_DB_MESH_MODULE_MSG_IP_ASSIGNED,               /* 40. apson_db.proto: mesh_module_msg_ip_assigned */
    PB_AP_DB_MESH_MODULE_MSG_CLIENT_DISCONNECT,         /* 41. apson_db.proto: mesh_module_msg_client_disconnect */
    PB_AP_DB_MESH_MODULE_MSG_UL_CONNECT,                /* 42. apson_db.proto: mesh_module_msg_uplink_connect */
    PB_AP_DB_MESH_MODULE_MSG_UL_AUTHORIZED,             /* 43. apson_db.proto: mesh_module_msg_uplink_authorized */
    PB_AP_DB_MESH_MODULE_MSG_UL_DISCONNECT,             /* 44. apson_db.proto: mesh_module_msg_uplink_disconnect */
    PB_AP_DB_MESH_MODULE_MSG_WEAK_CLIENT_INFO,          /* 45. apson_db.proto: mesh_module_msg_weak_client_info */
    PB_AP_DB_MESH_MODULE_CMD_SET_MONITOR_CLIENT,        /* 46. apson_db.proto: mesh_module_cmd_set_monitor_client */
    PB_AP_DB_MESH_MODULE_MSG_MONITOR_CLIENT_INFO,       /* 47. apson_db.proto: mesh_module_msg_monitor_client_info*/
    PB_AP_DB_MODULE_CMD_SET_CLIENT_CRITERIA,            /* 48. apson_db.proto: module_cmd_set_client_criteria*/
    PB_AP_DB_APSON_MESH_INFO,                           /* 49. apson_db.proto: apson_mesh_info */
    PB_AP_STATS_REBOOT_AP,                              /* 50. apson_stats.proto: reboot_ap */
    PB_AP_STATS_KICKOUT_CLIENT,                         /* 51. apson_stats.proto: kickout_client */
    PB_AP_STATS_SPE_AP_IMG_INFO,                        /* 52. apson_stats.proto: specific_target_ap_image_info */
    PB_AP_STATS_LAST_REL_IMG_INFO,                      /* 53. apson_stats.proto: last_released_image_info */
    PB_AP_STATS_AP_IMG_UPGRADE,                         /* 54. apson_stats.proto: ap_image_upgrade */
    PB_AP_STATS_AP_STATS,                               /* 55. apson_stats.proto: ap_stats */
    PB_AP_STATS_WLAN_STATS,                             /* 56. apson_stats.proto: wlan_stats */
    PB_AP_STATS_STA_STATS,                              /* 57. apson_stats.proto: sta_stats */
    PB_AP_STATS_MESH_STATS,                             /* 58. apson_stats.proto: mesh_stats */
    PB_AP_STATS_SYS_WIDTH_STATS,                        /* 59. apson_stats.proto: system_width_stats */
    PB_AP_DB_AP_LIST,                                   /* 60. apson_db.proto: ap_list */
    PB_CFG,                                             /* 61. config.proto: config */
    PB_CFG_APSON,                                       /* 62. config.proto: apson_s */
    PB_CFG_WLAN_INFO,                                   /* 63. config.proto: wlan_info_s */
    PB_CFG_WLAN,                                        /* 64. config.proto: wlan_s */
    PB_CFG_RADIO,                                       /* 65. config.proto: radio_s */
    PB_CFG_MESH,                                        /* 66. config.proto: mesh_s */
    PB_CFG_PROFILE,                                     /* 67. config.proto: profile_s */
    PB_CFG_TM_INFO,                                     /* 68. config.proto: time_info_s */
    PB_CFG_SCHE_INFO,                                   /* 69. config.proto: schedule_info_s */
    PB_CFG_SCHE_LIST,                                   /* 70. config.proto: schedule_list_s */
    PB_CFG_SCHE_SET,                                    /* 71. config.proto: schedule_setting_s */
    PB_CFG_WPS,                                         /* 72. config.proto: wps_s */
    PB_CFG_MAC_INFO,                                    /* 73. config.proto: mac_info_s */
    PB_CFG_MAC_FLT,                                     /* 74. config.proto: mac_filter_s */
    PB_CFG_DEL_FLT,                                     /* 75. config.proto: mac_del_filter */
    PB_CFG_TM_SET,                                      /* 76. config.proto: time_setting_s */
    PB_CFG_DEV_SET,                                     /* 77. config.proto: dev_setting_s */
    PB_CFG_RADIO_INFO,                                  /* 78. config.proto: radio_info_s */
    PB_AP_STATS_BIN_STATS,                              /* 79. apson_stats.proto: bin_stats */
    PB_AP_BIN_DATA,                                     /* 80. apson_stats.proto: bin_data */
    PB_APSON_MESH_DL_AP_ACTIVITY,                       /* 81. apson_db.proto: mesh_dl_ap_activity*/
    PB_AUTO_ASGMT_SITE_SVY_INFO,                        /* 82. auto_asgmt.proto: site_survey_info */
    PB_AP_DB_CLI_BCN_MEASURE,                           /* 83. apson_db.proto: apson_db_client_beacon_measure */
    PB_AP_DB_BCN_RPT,                                   /* 84. apson_db.proto: apson_db_beacon_report */
#ifdef _SUPER_MESH_PRO_
    PB_SP_CFG_WLAN_INFO,
    PB_SP_CFG_WLAN,
    PB_SP_CFG_WLAN_BSC_SET,
    PB_SP_CFG_WLAN_SCY_SET,
    PB_SP_CFG_WLAN_ADVS_SET,
    PB_SP_CFG_RADIO,
    PB_SP_CFG_RADIO_BASIC_SET,
    PB_SP_CFG_RADIO_RSRC,
    PB_SP_CFG_MESH,
    PB_SP_CFG_CAPTIVE_PORTAL,
    PB_SP_CFG_CP_SETTING,
    PB_SP_CFG_IP_FILTER,
    PB_SP_CFG_MAC_BYPASS,
    PB_SP_CFG_ACL,
    PB_SP_CFG_RADIO_SETTING,
    PB_SP_CFG_CLIENT_ACL_LIST,
    PB_SP_CFG_QOS,
    PB_SP_CFG_QOS_BSC_SET,
    PB_SP_CFG_QOS_RULE,
    PB_SP_CFG_VLAN,
    PB_SP_CFG_VLAN_LST,
    PB_SP_CFG_PVID_SET,
    PB_SP_CFG_ARP_FILTER,
    PB_SP_CFG_GATEWAY_LIST,
    PB_SP_CFG_TM_SCHE,
    PB_SP_CFG_TM_RULE_LST,
    PB_SP_CFG_WLAN_PARTITION,
    PB_SP_CFG_BAND_INFO,
    PB_SP_CFG_DEVICE_MGMT,
    PB_SP_CFG_CONFIG,
    PB_SP_CFG_AP_ARRAY_SET,
    PB_SP_CFG_MESH_LOCAL_SET,
#endif
    PB_CFG_ROAM,                                        /* ??. config.proto: roam_s */
    PB_NULL,
    PB_LAST}PB_MSG;



//+++ [Add the new message]step 4: Wrap structure by using typedef
/*********************Define wrapped message structure*********************/

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped Example_SubMessage structure.
*
*       typedef struct _Example_SubMessage {
*           bool has_substring;
*           char substring[16];
*           bool has_subint32;
*           int32_t subint32;
*       } Example_SubMessage;
*
* NOTES :
*
*H************************************************************************/
typedef Example_SubMessage EX_SUBMSG;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped Example_alltypes structure.
*
*       typedef struct _Example_alltypes {
*           int32_t opt_int32;
*           int64_t opt_int64;
*           uint32_t opt_uint32;
*           uint64_t opt_uint64;
*           int32_t opt_sint32;
*           int64_t opt_sint64;
*           bool opt_bool;
*           float opt_float;
*           double opt_double;
*           Example_ExEnum opt_enum;
*           char opt_string[16];
*           Example_alltypes_opt_bytes_t opt_bytes;
*           Example_SubMessage opt_submsg;
*           pb_size_t rep_int32_count;
*           int32_t rep_int32[3];
*           pb_size_t rep_submsg_count;
*           Example_SubMessage rep_submsg[5];
*       } Example_alltypes;
*
*
* NOTES :
*
*H************************************************************************/
typedef Example_alltypes EX_ALLTYPES;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped Example_alltypes structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_register_info_master_mac_t;
*    typedef PB_BYTES_ARRAY_T(6) apson_register_info_mac_t;
*    typedef PB_BYTES_ARRAY_T(16) apson_register_info_ap_ipv6_t;
*    typedef struct _apson_register_info {
*        char domain[32];
*        apson_register_info_master_mac_t master_mac;
*        char name[32];
*        char location[32];
*        char serial_num[15];
*        apson_register_info_mac_t mac;
*        char model_name[16];
*        uint32_t radio0_type;
*        uint32_t radio0_mimo_streams;
*        uint32_t radio1_type;
*        uint32_t radio1_mimo_streams;
*        uint32_t radio2_type;
*        uint32_t radio2_mimo_streams;
*        char main_image_ver[16];
*        char backup_image_ver[16];
*        uint32_t ap_ipv4;
*        apson_register_info_ap_ipv6_t ap_ipv6;
*        char ap_description[128];
*        uint32_t ap_uptime;
*        uint32_t register_timestamp;
*        uint32_t ipv4_mask;
*        uint32_t ipv4_gateway;
*        uint32_t last_reboot_reason;
*        uint32_t ap_sm_state;
*        uint32_t last_update_timestamp;
*        uint32_t campaigner_priority;
*        char latest_image_ver[16];
*        int32_t region_en;
*        char region_id[16];
*        char oui[8];
*        int32_t dl_progress;
*        int32_t dl_status;
*    } apson_register_info;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_register_info APSON_REGISTER_INFO;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped apson_config_wlan structure.
*
*    typedef struct _apson_config_wlan {
*        char device[32];
*        char ssid[32];
*        char key[64];
*        char encryption[64];
*        char checksum[36];
*    } apson_config_wlan;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_config_wlan APSON_CONFIG_WLAN;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped meshd_config structure.
*
*typedef struct _meshd_config {
*    char ssid[32];
*    char pwd[128];
*} meshd_config;
*
*
* NOTES :
*
*H************************************************************************/
typedef meshd_config MESHD_CONFIG;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped apson_config_msg structure.
*
*    typedef struct _apson_config_msg {
*        char checksum[36];
*
*    } apson_config_msg;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_config_msg APSON_CONFIG_MSG;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped meshd_mesh_ie_s structure.
*
*    typedef struct _meshd_mesh_ie_s {
*        uint32_t role;
*        uint32_t hops;
*        uint32_t family_cnt;
*        uint32_t ul_score;
*        uint32_t status;
*    } meshd_mesh_ie_s;
*
*
* NOTES :
*
*H************************************************************************/
typedef meshd_mesh_ie_s MSD_MESH_IE;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped meshd_timeval_s structure.
*
*    typedef struct _meshd_timeval_s {
*        int64_t tv_sec;
*        int64_t tv_usec;
*
*    } meshd_timeval_s;
*
*
* NOTES :
*
*H************************************************************************/
typedef meshd_timeval_s MSD_TIMEVAL;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped meshd_mesh_dev_s structure.
*
*    typedef struct _meshd_mesh_dev_s {
*        char bssid[6];
*        uint32_t channel;
*        uint32_t snr;
*        int32_t rssi;
*        meshd_mesh_ie_s mesh_ie;
*        meshd_timeval_s last_seen;
*    } meshd_mesh_dev_s;
*
* NOTES :
*
*H************************************************************************/
typedef meshd_mesh_dev_s MSD_MESH_DEV;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped meshd_ap_list_s structure.
*
*    typedef struct _meshd_ap_list_s {
*        pb_size_t ap_list_count;
*        meshd_mesh_dev_s ap_list[32];
*    } meshd_ap_list_s;
*
* NOTES :
*
*H************************************************************************/
typedef meshd_ap_list_s MSD_AP_LIST;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped meshd_self_s structure.
*
*    typedef struct _meshd_self_s {
*        meshd_mesh_dev_s self;
*    } meshd_self_s;
*
* NOTES :
*
*H************************************************************************/
typedef meshd_self_s MSD_SELF;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped profile_apson_s structure.
*
*    typedef struct _profile_apson_s {
*        int32_t discover_interval;
*        int32_t election_interval;
*        int32_t max_discoveries;
*        int32_t max_elections;
*        int32_t idle_time;
*        int32_t listen_master_time;
*        int32_t reg_time;
*        uint32_t stats_report_interval;
*        uint32_t stats_request_interval;
*        uint32_t echo_interval;
*        uint32_t echo_timeout;
*        uint32_t max_client;
*        uint32_t max_ap;
*        int32_t checksum;
*    } profile_apson_s;
*
* NOTES :
*
*H************************************************************************/
typedef profile_apson_s PF_APSON;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped profile_mcs_to_datarate_s structure.
*
*    typedef struct _profile_mcs_to_datarate_s {
*        pb_size_t datarate_count;
*        int32_t datarate[16];
*        int32_t checksum;
*    } profile_mcs_to_datarate_s;
*
* NOTES :
*
*H************************************************************************/
typedef profile_mcs_to_datarate_s PF_MCS_TO_DATARATE;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped profile_mesh_s structure.
*
*    typedef struct _profile_mesh_s {
*        char mesh_ssid[33];
*        char mesh_security_mode[16];
*        char mesh_encryption_type[16];
*        char mesh_psk[65];
*        uint32_t max_hops;
*        uint32_t max_children;
*        pb_size_t eth_list_count;
*        char eth_list[5][16];
*        pb_size_t eth_speed_count;
*        uint32_t eth_speed[5];
*        char br_name[16];
*        uint32_t mesh_module_antenna_num;
*        uint32_t mesh_module_scan_intval;
*        uint32_t mesh_module_nbr_aged_timeout;
*        uint32_t mesh_module_nbr_max_count;
*        char mesh_module_uplink_if_name[16];
*        char mesh_module_downlink_if_name[16];
*        int32_t arpd_looping_detecting_interval;
*        int32_t arpd_network_health_detecting_interval;
*        int32_t arpd_network_health_arploss_detecting_interval;
*        int32_t arpd_short_arp_size;
*        int32_t arpd_long_arp_size;
*        uint32_t min_local_speed;
*        uint32_t client_snr_threshold;
*        uint32_t mesh_snr_threshold;
*        int32_t checksum;
*    } profile_mesh_s;
*
*
* NOTES :
*
*H************************************************************************/
typedef profile_mesh_s PF_MESH;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped profile_snr_to_mcs_s structure.
*
*    typedef struct _profile_snr_to_mcs_s {
*        char name[16];
*        uint32_t min_snr;
*        uint32_t max_snr;
*        pb_size_t table_count;
*        int32_t table[50];
*    } profile_snr_to_mcs_s;
*
* NOTES :
*
*H************************************************************************/
typedef profile_snr_to_mcs_s PF_SNR_TO_MCS;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped profile_snr_to_mcs_s structure.
*
*    typedef struct _profile_snr_to_mcs_s {
*    pb_size_t snr2mcstbl_count;
*    profile_snr_to_mcs_s snr2mcstbl[32];
*    int32_t checksum;
*    } profile_snr_to_mcs_s;
*
* NOTES :
*
*H************************************************************************/
typedef profile_snr_to_mcs_tbl_s PF_SNR_TO_MCS_TBL;

int apson_pb_encode_profile_snr_mcs_tbl(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_profile_snr_mcs_tbl(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_profile_snr_mcs_tbl(void *strMsg, uint32_t *pBufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped profile_profile_s structure.
*
*    typedef struct _profile_profile_s {
*        profile_apson_s apson;
*        profile_mesh_s mesh;
*        profile_mcs_to_datarate_s mcs_to_datarate;
*        profile_snr_to_mcs_s snr_to_mcs;
*    } profile_profile_s;
*
* NOTES :
*
*H************************************************************************/
typedef profile_profile_s PF_PROFILE;

/************************************************************************
 Structure Name: APSON_CLIENT_ACTIVITY

 Usage: Data of client activity. For APSON Master/Member database. also 
        being used to member AP report to master AP.

 Editor: Jerry Kuo

    typedef PB_BYTES_ARRAY_T(6) apson_client_activity_mac_t;
    typedef PB_BYTES_ARRAY_T(6) apson_client_activity_ap_mac_t;
    typedef struct _apson_client_activity {
        apson_client_activity_mac_t mac;
        uint32_t                    snr;
        int32_t                     rssi;
        char                        ifname[16];
        uint32_t                    wlanmode;
        uint32_t                    tx_ss;
        uint32_t                    rx_ss;
        uint32_t                    bw;
        uint32_t                    reason;
        uint64_t                    to_client_tx_bytes;
        uint64_t                    to_client_rx_bytes;
        uint64_t                    to_client_tx_pkts;
        uint64_t                    to_client_rx_pkts;
        uint32_t                    state;
        apson_client_activity_ap_mac_t ap_mac;
    } apson_client_activity;

 Encode function:
     apson_pb_encode_client_activity()
 Decode function:
     apson_pb_decode_client_activity()
 Buffer size function:
     apson_pb_get_buf_size_client_activity()

 *************************************************************************/
typedef apson_client_activity APSON_CLIENT_ACTIVITY, *PAPSON_CLIENT_ACTIVITY;

int apson_pb_encode_client_activity(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_client_activity(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_size_client_activity(void *strMsg, uint32_t *pBufsize);

/************************************************************************
 Structure Name: APSON_MASH_DL_AP_ACTIVITY

 Usage: Data of mesh downlink ap activity. For APSON Master/Member database. also
        being used to member AP report to master AP.

 Editor: Alice

typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_dl_ap_activity_mac_t;
typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_dl_ap_activity_ap_mac_t;
typedef struct _apson_db_mesh_dl_ap_activity {
    apson_db_mesh_dl_ap_activity_mac_t mac;
    uint32_t snr;
    int32_t rssi;
    char ifname[16];
    uint32_t wlanmode;
    uint32_t tx_ss;
    uint32_t rx_ss;
    uint32_t bw;
    uint32_t reason;
    uint64_t to_client_tx_bytes;
    uint64_t to_client_rx_bytes;
    uint64_t to_client_tx_pkts;
    uint64_t to_client_rx_pkts;
    uint32_t state;
    apson_db_mesh_dl_ap_activity_ap_mac_t ap_mac;
} apson_db_mesh_dl_ap_activity;

 Encode function:
     apson_pb_encode_mesh_dl_ap_activity()
 Decode function:
     apson_pb_decode_mesh_dl_ap_activity()
 Buffer size function:
     apson_pb_get_buf_size_mesh_dl_ap_activity()

 *************************************************************************/
typedef apson_db_mesh_dl_ap_activity APSON_MESH_DL_AP_ACTIVITY, *PAPSON_MESH_DL_AP_ACTIVITY;

int apson_pb_encode_mesh_dl_ap_activity(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_mesh_dl_ap_activity(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_size_mesh_dl_ap_activity(void *strMsg, uint32_t *pBufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_member_ap_wlan_stats_ap_mac_t;
*    typedef struct _apson_db_member_ap_wlan_stats {
*        apson_db_member_ap_wlan_stats_ap_mac_t ap_mac;
*        uint32_t radio_id;
*        uint32_t ap_radio_type;
*        uint32_t frequency_band;
*        char interface_name[17];
*        char ssid[33];
*        uint64_t wlan_id;
*        uint32_t vap_mode;
*        pb_size_t wlan_stats_5bin_count;
*        apson_db_wlan_bin_stats wlan_stats_5bin[5];
*        uint32_t num_associated_station;
*        uint32_t last_update_timestamp;
*    } apson_db_member_ap_wlan_stats;
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_member_ap_wlan_stats AP_DB_MA_WLAN_STATS;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _apson_db_member_ap_wlan_stats_list {
*        uint32_t num_wlan;
*        pb_size_t radio_id_count;
*        apson_db_member_ap_wlan_stats wlan_stats_list[48];
*    } apson_db_member_ap_wlan_stats_list;
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_member_ap_wlan_stats_list AP_DB_MA_WLAN_STATS_LIST;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _apson_db_ap_mesh_neighbor_stats {
*        apson_db_ap_mesh_neighbor_stats_neighbor_mac_t neighbor_mac;
*        uint32_t neighbor_flag;
*        uint32_t ap_nbr_local_speed_mbps;
*        uint32_t nbr_root_path_speed_mbps;
*        uint32_t ap_nbr_root_path_speed_mbps;
*        uint32_t hop;
*        uint32_t mesh_role;
*        int32_t nbr_rssi;
*        uint32_t nbr_snr;
*        uint32_t nbr_channel;
*        uint32_t last_seen_timestamp;
*    } apson_db_ap_mesh_neighbor_stats;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_mesh_neighbor_stats AP_DB_AP_MESH_NB_STATS;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _apson_db_ap_radio_stats {
*        uint32_t radio_id;
*        uint32_t issupport;
*        uint32_t air_utlization_rx;
*        uint32_t air_utlization_busy;
*        uint32_t air_utlization_idle;
*        uint32_t air_utlization_rx_to_me;
*        uint32_t channel;
*        uint32_t channel_width;
*        uint32_t mimo_input_streams;
*        uint32_t mimo_output_streams;
*        uint32_t radio_type;
*        uint32_t frequency_band;
*        uint32_t num_associated_station;
*        uint32_t air_utlization_tx;
*    } apson_db_ap_radio_stats;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_radio_stats AP_DB_AP_RADIO_STATS;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_ap_system_stats_ap_mac_t;
*    typedef struct _apson_db_ap_system_stats {
*        char serial_number[16];
*        char ap_name[33];
*        char ap_location[65];
*        char ap_description[129];
*        apson_db_ap_system_stats_ap_mac_t ap_mac;
*        uint32_t ap_uptime;
*        uint32_t register_timestamp;
*        uint32_t ipv4;
*        char ipv6[16];
*        uint32_t ipv4_mask;
*        uint32_t ipv4_gateway;
*        char image_version[33];
*        char backup_version[33];
*        uint32_t last_reboot_reason;
*        uint32_t ap_sm_state;
*        uint32_t last_update_timestamp;
*    } apson_db_ap_system_stats;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_system_stats AP_DB_AP_SYS_STATS;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _apson_db_ap_wlan_activity_status {
*        char interface_name[17];
*        char ssid[33];
*        uint32_t radio_id;
*        uint32_t frequency_band;
*        uint32_t radio_type;
*    } apson_db_ap_wlan_activity_status;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_wlan_activity_status AP_DB_AP_WLAN_ACTIVITY_STATUS;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _apson_db_ap_wlan_stats_list {
*        uint32_t num_created_wlans;
*        pb_size_t wlan_stats_list_count;
*        apson_db_ap_wlan_stats wlan_stats_list[48];
*    } apson_db_ap_wlan_stats_list;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_wlan_stats_list AP_DB_AP_WLAN_STATS_LIST;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_module_cmd_set_monitor_client_anchor_ap_mac_t;
*    typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_module_cmd_set_monitor_client_neighbor_ap_mac_t;
*    typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_module_cmd_set_monitor_client_weak_client_mac_t;
*    typedef struct _apson_db_mesh_module_cmd_set_monitor_client {
*        apson_db_mesh_module_cmd_set_monitor_client_anchor_ap_mac_t anchor_ap_mac;
*        apson_db_mesh_module_cmd_set_monitor_client_neighbor_ap_mac_t neighbor_ap_mac;
*        apson_db_mesh_module_cmd_set_monitor_client_weak_client_mac_t weak_client_mac;
*        uint32_t associated_radio_id;
*        char interface_name[16];
*        uint32_t monitor_snr_db_threshold;
*        int32_t monitor_rssi_dbm_threshold;
*        uint32_t monitor_path_speed_mbps_threshold;
*        uint32_t monitor_time_sec;
*        uint32_t neighbor_ap_path_speed_mbps;
*        bool bRoaming;
*    } apson_db_mesh_module_cmd_set_monitor_client;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_mesh_module_cmd_set_monitor_client AP_DB_MESH_MODULE_CMD_SET_MONITOR_CLIENT;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_module_msg_client_associate_client_mac_t;
*    typedef struct _apson_db_mesh_module_msg_client_associate {
*        apson_db_mesh_module_msg_client_associate_client_mac_t client_mac;
*        uint32_t associated_radio_id;
*        char interface_name[17];
*        uint32_t client_radio_type;
*        uint32_t mimo_input_streams;
*        uint32_t mimo_output_streams;
*        int32_t client_rssi;
*        uint32_t client_snr;
*    } apson_db_mesh_module_msg_client_associate;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_mesh_module_msg_client_associate AP_DB_MESH_MODULEMSG_CLIENT_ASSOCIATE;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_module_msg_client_authorized_client_mac_t;
*    typedef struct _apson_db_mesh_module_msg_client_authorized {
*        apson_db_mesh_module_msg_client_authorized_client_mac_t client_mac;
*        uint32_t associated_radio_id;
*        char interface_name[17];
*        int32_t client_rssi;
*        uint32_t client_snr;
*    } apson_db_mesh_module_msg_client_authorized;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_mesh_module_msg_client_authorized AP_DB_MESH_MODULE_MSG_CLIENT_AUTHORIZED;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_module_msg_ip_assigned_client_mac_t;
*    typedef struct _apson_db_mesh_module_msg_ip_assigned {
*        apson_db_mesh_module_msg_ip_assigned_client_mac_t client_mac;
*        uint32_t associated_radio_id;
*        char interface_name[17];
*        uint32_t ipv4;
*        char ipv6[16];
*    } apson_db_mesh_module_msg_ip_assigned;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_mesh_module_msg_ip_assigned AP_DB_MESH_MODULE_MSG_IP_ASSIGNED;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_module_msg_monitor_client_info_update_ap_mac_t;
*    typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_module_msg_monitor_client_info_anchor_ap_mac_t;
*    typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_module_msg_monitor_client_info_weak_client_mac_t;
*    typedef struct _apson_db_mesh_module_msg_monitor_client_info {
*        apson_db_mesh_module_msg_monitor_client_info_update_ap_mac_t update_ap_mac;
*        apson_db_mesh_module_msg_monitor_client_info_anchor_ap_mac_t anchor_ap_mac;
*        apson_db_mesh_module_msg_monitor_client_info_weak_client_mac_t weak_client_mac;
*        uint32_t associated_radio_id;
*        char interface_name[16];
*        uint32_t received_snr_db;
*        int32_t received_rssi_dbm;
*        uint32_t weak_client_path_speed_mbps;
*        uint32_t monitor_time_sec;
*        uint32_t wlanmode;
*        uint32_t tx_ss;
*        uint32_t rx_ss;
*        uint32_t bw;
*    } apson_db_mesh_module_msg_monitor_client_info;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_mesh_module_msg_monitor_client_info AP_DB_MESH_MODULE_MSG_MONITOR_CLIENT_INFO;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_module_msg_uplink_authorized_uplink_bssid_t;
*    typedef struct _apson_db_mesh_module_msg_uplink_authorized {
*        apson_db_mesh_module_msg_uplink_authorized_uplink_bssid_t uplink_bssid;
*        uint32_t associated_radio_id;
*        char interface_name[17];
*        int32_t client_rssi;
*        uint32_t client_snr;
*    } apson_db_mesh_module_msg_uplink_authorized;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_mesh_module_msg_uplink_authorized AP_DB_MESH_MODULE_MSG_UL_AUTHORIZED;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_module_msg_uplink_connect_uplink_bssid_t;
*    typedef struct _apson_db_mesh_module_msg_uplink_connect {
*        apson_db_mesh_module_msg_uplink_connect_uplink_bssid_t uplink_bssid;
*        uint32_t associated_radio_id;
*        uint32_t uplink_radio_type;
*        char interface_name[17];
*        uint32_t mimo_input_streams;
*        uint32_t mimo_output_streams;
*        int32_t client_rssi;
*        uint32_t client_snr;
*    } apson_db_mesh_module_msg_uplink_connect;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_mesh_module_msg_uplink_connect AP_DB_MESH_MODULE_MSG_UPLINK_CONNECT;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_module_msg_uplink_disconnect_uplink_bssid_t;
*    typedef struct _apson_db_mesh_module_msg_uplink_disconnect {
*        apson_db_mesh_module_msg_uplink_disconnect_uplink_bssid_t uplink_bssid;
*        uint32_t associated_radio_id;
*        char interface_name[17];
*        uint32_t reason_code;
*        int32_t client_rssi;
*        uint32_t client_snr;
*    } apson_db_mesh_module_msg_uplink_disconnect;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_mesh_module_msg_uplink_disconnect AP_DB_MESH_MODULE_MSG_UL_DISCONNECT;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_module_msg_weak_client_info_weak_client_mac_t;
*    typedef struct _apson_db_mesh_module_msg_weak_client_info {
*        apson_db_mesh_module_msg_weak_client_info_weak_client_mac_t weak_client_mac;
*        uint32_t associated_radio_id;
*        char interface_name[16];
*        uint32_t received_snr;
*        int32_t received_rssi;
*    } apson_db_mesh_module_msg_weak_client_info;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_mesh_module_msg_weak_client_info AP_DB_MESH_MODULE_MSG_WEAK_CLIENT_INFO;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
    typedef PB_BYTES_ARRAY_T(6) apson_db_module_cmd_set_client_criteria_anchor_ap_mac_t;
    typedef PB_BYTES_ARRAY_T(6) apson_db_module_cmd_set_client_criteria_neighbor_ap_mac_t;
    typedef PB_BYTES_ARRAY_T(6) apson_db_module_cmd_set_client_criteria_weak_client_mac_t;
    typedef struct _apson_db_module_cmd_set_client_criteria {
        apson_db_module_cmd_set_client_criteria_anchor_ap_mac_t anchor_ap_mac;
        apson_db_module_cmd_set_client_criteria_neighbor_ap_mac_t neighbor_ap_mac;
        apson_db_module_cmd_set_client_criteria_weak_client_mac_t weak_client_mac;
        uint32_t associated_radio_id;
        char interface_name[16];
        uint32_t accept_snr_db_threshold;
        int32_t accept_rssi_dbm_threshold;
        uint32_t accept_client_path_speed_mbps_threshold;
        uint32_t lock_time_sec;
    } apson_db_module_cmd_set_client_criteria;

*
* NOTES :
*
*H************************************************************************/
typedef apson_db_module_cmd_set_client_criteria AP_DB_MODULE_CMD_SET_CLIENT_CRITERIA;



/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _apson_db_station_bin_stats {
*        uint64_t ap_sta_tx_packts;
*        uint64_t ap_sta_tx_bytes;
*        uint64_t sta_ap_rx_packts;
*        uint64_t sta_ap_rx_bytes;
*        int32_t client_rssi;
*        uint32_t client_snr;
*        uint32_t start_tm;
*        uint32_t tm_len;
*    } apson_db_station_bin_stats;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_station_bin_stats AP_DB_STA_BIN_STATS;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _apson_db_wlan_bin_stats {
*        uint64_t ap_sta_tx_packts;
*        uint64_t ap_sta_tx_bytes;
*        uint64_t sta_ap_rx_packts;
*        uint64_t sta_ap_rx_bytes;
*        uint32_t start_tm;
*        uint32_t tm_len;
*    } apson_db_wlan_bin_stats;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_wlan_bin_stats AP_DB_WLAN_BIN_STATS;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_ap_mesh_downlink_ap_stats_downlink_client_mac_t;
*    typedef struct _apson_db_ap_mesh_downlink_ap_stats {
*        apson_db_ap_mesh_downlink_ap_stats_downlink_client_mac_t downlink_client_mac;
*        uint32_t downlink_client_radio_type;
*        char interface_name[17];
*        char ssid[33];
*        pb_size_t downlink_client_stats_5bin_count;
*        apson_db_station_bin_stats downlink_client_stats_5bin[5];
*        uint32_t last_seen_timestamp;
*        uint32_t channel;
*        uint32_t associated_timestamp;
*        uint32_t authorized_timestamp;
*        uint32_t downlink_ap_hop;
*        uint32_t ap_dlink_local_speed_mbps;
*        uint32_t dlink_ap_root_path_speed_mbps;
*        uint32_t mimo_input_streams;
*        uint32_t mimo_output_streams;
*        int32_t downlink_ap_rssi;
*        uint32_t downlink_ap_snr;
*    } apson_db_ap_mesh_downlink_ap_stats;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_mesh_downlink_ap_stats APSON_DB_AP_MESH_DL_AP_STATS;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_ap_mesh_downlink_wlan_ap_mac_t;
*    typedef struct _apson_db_ap_mesh_downlink_wlan {
*        apson_db_ap_mesh_downlink_wlan_ap_mac_t ap_mac;
*        uint32_t radio_id;
*        uint32_t ap_radio_type;
*        uint32_t frequency_band;
*        char interface_name[17];
*        char ssid[33];
*        uint32_t wlan_id;
*        pb_size_t wlan_stats_5bins_count;
*        apson_db_wlan_bin_stats wlan_stats_5bins[5];
*        uint32_t num_associated_station;
*        uint32_t last_update_timestamp;
*    } apson_db_ap_mesh_downlink_wlan;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_mesh_downlink_wlan AP_DB_AP_MESH_DOWNLINK_WLAN;



/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
    typedef struct _apson_db_ap_mesh_neighbor_stats_list {
        uint32_t num_neighbors;
        pb_size_t neighbor_stats_list_count;
        apson_db_ap_mesh_neighbor_stats neighbor_stats_list[16];
    } apson_db_ap_mesh_neighbor_stats_list;

*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_mesh_neighbor_stats_list AP_DB_AP_MESH_NB_STATS_LIST;



/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_ap_mesh_stats_uplink_ap_mac_t;
*    typedef struct _apson_db_ap_mesh_stats {
*        uint32_t mesh_role;
*        uint32_t hop;
*        uint32_t ap_uplink_local_speed_mbps;
*        uint32_t ap_root_path_speed_mbps;
*        uint32_t uplink_associated_timestamp;
*        apson_db_ap_mesh_stats_uplink_ap_mac_t uplink_ap_mac;
*        int32_t rssi;
*        uint32_t snr;
*        uint32_t channel;
*        uint32_t radio_id;
*        uint32_t radio_type;
*        char interface_name[17];
*        uint32_t mesh_review_mesh;
*        apson_db_ap_mesh_neighbor_stats best_candidate_uplink_ap;
*        uint32_t num_downlink_ap;
*        uint32_t frequency_band;
*    } apson_db_ap_mesh_stats;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_mesh_stats AP_DB_AP_MESH_STATS;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _apson_db_ap_radio_stats_list {
*        uint32_t num_radio;
*        pb_size_t radio_stats_list_count;
*        apson_db_ap_radio_stats radio_stats_list[3];
*    } apson_db_ap_radio_stats_list;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_radio_stats_list AP_DB_AP_RADIO_STATS_LIST;



/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_ap_station_stats_client_mac_t;
*    typedef struct _apson_db_ap_station_stats {
*        apson_db_ap_station_stats_client_mac_t client_mac;
*        uint32_t associated_radio_id;
*        uint32_t ap_radio_type;
*        uint32_t frequency_band;
*        char interface_name[17];
*        char ssid[33];
*        apson_db_station_bin_stats station_snapshot_stats;
*        uint32_t client_radio_type;
*        uint32_t channel;
*        uint32_t asso_wlan_id;
*    } apson_db_ap_station_stats;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_station_stats AP_DB_AP_STA_STATS;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*        typedef struct _apson_db_ap_wlan_list_status {
*            uint32_t num_created_wlans;
*            pb_size_t wlan_status_list_count;
*            apson_db_ap_wlan_activity_status wlan_status_list[48];
*        } apson_db_ap_wlan_list_status;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_wlan_list_status AP_DB_AP_WLAN_LIST_STATUS;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_ap_wlan_stats_ap_mac_t;
*    typedef struct _apson_db_ap_wlan_stats {
*        apson_db_ap_wlan_stats_ap_mac_t ap_mac;
*        uint32_t radio_id;
*        uint32_t ap_radio_type;
*        uint32_t frequency_band;
*        char interface_name[17];
*        char ssid[33];
*        uint64_t wlan_id;
*        uint32_t vap_mode;
*        apson_db_wlan_bin_stats wlan_snapshot_stats;
*    } apson_db_ap_wlan_stats;
*
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_wlan_stats AP_DB_AP_WLAN_STATS;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_member_ap_station_stats_client_mac_t;
*    typedef PB_BYTES_ARRAY_T(6) apson_db_member_ap_station_stats_associated_ap_mac_t;
*    typedef struct _apson_db_member_ap_station_stats {
*        apson_db_member_ap_station_stats_client_mac_t client_mac;
*        uint32_t associated_radio_id;
*        uint32_t ap_radio_type;
*        uint32_t frequency_band;
*        char interface_name[17];
*        char ssid[33];
*        uint32_t client_radio_type;
*        uint32_t channel;
*        pb_size_t station_stats_5bin_count;
*        apson_db_station_bin_stats station_stats_5bin[5];
*        char host_name[33];
*        uint32_t associated_timestamp;
*        uint32_t authorized_timestamp;
*        uint32_t ip_assigned_timestamp;
*        uint32_t station_ipv4;
*        char station_ipv6[16];
*        uint32_t is_weak_client;
*        uint32_t station_hop;
*        uint32_t local_speed_mbps;
*        uint32_t path_speed_mbps;
*        uint32_t mimo_input_streams;
*        uint32_t mimo_output_streams;
*        int32_t client_rssi;
*        uint32_t client_snr;
*        uint32_t last_update_timestamp;
*        apson_db_member_ap_station_stats_associated_ap_mac_t associated_ap_mac;
*        uint32_t asso_wlan_id;
*    } apson_db_member_ap_station_stats;
*
*
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_member_ap_station_stats AP_DB_MA_STA_STATS;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_mesh_module_msg_client_disconnect_client_mac_t;
*    typedef struct _apson_db_mesh_module_msg_client_disconnect {
*        apson_db_mesh_module_msg_client_disconnect_client_mac_t client_mac;
*        uint32_t associated_radio_id;
*        char interface_name[17];
*        uint32_t reason_code;
*        apson_db_station_bin_stats snapshot_stats;
*    } apson_db_mesh_module_msg_client_disconnect;
*
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_mesh_module_msg_client_disconnect  AP_DB_MESH_MODULE_MSG_CLIENT_DISCONNECT;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _apson_db_ap_mesh_downlink_ap_stats_list {
*        uint32_t num_downlink_ap;
*        pb_size_t downlink_ap_stats_list_count;
*        apson_db_ap_mesh_downlink_ap_stats downlink_ap_stats_list[10];
*    } apson_db_ap_mesh_downlink_ap_stats_list;
*
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_mesh_downlink_ap_stats_list AP_DB_MESH_DL_AP_STATS_LIST;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _apson_db_ap_station_stats_list {
*        uint32_t num_station;
*        pb_size_t station_stats_list_count;
*        apson_db_ap_station_stats station_stats_list[40];
*    } apson_db_ap_station_stats_list;
*
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_station_stats_list AP_DB_AP_STA_STATS_LIST;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _apson_db_member_ap_station_stats_list {
*        uint32_t num_station;
*        pb_size_t station_stats_list_count;--
*        apson_db_member_ap_station_stats station_stats_list[40];
*    } apson_db_member_ap_station_stats_list;
*
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_member_ap_station_stats_list AP_DB_MA_STA_STATS_LIST;


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _apson_db_apson_mesh_info {
*        apson_db_ap_mesh_stats mesh_stats;
*        apson_db_ap_mesh_neighbor_stats_list neighbor_stats;
*        apson_db_ap_mesh_downlink_ap_stats_list mesh_dl_ap_stats;
*        apson_db_ap_mesh_downlink_wlan mesh_dl_wlan;
*    } apson_db_apson_mesh_info;
*
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_apson_mesh_info AP_DB_APSON_MESH_INFO;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
* typedef struct _ap_stats_ap_stats {
*    uint32_t ap_mac;
*    uint32_t level;
*    bool next;
*} ap_stats_ap_stats;
*
*
* NOTES :
*
*H************************************************************************/
typedef ap_stats_ap_stats AP_STATS_AP_STATS;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
* typedef struct _ap_stats_kickout_client {
*     uint32_t client_mac;
*     uint32_t ap_mac;
*     char interface_name[17];
*     uint32_t reason_code;
*     uint32_t lock_tim;
* } apson_stats_kickout_client;
*
*
* NOTES :
*
*H************************************************************************/
typedef ap_stats_kickout_client AP_STATS_KICKOUT_CLIENT;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
* typedef struct _apson_stats_last_released_image_info {
*     uint32_t ap_mac;
*     char ap_model_name[17];
*     char last_released_version[13];
* } apson_stats_last_released_image_info;
*
*
* NOTES :
*
*H************************************************************************/
typedef ap_stats_last_released_image_info AP_STATS_LAST_REL_IMG_INFO;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
* typedef struct _apson_stats_mesh_stats {
*     uint32_t ap_mac;
*     uint32_t level;
* } apson_stats_mesh_stats;
*
*
* NOTES :
*
*H************************************************************************/
typedef ap_stats_mesh_stats AP_STATS_MESH_STATS;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
* typedef struct _ap_stats_reboot_ap {
*     uint32_t ap_mac;
*     uint32_t reason_code;
*     uint32_t reboot_sec_later;
* } ap_stats_reboot_ap;
*
*
* NOTES :
*
*H************************************************************************/
typedef ap_stats_reboot_ap AP_STATS_REBOOT_AP;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
* typedef struct _ap_stats_specific_target_ap_image_info {
*     uint32_t check_type;
*     uint32_t ap_mac;
*     char ap_model_name[13];
* } ap_stats_specific_target_ap_image_info;
*
*
* NOTES :
*
*H************************************************************************/
typedef ap_stats_specific_target_ap_image_info AP_STATS_SPEC_AP_IMG_INFO;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
* typedef PB_BYTES_ARRAY_T(6) ap_stats_sta_stats_ap_mac_t;
* typedef PB_BYTES_ARRAY_T(6) ap_stats_sta_stats_sta_mac_t;
* typedef struct _ap_stats_sta_stats {
*     ap_stats_sta_stats_ap_mac_t ap_mac;
*     uint64_t wlan_id;
*     ap_stats_sta_stats_sta_mac_t sta_mac;
*     uint32_t level;
* } ap_stats_sta_stats;
*
*
* NOTES :
*
*H************************************************************************/
typedef ap_stats_sta_stats AP_STATS_STA_STATS;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
* typedef struct _ap_stats_wlan_stats {
*     uint32_t ap_mac;
*     uint64_t wlan_id;
*     uint32_t level;
* } ap_stats_wlan_stats;
*
*
* NOTES :
*
*H************************************************************************/
typedef ap_stats_wlan_stats AP_STATS_WLAN_STATS;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
* typedef struct _ap_stats_ap_image_upgrade {
*     uint32_t num_ap;
*     apson_stats_last_released_image_info ap_image_info;
* } ap_stats_ap_image_upgrade;
*
*
* NOTES :
*
*H************************************************************************/
typedef ap_stats_ap_image_upgrade AP_STATS_AP_IMG_UPGRADE;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
* typedef struct _ap_stats_system_width_stats {
*     apson_stats_ap_stats ap;
*     apson_stats_wlan_stats wlan;
*     apson_stats_sta_stats sta;
*     apson_stats_mesh_stats mesh;
* } ap_stats_system_width_stats;
*
*
* NOTES :
*
*H************************************************************************/
typedef ap_stats_system_width_stats AP_STATS_SYS_WIDTH_STATS;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) apson_db_ap_list_mac_t;
*    typedef struct _apson_db_ap_list {
*        apson_db_ap_list_mac_t mac;
*        apson_register_info ap_db;
*        apson_db_apson_mesh_info mesh_db;
*        apson_db_ap_radio_stats_list radio_db;
*        apson_db_member_ap_wlan_stats_list wlan_db;
*        apson_db_member_ap_station_stats_list sta_db;
*    } apson_db_ap_list;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_ap_list AP_DB_AP_LIST;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _apson_s {
*        int32_t checksum;
*        char interface[16];
*        char domain[32];
*        char model[32];
*        bool prefermaster;
*        bool provision;
*        bool election;
*        bool router;
*    } apson_s;
*
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_s CFG_APSON;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _mesh_s {
*        int32_t checksum;
*        char ssid[33];
*        uint32_t security_mode;
*        uint32_t encryption_mode;
*        char psk[65];
*        pb_size_t eth_list_count;
*        char eth_list[5][16];
*        pb_size_t eth_speed_count;
*        uint32_t eth_speed[5];
*        char br_name[16];
*        uint32_t antenna_num;
*        char uplink_if_name[16];
*        char downlink_if_name[16];
*    } mesh_s;
*
*
*
* NOTES :
*
*H************************************************************************/
typedef mesh_s CFG_MESH;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _profile_s {
*        int32_t checksum;
*        profile_apson_s apson;
*        profile_mesh_s mesh;
*        profile_mcs_to_datarate_s mcs_to_datarate;
*        profile_snr_to_mcs_tbl_s snr_to_mcs;
*    } profile_s;
*
*
*
* NOTES :
*
*H************************************************************************/
typedef profile_s CFG_PROFILE;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _radio_s {
*        int32_t checksum;
*        int32_t radio_sum;
*        pb_size_t radio_sub_count;
*        radio_info_s radio_sub[16];
*        } radio_s;
*
*
*
* NOTES :
*
*H************************************************************************/
typedef radio_s CFG_RADIO;

typedef roam_info_s CFG_ROAM_INFO;
typedef roam_s CFG_ROAM;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*   typedef struct _wlan_info_s {
*        int32_t checksum;
*        char device[9];
*        char ifname[9];
*        char ssid[33];
*        char encryption[10];
*        char key[64];
*        char sche_name[16];
*        int32_t secu_enable;
*        int32_t enable;
*        int32_t isolate;
*        char radioid[20];
*        int32_t ssid_bcast;
*        int32_t qos;
*        char type[15];
*        int32_t key_renewal;
*        char radius_ip1[16];
*        int32_t radius_port1;
*        char radius_secret1[150];
*        char radius_ip2[16];
*        int32_t radius_port2;
*        char radius_secret2[150];
*    } wlan_info_s;
*
*
*
* NOTES :
*
*H************************************************************************/
typedef wlan_info_s CFG_WLAN_INFO;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _wlan_s {
*        int32_t checksum;
*        int32_t wlan_sum;
*        pb_size_t wlan_sub_count;
*        wlan_info_s wlan_sub[4];
*    } wlan_s;
*
*
*
* NOTES :
*
*H************************************************************************/
typedef wlan_s CFG_WLAN;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _config {
*        int32_t checksum;
*        char cfg_path[64];
*        apson_s apson;
*        wlan_s wlan;
*        radio_s radio;
*        mesh_s mesh;
*        profile_s profile;
*        schedule_setting_s sche_set;
*        mac_filter_s mac_filt;
*        wps_s wps;
*        mac_del_filter mac_del_flt;
*        time_setting_s tm_set;
*        dev_setting_s dev_set;
*        int32_t isEncode;
*        roam_s roam;
*    } config;
*
*
* NOTES :
*
*H************************************************************************/
typedef config CFG_INFO;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _dev_setting_s {
*        int32_t checksum;
*        char dev_name[21];
*        char admin_pwd[16];
*        char presentation_url[257];
*        int32_t captcha;
*        int32_t change_pwd;
*        char tz_location[32];
*    } dev_setting_s;
*
*
* NOTES :
*
*H************************************************************************/
typedef dev_setting_s CFG_DEV_SET;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*    typedef struct _mac_del_filter {
*        int32_t checksum;
*        char mac[18];
*    } mac_del_filter;
*
*
* NOTES :
*
*H************************************************************************/
typedef mac_del_filter CFG_MAC_DEL_FLT;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _mac_info_s {
*        int32_t checksum;
*        char status[6];
*        char mac[18];
*        char dev_name[21];
*        char sche_name[16];
*    } mac_info_s;
*
*
* NOTES :
*
*H************************************************************************/
typedef mac_info_s CFG_MAC_INFO;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*
*    typedef struct _time_info_s {
*        int32_t checksum;
*        int32_t hour;
*        int32_t minute;
*        int32_t middate;
*    } time_info_s;
*
*
* NOTES :
*
*H************************************************************************/
typedef time_info_s CFG_TM_INFO;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _time_setting_s {
*        int32_t checksum;
*        char cur_date[11];
*        char cur_time[9];
*        int32_t ntp;
*        char ntp_server[257];
*        char tz[3];
*        int32_t daylight_saving;
*        char dst_start_month[3];
*        char dst_start_week[2];
*        char dst_start_day_of_week[2];
*        char dst_start_time[9];
*        char dst_end_month[3];
*        char dst_end_week[2];
*        char dst_end_day_of_week[2];
*        char dst_end_time[9];
*    } time_setting_s;
*
*
* NOTES :
*
*H************************************************************************/
typedef time_setting_s CFG_TM_SET;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _wps_s {
*        int32_t checksum;
*        int32_t wpspbc;
*        int32_t wpspin;
*    } wps_s;
*
*
* NOTES :
*
*H************************************************************************/
typedef wps_s CFG_WPS;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _schedule_info_s {
*        int32_t checksum;
*        int32_t date;
*        int32_t allday;
*        int32_t time_format;
*        time_info_s start_time;
*        time_info_s end_time;
*    } schedule_info_s;
*
*
* NOTES :
*
*H************************************************************************/
typedef schedule_info_s CFG_SCHE_INFO;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _mac_filter_s {
*        int32_t checksum;
*        char enable[6];
*        char is_allow_list[6];
*        pb_size_t mac_info_count;
*        mac_info_s mac_info[24];
*    } mac_filter_s;
*
*
* NOTES :
*
*H************************************************************************/
typedef mac_filter_s CFG_MAC_FLT;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _schedule_list_s {
*        int32_t checksum;
*        char name[16];
*        pb_size_t sche_info_count;
*        schedule_info_s sche_info[84];
*    } schedule_list_s;
*
*
* NOTES :
*
*H************************************************************************/
typedef schedule_list_s CFG_SCHE_LST;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _schedule_setting_s {
*        int32_t checksum;
*        pb_size_t sche_list_count;
*        schedule_list_s sche_list[10];
*    } schedule_setting_s;
*
*
* NOTES :
*
*H************************************************************************/
typedef schedule_setting_s CFG_SCHE_SET;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _radio_info_s {
*        int32_t checksum;
*        char radioid[20];
*        int32_t enable;
*        char mode[15];
*        int32_t ch_width;
*        int32_t ch;
*        int32_t second_ch;
*        int32_t tx_power;
*        int32_t coexistence;
*        int32_t mumimo_enable;
*        int32_t band_steer_enable;
*        int32_t air_tm_fairness_en;
*    } radio_info_s;
*
*
* NOTES :
*
*H************************************************************************/
typedef radio_info_s CFG_RADIO_INFO;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) ap_stats_bin_stats_ap_mac_t;
*    typedef struct _ap_stats_bin_stats {
*        ap_stats_bin_stats_ap_mac_t ap_mac;
*        uint32_t level;
*        bool next;
*    } ap_stats_bin_stats;
*
*
* NOTES :
*
*H************************************************************************/
typedef ap_stats_bin_stats AP_STATS_BIN_STATS;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _ap_stats_bin_data {
*        apson_db_member_ap_wlan_stats_list wlan_bin;
*        apson_db_member_ap_station_stats_list sta_bin;
*        apson_db_wlan_bin_stats mesh_dl_bin;
*        apson_db_wlan_bin_stats mesh_ul_bin;
*    } ap_stats_bin_data;
*
*
* NOTES :
*
*H************************************************************************/
typedef ap_stats_bin_data AP_STATS_BIN_DATA;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) auto_asgmt_site_survey_info_ap_mac_t;
*    typedef struct _auto_asgmt_site_survey_info {
*        auto_asgmt_site_survey_info_ap_mac_t ap_mac;
*        uint32_t radio;
*        uint32_t err_reason;
*        uint32_t ch_cnt;
*        pb_size_t ch_list_count;
*        uint32_t ch_list[64];
*    } auto_asgmt_site_survey_info;
*
*
* NOTES :
*
*H************************************************************************/
typedef auto_asgmt_site_survey_info AUTO_ASGMT_SITE_SURVEY_INFO;

/*H**********************************************************************
* FILENAME :    apson_pb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*   typedef PB_BYTES_ARRAY_T(6) apson_db_client_beacon_measure_mac_t;
*   typedef PB_BYTES_ARRAY_T(6) apson_db_client_beacon_measure_curr_bssid_t;
*   typedef struct _apson_db_client_beacon_measure {
*       apson_db_client_beacon_measure_mac_t mac;
*       apson_db_client_beacon_measure_curr_bssid_t curr_bssid;
*       char ssid[33];
*       uint32_t cli_cap;
*       char ifname[16];
*       uint32_t archor_path_speed;
*       pb_size_t bcn_rpt_count;
*       apson_db_beacon_report bcn_rpt[48];
*   } apson_db_client_beacon_measure;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_client_beacon_measure AP_DB_CLIENT_BEACON_MEASURE;

/*H**********************************************************************
* FILENAME :    apson_pb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*   typedef PB_BYTES_ARRAY_T(6) apson_db_beacon_report_bssid_t;
*   typedef struct _apson_db_beacon_report {
*       apson_db_beacon_report_bssid_t bssid;
*       uint32_t ch;
*       uint32_t rcpi;
*   } apson_db_beacon_report;
*
*
* NOTES :
*
*H************************************************************************/
typedef apson_db_beacon_report AP_DB_BEACON_REPORT;

#ifdef _SUPER_MESH_PRO_
/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_band_info {
*        int32_t link_integrity;
*        int32_t eth_to_wlan;
*        pb_size_t ssid_count;
*        int32_t ssid[8];
*    } sp_config_band_info;
*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_band_info SP_CONFIG_BAND_INFO;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) sp_config_client_acl_list_mac_t;
*    typedef struct _sp_config_client_acl_list {
*        int32_t wlan_id;
*        int32_t radio_id;
*        sp_config_client_acl_list_mac_t mac;
*    } sp_config_client_acl_list;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_client_acl_list SP_CONFIG_CLIENT_ACL_LIST;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_cp_setting {
*        int32_t checksum;
*        char wlan_name[20];
*        int32_t wlan_en;
*        int32_t enable_captive;
*        int32_t webpage_id;
*        int32_t timeout;
*    } sp_config_cp_setting;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_cp_setting SP_CONFIG_CP_SETTING;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_device_mgmt {
*        int32_t checksum;
*        int32_t sub_cfg_id;
*        char user_name[64];
*        char pwd[12];
*        int32_t telnet;
*        int32_t ssh;
*        int32_t timeout;
*        int32_t sntp_status;
*        char sntp_server_ip[128];
*        int32_t sntp_tm_zone_idx;
*        int32_t sntp_daylight_saving;
*        int32_t country_code;
*        char sys_log_server_ip_eu[128];
*    } sp_config_device_mgmt;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_device_mgmt SP_CONFIG_DEVICE_MGMT;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) sp_config_gateway_list_mac_add_t;
*    typedef struct _sp_config_gateway_list {
*        int32_t idx;
*        int32_t ip_add;
*        sp_config_gateway_list_mac_add_t mac_add;
*    } sp_config_gateway_list;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_gateway_list SP_CONFIG_GATEWAY_LIST;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_ip_filter {
*        int32_t checksum;
*        char wlan_name[20];
*        int32_t wlan_en;
*        int32_t ip;
*        int32_t subnet;
*    } sp_config_ip_filter;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_ip_filter SP_CONFIG_IP_FILTER;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) sp_config_mac_bypass_mac_t;
*    typedef struct _sp_config_mac_bypass {
*        int32_t checksum;
*        char wlan_name[20];
*        int32_t wlan_en;
*        sp_config_mac_bypass_mac_t mac;
*    } sp_config_mac_bypass;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_mac_bypass SP_CONFIG_MAC_BYPASS;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef PB_BYTES_ARRAY_T(6) sp_config_mesh_prefer_uplinkap_t;
*    typedef PB_BYTES_ARRAY_T(6) sp_config_mesh_prefer_meshap_t;
*    typedef struct _sp_config_mesh {
*        int32_t checksum;
*        char domain[32];
*        char mesh_ssid[33];
*        int32_t mesh_security_mode;
*        int32_t mesh_encryption_type;
*        char mesh_psk[64];
*        int32_t ipv6;
*    } sp_config_mesh;
*
*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_mesh SP_CONFIG_MESH;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_pvid_set {
*        int32_t auto_asgnt_status;
*        int32_t mgmt;
*        int32_t lan1;
*        int32_t lan2;
*        pb_size_t ssidradio0_count;
*        int32_t ssidradio0[8];
*        pb_size_t ssidradio1_count;
*        int32_t ssidradio1[8];
*        pb_size_t ssidradio2_count;
*        int32_t ssidradio2[8];
*        pb_size_t ssidradio3_count;
*        int32_t ssidradio3[8];
*    } sp_config_pvid_set;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_pvid_set SP_CONFIG_PVID_SET;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_qos_bsc_set {
*        int32_t sub_cfg_id;
*        int32_t status;
*        int32_t downlink_bw;
*        int32_t uplink_bw;
*    } sp_config_qos_bsc_set;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_qos_bsc_set SP_CONFIG_QOS_BSC_SET;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_qos_rule {
*        int32_t band;
*        int32_t rule_type;
*        int32_t ssid_idx;
*        int32_t down_speed;
*        int32_t down_speed_type;
*        int32_t up_speed;
*        int32_t up_speed_type;
*    } sp_config_qos_rule;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_qos_rule SP_CONFIG_QOS_RULE;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_radio_basic_set {
*        int32_t checksum;
*        char band_id[10];
*        int32_t enable;
*        int32_t tx_power;
*        int32_t beacon_interval;
*        int32_t ack_Timeout;
*        int32_t short_gi;
*        int32_t igmp;
*        int32_t multi_rate_en;
*        int32_t multi_rate;
*        int32_t multi_bw_en;
*        int32_t multi_max_bw;
*        int32_t ht_coexit;
*        int32_t dhcp_transfer;
*    } sp_config_radio_basic_set;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_radio_basic_set SP_CONFIG_RADIO_BASIC_SET;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_radio_rsrc {
*        int32_t band_steer_en;
*        int32_t air_tm_fairness_en;
*        int32_t aging_out;
*        int32_t acl_rssi;
*        int32_t conn_limit;
*    } sp_config_radio_rsrc;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_radio_rsrc SP_CONFIG_RADIO_RSRC;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_radio_setting {
*        char wlan_name[20];
*        int32_t wlan_en;
*        int32_t wlan_num;
*        int32_t mode;
*    } sp_config_radio_setting;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_radio_setting SP_CONFIG_RADIO_SETTING;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_tm_rule_lst {
*        char rule_name[32];
*        int32_t ssid_idx;
*        char ssid[33];
*        int32_t day_select;
*        int32_t all_day_select;
*        pb_size_t start_time_count;
*        int32_t start_time[2];
*        pb_size_t end_time_count;
*        int32_t end_time[2];
*        int32_t overnight;
*    } sp_config_tm_rule_lst;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_tm_rule_lst SP_CONFIG_TM_RULE_LST;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_vlan_lst {
*        int32_t index;
*        int32_t vid;
*        char name[33];
*        int32_t mgmt;
*        int32_t lan1;
*        int32_t lan2;
*        pb_size_t ssidradio0_count;
*        int32_t ssidradio0[8];
*        pb_size_t ssidradio1_count;
*        int32_t ssidradio1[8];
*        pb_size_t ssidradio2_count;
*        int32_t ssidradio2[8];
*        pb_size_t ssidradio3_count;
*        int32_t ssidradio3[8];
*    } sp_config_vlan_lst;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_vlan_lst SP_CONFIG_VLAN_LST;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_wlan_advs_set {
*        int32_t wmm;
*        int32_t priority;
*        char mode[15];
*        int32_t data_rate;
*        int32_t band_width;
*        int32_t isolate;
*    } sp_config_wlan_advs_set;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_wlan_advs_set SP_CONFIG_WLAN_ADVS_SET;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_wlan_bsc_set {
*        char wlan_name[20];
*        uint32_t wlan_en;
*        char radio_name[20];
*        char ssid[33];
*        int32_t ssid_bcast;
*    } sp_config_wlan_bsc_set;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_wlan_bsc_set SP_CONFIG_WLAN_BSC_SET;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_wlan_scy_set {
*        char id[20];
*        char name[20];
*        char type[15];
*        char encryption[10];
*        char key[64];
*        int32_t key_renewal;
*        int32_t key_change;
*        int32_t key_act_time;
*        char radius_server1[16];
*        int32_t radius_port1;
*        char radius_secret1[150];
*        char radius_server2[16];
*        int32_t radius_port2;
*        char radius_secret2[150];
*        int32_t accounting_enable;
*        char accounting_server[16];
*        int32_t accounting_port;
*        char accounting_secret[150];
*        char accounting2_server[16];
*        int32_t accounting2_port;
*        char accounting2_secret[150];
*    } sp_config_wlan_scy_set;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_wlan_scy_set SP_CONFIG_WLAN_SCY_SET;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_acl {
*        int32_t checksum;
*        sp_config_radio_setting radio_set;
*        pb_size_t clt_acl_lst_count;
*        sp_config_client_acl_list clt_acl_lst[32];
*    } sp_config_acl;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_acl SP_CONFIG_ACL;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_arp_filter {
*        int32_t checksum;
*        int32_t arp_spf_prev;
*        pb_size_t gateway_lst_count;
*        sp_config_gateway_list gateway_lst[32];
*    } sp_config_arp_filter;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_arp_filter SP_CONFIG_ARP_FILTER;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_captive_portal {
*        int32_t checksum;
*        pb_size_t cp_set_count;
*        sp_config_cp_setting cp_set[24];
*        pb_size_t ip_flt_count;
*        sp_config_ip_filter ip_flt[32];
*        pb_size_t mac_bps_count;
*        sp_config_mac_bypass mac_bps[32];
*    } sp_config_captive_portal;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_captive_portal SP_CONFIG_CAPTIVE_PORTAL;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_qos {
*        int32_t checksum;
*        sp_config_qos_bsc_set bsc_set;
*        pb_size_t rule_set_count;
*        sp_config_qos_rule rule_set[32];
*    } sp_config_qos;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_qos SP_CONFIG_QOS;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_radio {
*        int32_t checksum;
*        pb_size_t bsc_set_count;
*        sp_config_radio_basic_set bsc_set[4];
*        sp_config_radio_rsrc rsrc;
*    } sp_config_radio;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_radio SP_CONFIG_RADIO;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_tm_sche {
*        int32_t checksum;
*        int32_t sub_cfg_id;
*        int32_t status;
*        pb_size_t rule_lst_count;
*        sp_config_tm_rule_lst rule_lst[16];
*    } sp_config_tm_sche;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_tm_sche SP_CONFIG_TM_SCHE;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_vlan {
*        int32_t checksum;
*        int32_t status;
*        pb_size_t vlan_lst_count;
*        sp_config_vlan_lst vlan_lst[34];
*        sp_config_pvid_set pvid_set;
*    } sp_config_vlan;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_vlan SP_CONFIG_VLAN;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_wlan_info {
*        int32_t checksum;
*        sp_config_wlan_bsc_set bsc_set;
*        sp_config_wlan_scy_set scy_set;
*        sp_config_wlan_advs_set advs_set;
*    } sp_config_wlan_info;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_wlan_info SP_CONFIG_WLAN_INFO;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_wlan_partition {
*        int32_t checksum;
*        int32_t sub_cfg_id;
*        pb_size_t band_count;
*        sp_config_band_info band[4];
*    } sp_config_wlan_partition;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_wlan_partition SP_CONFIG_WLAN_PARTITION;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*    typedef struct _sp_config_wlan {
*        int32_t checksum;
*        pb_size_t info_count;
*        sp_config_wlan_info info[32];
*    } sp_config_wlan;

*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_wlan SP_CONFIG_WLAN;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*   typedef struct _sp_config_config {
*       int32_t checksum;
*       sp_config_wlan wlan;
*       sp_config_radio radio;
*       sp_config_mesh mesh;
*       sp_config_captive_portal cp_portal;
*       sp_config_acl acl;
*       sp_config_qos qos;
*       sp_config_vlan vlan;
*       sp_config_arp_filter arp_flt;
*       sp_config_tm_sche tm_sche;
*       sp_config_wlan_partition wlan_prtn;
*       sp_config_device_mgmt dvc_mgmt;
*       sp_config_ap_array_setting ap_array;
*       int32_t bEncode;
*   } sp_config_config;
*
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_config SP_CONFIG_CONFIG;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*   typedef struct _sp_config_ap_array_setting {
*       int32_t checksum;
*       char name[32];
*       int32_t bPrefer_master;
*       int32_t filt_en;
*       int32_t add_auto_en;
*       pb_size_t mem_lst_count;
*       apson_register_info mem_lst[16];
*   } sp_config_ap_array_setting;
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_ap_array_setting SP_CONFIG_AP_ARRAY_SETTING;

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped  structure.
*
*   typedef PB_BYTES_ARRAY_T(6) sp_config_mesh_local_set_prefer_uplinkap_t;
*   typedef struct _sp_config_mesh_local_set {
*       int32_t checksum;
*       sp_config_mesh_local_set_prefer_uplinkap_t prefer_uplinkap;
*       int32_t prefer_map;
*   } sp_config_mesh_local_set;
*
* NOTES :
*
*H************************************************************************/
typedef sp_config_mesh_local_set SP_CONFIG_MESH_LOCAL_SET;

#endif //endif supermeshpro



//+++ [Add the new message]step 5: Wrap enum if there is using in message
/*********************Define wrapped message structure*********************/
/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Wrapped Example_ExEnum enum.
*
*       typedef enum ExampleEnum {
*       EX_ENUM_FIRST    = Example_ExEnum_First,
*       EX_ENUM_SECOND   = Example_ExEnum_Second,
*       EX_ENUM_THIRD    = Example_ExEnum_Third }EXAMPLE_EXENUM;
*
* NOTES :
*
*H************************************************************************/
typedef enum ExampleEnum {
    EX_ENUM_FIRST    = Example_ExEnum_First,
    EX_ENUM_SECOND   = Example_ExEnum_Second,
    EX_ENUM_THIRD    = Example_ExEnum_Third }EXAMPLE_EXENUM;

//+++ [Add the new message]step 8: Declare function name, encode, decode & get buffer size
/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       The example of how to encode all types message into buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_ex(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the allocated buffer for storing data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_ex(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       The example of how to decode buffer into structure.
*
* FUNCTIONS :
*       int apson_pb_decode_ex(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            OUTPUT      Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the buffer for decoding into pure data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_ex(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       The example of how to estimate the buffer's size should be allocated
*
* FUNCTIONS :
*       int apson_pb_get_buf_size_ex(void *strMsg, uint16_t *pBufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_size_ex(void *strMsg, uint32_t *pBufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       The is the example of how to encode/ decode data through apson pb api.
*
* FUNCTIONS :
*       int apson_pb_example(void)
*
* PARAMETERS :
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*
*H************************************************************************/
int apson_pb_example(void);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       The is the example of filling in data into structure
*
* FUNCTIONS :
*       void apson_pb_example_set_data(EX_ALLTYPES *strEncode)
*
* PARAMETERS :
*
* RETURN:
*
* NOTES :
*
*H************************************************************************/
void apson_pb_example_set_data(EX_ALLTYPES *strEncode);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       The is the example of printing structure's data
*
* FUNCTIONS :
*       void apson_pb_example_print_data(EX_ALLTYPES strDecode)
*
* PARAMETERS :
*
* RETURN:
*
* NOTES :
*
*H************************************************************************/
void apson_pb_example_print_data(EX_ALLTYPES strDecode);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Encode register info message into buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_register_info(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       char domain[32];
*       byte master_mac[6];
*       char name[32];
*       char location[32];
*       char serial_num[15];
*       apson_register_info_mac_t mac;
*       char model_name[16];
*       char radio0_type[1];
*       char radio0_mimo_streams[1];
*       char radio1_type[1];
*       char radio1_mimo_streams[1];
*       char radio2_type[1];
*       char radio2_mimo_streams[1];
*       char main_image_ver[16];
*       char backup_image_ver[16];
*       int32_t ap_ipv4;
*       int32_t ap_ipv6;
*       char latest_image_ver[16];
*       int32_t region_en;
*       char region_id[16];
*       char oui[6];
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_register_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Decode buffer into register info structure.
*
* FUNCTIONS :
*       int apson_pb_decode_register_info(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       char domain[32];
*       byte master_mac[6];
*       char name[32];
*       char location[32];
*       char serial_num[15];
*       apson_register_info_mac_t mac;
*       char model_name[16];
*       char radio0_type[1];
*       char radio0_mimo_streams[1];
*       char radio1_type[1];
*       char radio1_mimo_streams[1];
*       char radio2_type[1];
*       char radio2_mimo_streams[1];
*       char main_image_ver[16];
*       char backup_image_ver[16];
*       int32_t ap_ipv4;
*       int32_t ap_ipv6;
*       char latest_image_ver[16];
*       int32_t region_en;
*       char region_id[16];
*       char oui[6];
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_register_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Estimate the buffer's size of register info structure
*
* FUNCTIONS :
*       int apson_pb_get_buf_register_info(void *strMsg, uint16_t *pBufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_register_info(void *strMsg, uint32_t *pBufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Encode meshd config message into buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_meshd_config(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the allocated buffer for storing data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_meshd_config(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Decode buffer into meshd config structure.
*
* FUNCTIONS :
*       int apson_pb_decode_meshd_config(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            OUTPUT      Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the buffer for decoding into pure data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_meshd_config(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Estimate the buffer's size of meshd config structure
*
* FUNCTIONS :
*       int apson_pb_get_buf_meshd_config(void *strMsg, uint16_t *pBufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_meshd_config(void *strMsg, uint32_t *pBufsize);


//TBD
int apson_pb_encode_config_wlan(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_config_wlan(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_config_wlan(void *strMsg, uint32_t *pBufsize);


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Encode apson message config message into buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_config_msg(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the allocated buffer for storing data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_config_msg(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Decode buffer into apson message config structure.
*
* FUNCTIONS :
*       int apson_pb_decode_config_msg(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            OUTPUT      Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the buffer for decoding into pure data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_config_msg(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Estimate the buffer's size of apson message config structure
*
* FUNCTIONS :
*       int apson_pb_get_buf_config_msg(void *strMsg, uint16_t *pBufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_config_msg(void *strMsg, uint32_t *pBufsize);


/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Encode meshd self message into buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_msd_self(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the allocated buffer for storing data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_msd_self(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Decode buffer into meshd self structure.
*
* FUNCTIONS :
*       int apson_pb_decode_msd_self(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            OUTPUT      Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the buffer for decoding into pure data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_msd_self(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Estimate the buffer's size of meshd self structure
*
* FUNCTIONS :
*       int apson_pb_get_buf_msd_self(void *strMsg, uint16_t *pBufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_msd_self(void *strMsg, uint32_t *pBufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Encode meshd ap list message into buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_msd_ap_list(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the allocated buffer for storing data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_msd_ap_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Decode buffer into meshd ap list structure.
*
* FUNCTIONS :
*       int apson_pb_decode_msd_ap_list(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            OUTPUT      Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the buffer for decoding into pure data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_msd_ap_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Estimate the buffer's size of meshd ap list structure
*
* FUNCTIONS :
*       int apson_pb_get_buf_msd_ap_list(void *strMsg, uint16_t *pBufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_msd_ap_list(void *strMsg, uint32_t *pBufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Encode meshd mesh dev message into buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_msd_mesh_dev(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the allocated buffer for storing data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_msd_mesh_dev(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Decode buffer into meshd mesh dev structure.
*
* FUNCTIONS :
*       int apson_pb_decode_msd_mesh_dev(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            OUTPUT      Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the buffer for decoding into pure data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_msd_mesh_dev(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Estimate the buffer's size of meshd mesh dev structure
*
* FUNCTIONS :
*       int apson_pb_get_buf_msd_mesh_dev(void *strMsg, uint16_t *pBufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_msd_mesh_dev(void *strMsg, uint32_t *pBufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Encode meshd mesh_ie message into buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_msd_mesh_ie(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the allocated buffer for storing data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_msd_mesh_ie(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Decode buffer into meshd mesh_ie structure.
*
* FUNCTIONS :
*       int apson_pb_decode_msd_mesh_ie(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            OUTPUT      Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the buffer for decoding into pure data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_msd_mesh_ie(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Estimate the buffer's size of meshd mesh_ie structure
*
* FUNCTIONS :
*       int apson_pb_get_buf_msd_mesh_ie(void *strMsg, uint16_t *pBufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_msd_mesh_ie(void *strMsg, uint32_t *pBufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Encode meshd timeval message into buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_msd_timeval(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the allocated buffer for storing data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_msd_timeval(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Decode buffer into meshd timeval structure.
*
* FUNCTIONS :
*       int apson_pb_decode_msd_timeval(void *strMsg, uint8_t *pBuffer, uint16_t u16Bufsize)
*
* PARAMETERS :
*       void *strMsg            OUTPUT      Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the buffer for decoding into pure data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_msd_timeval(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Estimate the buffer's size of meshd timeval structure
*
* FUNCTIONS :
*       int apson_pb_get_buf_msd_timeval(void *strMsg, uint16_t *pBufsize)
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_msd_timeval(void *strMsg, uint32_t *pBufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Encode profile message into buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_profile(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the allocated buffer for storing data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_profile(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Decode profile buffer into  structure.
*
* FUNCTIONS :
*       int apson_pb_decode_profile(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
*
* PARAMETERS :
*       void *strMsg            OUTPUT      Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the buffer for decoding into pure data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_profile(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Estimate the profile buffer's size of structure
*
* FUNCTIONS :
*       int apson_pb_get_buf_profile(void *strMsg, uint32_t *pBufsize);
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_profile(void *strMsg, uint32_t *pBufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Encode profile apson message into buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_profile_apson(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the allocated buffer for storing data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_profile_apson(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Decode buffer into profile apson structure.
*
* FUNCTIONS :
*       int apson_pb_decode_profile_apson(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
*
* PARAMETERS :
*       void *strMsg            OUTPUT      Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the buffer for decoding into pure data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_profile_apson(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Estimate the buffer's size of profile apson structure
*
* FUNCTIONS :
*       int apson_pb_get_buf_profile_apson(void *strMsg, uint32_t *pBufsize);
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_profile_apson(void *strMsg, uint32_t *pBufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Encode profile mesh message into buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_profile_mesh(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the allocated buffer for storing data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_profile_mesh(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Decode buffer into profile mesh structure.
*
* FUNCTIONS :
*       int apson_pb_decode_profile_mesh(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
*
* PARAMETERS :
*       void *strMsg            OUTPUT      Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the buffer for decoding into pure data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_profile_mesh(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Estimate the buffer's size of profile mesh structure
*
* FUNCTIONS :
*       int apson_pb_get_buf_profile_mesh(void *strMsg, uint32_t *pBufsize);
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_profile_mesh(void *strMsg, uint32_t *pBufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Encode mcs_to_datarate message into buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_profile_mcs_datarate(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the allocated buffer for storing data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_profile_mcs_datarate(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Decode buffer into profile_mcs_satarate structure.
*
* FUNCTIONS :
*       int apson_pb_decode_profile_mcs_datarate(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
*
* PARAMETERS :
*       void *strMsg            OUTPUT      Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the buffer for decoding into pure data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_profile_mcs_datarate(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Estimate the buffer's size of profile_mcs_datarate structure
*
* FUNCTIONS :
*       int apson_pb_get_buf_profile_mcs_datarate(void *strMsg, uint32_t *pBufsize);
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_profile_mcs_datarate(void *strMsg, uint32_t *pBufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Encode message into profile_snr_mcs buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_profile_snr_mcs(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the allocated buffer for storing data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_profile_snr_mcs(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Decode buffer into profile_snr_mcs structure.
*
* FUNCTIONS :
*       int apson_pb_decode_profile_snr_mcs(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
*
* PARAMETERS :
*       void *strMsg            OUTPUT      Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the buffer for decoding into pure data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_profile_snr_mcs(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Estimate the buffer's size of profile_snr_mcs structure
*
* FUNCTIONS :
*       int apson_pb_get_buf_profile_snr_mcs(void *strMsg, uint32_t *pBufsize);
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_profile_snr_mcs(void *strMsg, uint32_t *pBufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Encode message into ma_wlan_stats buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_ap_db_ma_wlan_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the allocated buffer for storing data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_ap_db_ma_wlan_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Decode buffer into ma_wlan_stats structure.
*
* FUNCTIONS :
*       int apson_pb_decode_ap_db_ma_wlan_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
*
* PARAMETERS :
*       void *strMsg            OUTPUT      Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the buffer for decoding into pure data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_ap_db_ma_wlan_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Estimate the buffer's size of ma_wlan_stats structure
*
* FUNCTIONS :
*       int apson_pb_get_buf_ap_db_ma_wlan_stats(void *strMsg, uint32_t *pBufsize);
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_ap_db_ma_wlan_stats(void *strMsg, uint32_t *pBufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Encode message into ma_wlan_stats_list buffer.
*
* FUNCTIONS :
*       int apson_pb_encode_ap_db_ma_wlan_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the allocated buffer for storing data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_encode_ap_db_ma_wlan_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Decode buffer into ma_wlan_stats_list structure.
*
* FUNCTIONS :
*       int apson_pb_decode_ap_db_ma_wlan_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
*
* PARAMETERS :
*       void *strMsg            OUTPUT      Give the strcture with data for encoding to buffer.
*       uint8_t *pBuffer        OUTPUT      Give the buffer for decoding into pure data.
*       uint16_t u16Bufsize     INPUT       Give the buffer size.
*
* RETURN:
*       FAIL / SIZE OF ENCODED DATA
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_decode_ap_db_ma_wlan_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

/*H**********************************************************************
* FILENAME :    apsonpb.c
*
* DESCRIPTION :
*       Estimate the buffer's size of ma_wlan_stats_list structure
*
* FUNCTIONS :
*       int apson_pb_get_buf_ap_db_ma_wlan_stats_list(void *strMsg, uint32_t *pBufsize);
*
* PARAMETERS :
*       void *strMsg            INPUT       Give the strcture with data for estimating buffer's size.
*       uint16_t u16Bufsize     OUTPUT      Store the estimated buffer's size.
*
* RETURN:
*       FAIL / SUCCESS
*
* NOTES :
*       Wrapped function for maintainer of pb moudle.
*
*H************************************************************************/
int apson_pb_get_buf_ap_db_ma_wlan_stats_list(void *strMsg, uint32_t *pBufsize);




//TBD: Add the header comment
int apson_pb_encode_ap_db_wlan_bin_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_wlan_bin_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_wlan_bin_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_sta_bin_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_sta_bin_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_sta_bin_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_sys_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_sys_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_sys_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_radio_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_radio_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_radio_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_radio_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_radio_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_radio_stats_list(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ma_sta_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ma_sta_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ma_sta_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ma_sta_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ma_sta_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ma_sta_stats_list(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_sta_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_sta_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_sta_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_sta_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_sta_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_sta_stats_list(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_mesh_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_mesh_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_mesh_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_mesh_dl_wlan(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_mesh_dl_wlan(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_mesh_dl_wlan(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_mesh_nb_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_mesh_nb_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_mesh_nb_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_mesh_nb_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_mesh_nb_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_mesh_nb_stats_list(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_mesh_dl_ap_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_mesh_dl_ap_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_mesh_dl_ap_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_mesh_dl_ap_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_mesh_dl_ap_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_mesh_dl_ap_stats_list(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_wlan_activity_status(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_wlan_activity_status(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_wlan_activity_status(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_wlan_list_status(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_wlan_list_status(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_wlan_list_status(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_wlan_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_wlan_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_wlan_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_wlan_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_wlan_stats_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_wlan_stats_list(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_mesh_module_msg_client_associate(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_mesh_module_msg_client_associate(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_mesh_module_msg_client_associate(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_mesh_module_msg_client_authorized(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_mesh_module_msg_client_authorized(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_mesh_module_msg_client_authorized(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_mesh_module_msg_ip_assigned(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_mesh_module_msg_ip_assigned(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_mesh_module_msg_ip_assigned(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_mesh_module_msg_client_disconnect(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_mesh_module_msg_client_disconnect(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_mesh_module_msg_client_disconnect(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_mesh_module_msg_ul_connect(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_mesh_module_msg_ul_connect(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_mesh_module_msg_ul_connect(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_mesh_module_msg_ul_authorized(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_mesh_module_msg_ul_authorized(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_mesh_module_msg_ul_authorized(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_mesh_module_msg_ul_disconnect(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_mesh_module_msg_ul_disconnect(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_mesh_module_msg_ul_disconnect(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_mesh_module_msg_weak_client_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_mesh_module_msg_weak_client_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_mesh_module_msg_weak_client_info(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_mesh_module_cmd_set_monitor_client(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_mesh_module_cmd_set_monitor_client(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_mesh_module_cmd_set_monitor_client(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_mesh_module_msg_monitor_client_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_mesh_module_msg_monitor_client_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_mesh_module_msg_monitor_client_info(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_module_cmd_set_client_criteria(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_module_cmd_set_client_criteria(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_module_cmd_set_client_criteria(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_apson_mesh_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_apson_mesh_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_apson_mesh_info(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_stats_reboot_ap(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_stats_reboot_ap(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_stats_reboot_ap(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_stats_kickout_client(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_stats_kickout_client(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_stats_kickout_client(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_stats_specific_ap_img_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_stats_specific_ap_img_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_stats_specific_ap_img_info(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_stats_last_released_img_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_stats_last_released_img_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_stats_last_released_img_info(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_stats_ap_img_updgrade(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_stats_ap_img_updgrade(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_stats_ap_img_updgrade(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_stats_ap_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_stats_ap_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_stats_ap_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_stats_wlan_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_stats_wlan_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_stats_wlan_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_stats_sta_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_stats_sta_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_stats_sta_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_stats_mesh_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_stats_mesh_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_stats_mesh_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_stats_sys_width_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_stats_sys_width_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_stats_sys_width_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_ap_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_ap_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_ap_db_ap_list(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_apson(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_apson(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_apson(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_wlan_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_wlan_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_wlan_info(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_wlan(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_wlan(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_wlan(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_radio(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_radio(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_radio(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_mesh(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_mesh(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_mesh(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_profile(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_profile(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_profile(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_dev_setting(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_dev_setting(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_dev_setting(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_mac_del_flt(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_mac_del_flt(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_mac_del_flt(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_mac_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_mac_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_mac_info(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_time_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_time_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_time_info(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_time_setting(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_time_setting(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_time_setting(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_wps(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_wps(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_wps(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_mac_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_mac_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_mac_list(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_schedule_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_schedule_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_schedule_info(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_mac_flt(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_mac_flt(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_mac_flt(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_schedule_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_schedule_list(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_schedule_list(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_schedule_setting(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_schedule_setting(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_schedule_setting(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cfg_radio_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_radio_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_size_cfg_radio_info(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_stats_bin_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_stats_bin_stats(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_size_ap_stats_bin_stats(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_stats_bin_data(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_stats_bin_data(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_size_ap_stats_bin_data(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_auto_asgmt_site_svy_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_auto_asgmt_site_svy_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_size_auto_asgmt_site_svy_info(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_client_beacon_measure(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_client_beacon_measure(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_size_ap_db_client_beacon_measure(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_ap_db_beacon_report(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_ap_db_beacon_report(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_size_ap_db_beacon_report(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_null(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_null(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_size_null(void *strMsg, uint32_t *pBufsize);

#ifdef _SUPER_MESH_PRO_
int apson_pb_encode_sp_cfg_wlan_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_wlan_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_wlan_info(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_wlan(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_wlan(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_wlan(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_wlan_bsc_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_wlan_bsc_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_wlan_bsc_set(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_wlan_scy_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_wlan_scy_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_wlan_scy_set(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_wlan_advs_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_wlan_advs_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_wlan_advs_set(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_radio(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_radio(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_radio(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_radio_bsc_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_radio_bsc_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_radio_bsc_set(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_radio_rsrc(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_radio_rsrc(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_radio_rsrc(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_mesh(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_mesh(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_mesh(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_cp(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_cp(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_cp(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_cp_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_cp_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_cp_set(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_cp_cfg_ip_flt(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cp_cfg_ip_flt(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cp_cfg_ip_flt(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_mac_bps(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_mac_bps(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_mac_bps(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_acl(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_acl(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_acl(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_radio_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_radio_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_radio_set(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_clt_acl_lst(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_clt_acl_lst(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_clt_acl_lst(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_qos(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_qos(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_qos(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_qos_bsc_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_qos_bsc_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_qos_bsc_set(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_qos_rule(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_qos_rule(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_qos_rule(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_vlan(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_vlan(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_vlan(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_vlan_lst(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_vlan_lst(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_vlan_lst(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_pvid_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_pvid_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_pvid_set(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_arp_flt(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_arp_flt(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_arp_flt(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_gw_lst(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_gw_lst(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_gw_lst(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_tm_sche(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_tm_sche(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_tm_sche(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_tm_rule_lst(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_tm_rule_lst(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_tm_rule_lst(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_wlan_prtn(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_wlan_prtn(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_wlan_prtn(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_band_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_band_info(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_band_info(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_dvc_mgmt(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_dvc_mgmt(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_dvc_mgmt(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_cfg(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_cfg(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_cfg(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_ap_array_setting(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_ap_array_setting(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_ap_array_setting(void *strMsg, uint32_t *pBufsize);
int apson_pb_encode_sp_cfg_mesh_local_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_sp_cfg_mesh_local_set(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_sp_cfg_mesh_local_set(void *strMsg, uint32_t *pBufsize);

#endif //endif supermeshpro

int apson_pb_encode_cfg_roam(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_decode_cfg_roam(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
int apson_pb_get_buf_cfg_roam(void *strMsg, uint32_t *pBufsize);



/*extern three functions for someon who wants to use lobapsonpb.so*/
extern int (* apson_pb_get_buf_size[PB_LAST])(void *strMsg, uint32_t *pBufsize);
extern int (* apson_pb_decode_msg[PB_LAST])(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);
extern int (* apson_pb_encode_msg[PB_LAST])(void *strMsg, uint8_t *pBuffer, uint32_t u32Bufsize);

#endif
