/****************************************************************************
 * Mediatek Inc.
 * 5F., No.5, Taiyuan 1st St., Zhubei City, 
 * Hsinchu County 302, Taiwan, R.O.C.
 * (c) Copyright 2014, Mediatek, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************
 
    Module Name:
    wifi_fwd.c
 
    Abstract:

    Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
     Annie Lu  2014-06-30	  Initial version
 */
#include "ez_cmm.h"
#include "dot11i_wpa.h"

#include "ez_hooks_proto.h"
#include "ez_lib_proto.h"


extern UCHAR BROADCAST_ADDR[MAC_ADDR_LEN];
extern UCHAR ZERO_MAC_ADDR[MAC_ADDR_LEN];
extern UCHAR	IPV4TYPE[];

extern unsigned char mtk_oui[MTK_OUI_LEN];
EZ_ADAPTER *ez_adapter = 0;


#if 1

EZ_ADAPTER *ez_get_adapter_hook(void)
{
	return ez_adapter;
}
#if 1
EZ_ADAPTER * ez_init_hook(void *driver_ad, void *wdev, unsigned char ap_mode)
{
	UINT8 i = 0;

	printk("----------------------%s()----------------------\n", __FUNCTION__);

	if (!ap_mode)
		return ez_adapter;

	if (ez_adapter == NULL) {
		printk("MOD NOT initialized yet\n");
		EZ_MEM_ALLOC(NULL, &ez_adapter, sizeof(EZ_ADAPTER));
		if (ez_adapter == NULL) {
			printk("MALLOC returned NULL\n");
			return NULL;
		}
		NdisZeroMemory(ez_adapter,sizeof(EZ_ADAPTER));
		ez_adapter->drv_ad_cnt = 0;
		ez_adapter->debug = DBG_LVL_ERROR;
		ez_adapter->ez_roam_time = 60;				//sec
		ez_adapter->ez_delay_disconnect_count = 4;
		ez_adapter->ez_wait_for_info_transfer = 1;		//sec
		ez_adapter->ez_wdl_missing_time = 8;			//sec
		ez_adapter->ez_force_connect_bssid_time = 300;		//sec
		ez_adapter->ez_peer_entry_age_out_time = 4;		//sec
		ez_adapter->ez_scan_same_channel_time = 60;		//sec
		ez_adapter->ez_partial_scan_time= 300;			//sec

		OS_NdisAllocateSpinLock(&ez_adapter->ez_conn_perm_lock);
		OS_NdisAllocateSpinLock(&ez_adapter->ez_handle_disconnect_lock);
		OS_NdisAllocateSpinLock(&ez_adapter->ez_mlme_sync_lock);
		OS_NdisAllocateSpinLock(&ez_adapter->ez_beacon_update_lock);
		OS_NdisAllocateSpinLock(&ez_adapter->ez_miniport_lock);
		OS_NdisAllocateSpinLock(&ez_adapter->ez_set_channel_lock);
		OS_NdisAllocateSpinLock(&ez_adapter->ez_set_peer_lock);
	}

	for (i = 0; i < MAX_DRV_AD_CNT; i++) {
		if (driver_ad && (ez_adapter->drv_ad[i] == driver_ad)) {
			printk("Current ad already in ez_adapter->drv_ad[%d]=%p.\n", i, driver_ad);
			return ez_adapter;
		}
	}

	for (i = 0; i < MAX_DRV_AD_CNT; i++) {
		if (!ez_adapter->drv_ad[i]) {
			ez_adapter->drv_ad[i] = driver_ad;
			ez_adapter->drv_ad_cnt++;
			printk("allocate empty ez_adapter->drv_ad[%d]=%p.\n", i, driver_ad);
			break;
		}
	}

	switch (ez_adapter->drv_ad_cnt) {
		case 1:
			ez_adapter->SingleChip = TRUE;
			printk("it's single chip.\n");
			break;
		case 2:
			ez_adapter->SingleChip = FALSE;
			printk("It's 2 chips DBDC.\n");
			break;
		case 3:
			ez_adapter->SingleChip = FALSE;
			printk("It's 3 chips.\n");
			break;
		default:
			printk("There must be wrong, drv_ad_cnt=%d.\n", ez_adapter->drv_ad_cnt);
			ASSERT(FALSE);
			break;
	}
	return ez_adapter;
}

void ez_exit_hook(
	void *driver_ad)
{
	UINT8 i = 0;

	if (ez_adapter == NULL) {
		printk("\n %s() ERROR ez_adapter is null\n", __FUNCTION__);
		return ;
	}

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("--> %s()\n", __FUNCTION__));

	for (i = 0; i < MAX_DRV_AD_CNT; i++) {
		if (ez_adapter->drv_ad[i] == driver_ad) {
			ez_adapter->drv_ad[i] = NULL;
			ez_adapter->drv_ad_cnt--;

			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
				("ez_exit_hook:remove drv_ad[%d]=%p, drv_ad_cnt=%d\n",
				i, driver_ad, ez_adapter->drv_ad_cnt));

			if (ez_adapter->drv_ad_cnt == 0) {
				OS_NdisFreeSpinLock(&ez_adapter->ez_beacon_update_lock);
				OS_NdisFreeSpinLock(&ez_adapter->ez_miniport_lock);
				OS_NdisFreeSpinLock(&ez_adapter->ez_set_channel_lock);
				OS_NdisFreeSpinLock(&ez_adapter->ez_set_peer_lock);
				OS_NdisFreeSpinLock(&ez_adapter->ez_conn_perm_lock);
				OS_NdisFreeSpinLock(&ez_adapter->ez_handle_disconnect_lock);
				OS_NdisFreeSpinLock(&ez_adapter->ez_mlme_sync_lock);

				EZ_MEM_FREE(ez_adapter);
				ez_adapter = 0;
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
					("ez_exit_hook(), release ez_adapter=0.\n"));
				break;
			}
		}
	}

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("<--%s()\n", __FUNCTION__));
}

#else

EZ_ADAPTER * ez_init_hook(void *driver_ad, void *wdev, unsigned char ap_mode)
{
	printk("###########################%s########################\n", __FUNCTION__);
	if (ap_mode)
	{
		printk("AP mode\n");
		if (ez_adapter != NULL)
		{
			if (ez_adapter->first_ad == NULL)
			{
				printk("No First AD while ez_adapter is allocated\n");
				ASSERT(FALSE);
			} else {
				if (ez_adapter->second_ad == NULL)
				{
					ez_adapter->second_ad = driver_ad;
					if (ez_adapter->second_ad == ez_adapter->first_ad)
					{
							OS_NdisFreeSpinLock(&ez_adapter->ez_beacon_update_lock);
			                OS_NdisFreeSpinLock(&ez_adapter->ez_miniport_lock);
			                OS_NdisFreeSpinLock(&ez_adapter->ez_set_channel_lock);
							OS_NdisFreeSpinLock(&ez_adapter->ez_set_peer_lock);
			
						printk("its single chip\n");
					} else {
						OS_NdisAllocateSpinLock(&ez_adapter->ez_conn_perm_lock);
						OS_NdisAllocateSpinLock(&ez_adapter->ez_handle_disconnect_lock);
						OS_NdisAllocateSpinLock(&ez_adapter->ez_mlme_sync_lock);
						OS_NdisAllocateSpinLock(&ez_adapter->ez_beacon_update_lock);
	                    OS_NdisAllocateSpinLock(&ez_adapter->ez_miniport_lock);
	                    OS_NdisAllocateSpinLock(&ez_adapter->ez_set_channel_lock);
						OS_NdisAllocateSpinLock(&ez_adapter->ez_set_peer_lock);
						ez_adapter->SingleChip = FALSE;
						ez_adapter->unique_ad_count += 1;
						printk("Its 2 chip DBDC\n");
					}
				} else if (ez_adapter->third_ad == NULL) {
					ez_adapter->third_ad = driver_ad;
					if (ez_adapter->third_ad == ez_adapter->first_ad
						|| ez_adapter->third_ad == ez_adapter->second_ad)
					{
						printk("its single chip DBDC on atleast one chip\n");
						ez_adapter->SingleChip = FALSE;
					} else {
						ez_adapter->unique_ad_count += 1;
					}
				} else {
					printk("There are 4 AP interfaces\n");
					ASSERT(FALSE);
				}
			}
		} else {
			printk("MOD NOT initialized yet\n");
			EZ_MEM_ALLOC(NULL,&ez_adapter,sizeof(EZ_ADAPTER));
			if (ez_adapter != NULL)
			{
				NdisZeroMemory(ez_adapter,sizeof(EZ_ADAPTER));
				ez_adapter->first_ad = driver_ad;
				ez_adapter->unique_ad_count = 1;
				ez_adapter->debug = DBG_LVL_ERROR;
				ez_adapter->SingleChip = TRUE;
				ez_adapter->ez_roam_time = 60;					//sec
				ez_adapter->ez_delay_disconnect_count = 4;
				ez_adapter->ez_wait_for_info_transfer = 1;		//sec
				ez_adapter->ez_wdl_missing_time = 8;			//sec
				ez_adapter->ez_force_connect_bssid_time = 300;	//sec
				ez_adapter->ez_peer_entry_age_out_time = 4;		//sec
				ez_adapter->ez_scan_same_channel_time = 60;		//sec
				ez_adapter->ez_partial_scan_time= 300;			//sec
				OS_NdisAllocateSpinLock(&ez_adapter->ez_beacon_update_lock);
	            OS_NdisAllocateSpinLock(&ez_adapter->ez_miniport_lock);
        	    OS_NdisAllocateSpinLock(&ez_adapter->ez_set_channel_lock);
				OS_NdisAllocateSpinLock(&ez_adapter->ez_set_peer_lock);
			} else {
				printk("MALLOC returned NULL\n");
			}
		}
	}
	return ez_adapter;
}


void ez_exit_hook(
	void *driver_ad)
{
	if(ez_adapter == NULL)
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("\n %s() ERROR ez_adapter is null\n", __FUNCTION__));
		return ;
	}
	if ((ez_adapter->first_ad == driver_ad))
	{
		ez_adapter->first_ad = NULL;
	}
	if ((ez_adapter->second_ad == driver_ad))
	{
		ez_adapter->second_ad = NULL;
	}
	if ((ez_adapter->third_ad == driver_ad))
	{
		ez_adapter->third_ad = NULL;
	}

	if ((ez_adapter->first_ad == NULL)
		&& (ez_adapter->second_ad == NULL)
		&& (ez_adapter->third_ad == NULL))
	{
		OS_NdisFreeSpinLock(&ez_adapter->ez_beacon_update_lock);
		OS_NdisFreeSpinLock(&ez_adapter->ez_miniport_lock);
		OS_NdisFreeSpinLock(&ez_adapter->ez_set_channel_lock);
		OS_NdisFreeSpinLock(&ez_adapter->ez_set_peer_lock);
		OS_NdisFreeSpinLock(&ez_adapter->ez_conn_perm_lock);
		OS_NdisFreeSpinLock(&ez_adapter->ez_handle_disconnect_lock);		
     	OS_NdisFreeSpinLock(&ez_adapter->ez_mlme_sync_lock);
		EZ_MEM_FREE(ez_adapter);
		ez_adapter = 0;
	}
}
#endif
BOOLEAN ez_need_bypass_rx_fwd_hook(ez_dev_t *ezdev)
{
	interface_info_t other_band_config;

	return (ezdev->ezdev_type == EZDEV_TYPE_APCLI && ez_get_other_band_info(ezdev, &other_band_config)
								&& !ezdev->ez_security.this_band_info.shared_info.link_duplicate 
								&& !MAC_ADDR_EQUAL(other_band_config.cli_peer_ap_mac ,ZERO_MAC_ADDR));

}


#ifdef IF_UP_DOWN

BOOLEAN ez_check_valid_hook()
{
	EZ_ADAPTER *ez_ad= ez_adapter;
	return !ez_ad->ez_all_intf_up_once;
}


BOOLEAN ez_all_ez_intf_up(EZ_ADAPTER *ez_ad)
{
	if (ez_ad->ez_intf_count_config_ap == ez_ad->ez_intf_count_current_ap
		&& ez_ad->ez_intf_count_config_cli == ez_ad->ez_intf_count_current_cli)
		return TRUE;
	return FALSE;
}

BOOLEAN ez_all_intf_up_hook(EZ_ADAPTER *ez_ad)
{
	if (ez_ad->ez_intf_count_config_ap == ez_ad->ez_intf_count_current_ap
		&& ez_ad->ez_intf_count_config_cli == ez_ad->ez_intf_count_current_cli
		&& ez_ad->non_ez_intf_count_config_ap == ez_ad->non_ez_intf_count_current_ap
		&& ez_ad->non_ez_intf_count_config_cli == ez_ad->non_ez_intf_count_current_cli)
		return TRUE;
	return FALSE;
}


void ez_apcli_disconnect_both_intf_hook(ez_dev_t *ezdev, PUCHAR bssid)
{
	struct _ez_peer_security_info * ez_peer = NULL;

	if(ezdev->wdev == NULL)
		return;

	ez_peer = ez_peer_table_search_by_addr_hook(ezdev,bssid);

	if(ez_peer)
	{
		struct _ez_peer_security_info *ez_other_band_peer = NULL;
		ez_dev_t *other_band_ezdev = NULL;
	
		other_band_ezdev = ez_get_otherband_ezdev(ezdev);
		if (other_band_ezdev)
		{
			ez_other_band_peer = ez_get_other_band_ez_peer(ezdev,ez_peer);
			if(ez_other_band_peer)
			{
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("Disconnect other band APCLI\n"));
				other_band_ezdev->driver_ops->ez_send_unicast_deauth
						(other_band_ezdev,ez_other_band_peer->mac_addr);
			}
		}
		ezdev->driver_ops->ez_send_unicast_deauth
				(ezdev,ez_peer->mac_addr);
	}
}

#endif
ez_dev_t * ez_start_hook(ez_init_params_t *ez_init_params)
{
	int allocated_band= -1;
	struct _ez_security *ez_sec_info;
	ez_dev_t *ezdev;	
	int i=0;
	allocated_band = ez_allocate_or_update_band(ez_adapter,ez_init_params);
	
	if (allocated_band == -1)
		return NULL;
	if (ez_init_params->ezdev_type == EZDEV_TYPE_AP)
	{
		ezdev = &ez_adapter->ez_band_info[allocated_band].ap_ezdev;
		OS_NdisAllocateSpinLock(&ez_adapter->ez_band_info[allocated_band].ez_peer_table_lock);
#ifdef IF_UP_DOWN
		ez_adapter->ez_intf_count_config_ap = ez_init_params->ez_intf_count_config_ap;
		ez_adapter->ez_intf_count_config_cli = ez_init_params->ez_intf_count_config_cli;
		ez_adapter->non_ez_intf_count_config_ap = ez_init_params->non_ez_intf_count_config_ap;
		ez_adapter->non_ez_intf_count_config_cli = ez_init_params->non_ez_intf_count_config_cli;
#endif
	}
	else
	{
		ezdev = &ez_adapter->ez_band_info[allocated_band].cli_ezdev;
	}

	ezdev->ez_band_idx = allocated_band;
	ezdev->ez_ad = ez_adapter;
	ezdev->driver_ad = ez_init_params->ad_obj;
	ezdev->ezdev_type = ez_init_params->ezdev_type;
	ezdev->channel = ez_init_params->channel;
	ezdev->driver_ops = &ez_adapter->ez_band_info[allocated_band].lut_driver_ops;
	ezdev->ez_peer_table_lock = &ez_adapter->ez_band_info[allocated_band].ez_peer_table_lock;
	NdisCopyMemory(ezdev->driver_ops ,ez_init_params->driver_ops_lut,sizeof(ez_driver_ops_t));
	COPY_MAC_ADDR(ezdev->if_addr,ez_init_params->mac_add);
	if (ez_init_params->ezdev_type == EZDEV_TYPE_AP)
	{
		COPY_MAC_ADDR(ezdev->bssid,ez_init_params->mac_add);
	}
	ezdev->os_hz = ez_init_params->os_hz;
	ez_sec_info = &ezdev->ez_security;
	
	ez_sec_info->this_band_info.shared_info.ssid_len = ez_init_params->ssid_len;
	NdisCopyMemory(ez_sec_info->this_band_info.shared_info.ssid,
		ez_init_params->ssid,ez_init_params->ssid_len);
	NdisCopyMemory(ez_sec_info->this_band_info.pmk,ez_init_params->pmk,EZ_PMK_LEN);
	NdisZeroMemory(ez_sec_info->this_band_info.psk, EZ_LEN_PSK);
	NdisCopyMemory(ez_sec_info->this_band_info.psk, ez_init_params->psk, strlen(ez_init_params->psk));

	EZ_MEM_ALLOC(NULL,&ez_sec_info->group_id,ez_init_params->group_id_len);
	NdisCopyMemory(ez_sec_info->group_id,ez_init_params->group_id,ez_init_params->group_id_len);
	ez_sec_info->group_id_len = ez_init_params->group_id_len;

	EZ_MEM_ALLOC(NULL,&ez_sec_info->ez_group_id,ez_init_params->ez_group_id_len);
	NdisCopyMemory(ez_sec_info->ez_group_id,ez_init_params->ez_group_id,ez_init_params->ez_group_id_len);
	ez_sec_info->ez_group_id_len = ez_init_params->ez_group_id_len;
	
	if (ez_init_params->gen_group_id_len)
	{		
		EZ_MEM_ALLOC(NULL,&ez_sec_info->gen_group_id,ez_init_params->gen_group_id_len);
		NdisCopyMemory(ez_sec_info->gen_group_id,ez_init_params->gen_group_id,ez_init_params->gen_group_id_len);
		ez_sec_info->gen_group_id_len = ez_init_params->gen_group_id_len;
	}

	ez_sec_info->ez_api_mode = ez_adapter->ez_api_mode;

	NdisCopyMemory(ez_sec_info->open_group_id,
		ez_init_params->open_group_id,ez_init_params->open_group_id_len);

	ez_sec_info->open_group_id_len = ez_init_params->open_group_id_len;

	ez_sec_info->ez_scan_timer = ez_init_params->ez_scan_timer;
	//ez_sec_info->ez_stop_scan_timer = ez_init_params->ez_scan_timer;
	ez_sec_info->ez_scan_pause_timer = ez_init_params->ez_scan_pause_timer;
	ez_sec_info->ez_group_merge_timer = ez_init_params->ez_group_merge_timer;
	ez_sec_info->ez_loop_chk_timer = ez_init_params->ez_loop_chk_timer;
	ez_sec_info->this_band_info.interface_activated = TRUE;

	
	if (ez_sec_info->rssi_threshold == 0)
			ez_sec_info->rssi_threshold = EZ_DEFAULT_RSSI_THRESHOLD;

	ezdev->ez_connect_wait_timer_backup = ez_init_params->ez_connect_wait_timer;

	//if (ez_adapter->ez_connect_wait_timer == NULL) {

		ez_adapter->ez_connect_wait_timer = ez_init_params->ez_connect_wait_timer;
		ez_adapter->ez_connect_wait_ezdev = ezdev;
//	}
	ez_adapter->best_ap_rssi_threshld_max = 1;

	ez_gen_dh_public_key(ezdev);
	ezdev->driver_ops->GenRandom(ezdev, ezdev->if_addr, &ez_sec_info->self_nonce[0]);

	NdisCopyMemory(&ez_sec_info->this_band_info.shared_info.channel_info,
		&ez_init_params->channel_info,
		sizeof(channel_info_t));
#ifdef IF_UP_DOWN
	if(ez_adapter->ez_all_intf_up_once) {
		//ezdev->ez_security.internal_force_connect_bssid_timeout = TRUE;
		ezdev->ez_security.ez_is_connection_allowed = TRUE;
	}
	else
#endif
	{
	ezdev->ez_security.ez_is_connection_allowed = FALSE;
	}
	ezdev->ez_security.ez_scan_same_channel = FALSE;

	ezdev->wdev = ez_init_params->wdev_obj;//! this should be the last step as it indicates that ezdev is valid
	if (ez_init_params->ezdev_type == EZDEV_TYPE_AP)
	{
#ifdef IF_UP_DOWN
		ez_adapter->ez_intf_count_current_ap++;
#endif
	} else {
#ifdef IF_UP_DOWN
		ez_adapter->ez_intf_count_current_cli++;
#endif
	}
	for (i = 0; i < 10; i ++)
	{
		ez_adapter->best_ap_rssi_threshld[i] = -110;
	}

#ifdef IF_UP_DOWN
		if (ez_all_ez_intf_up(ez_adapter) == TRUE)
			ez_adapter->ez_all_intf_up_once = TRUE;
#endif

	ez_adapter->sanity_check1 = &ez_adapter->sanity_check1;
	ez_adapter->sanity_check = &ez_adapter->sanity_check;
	printk("##############################MOD#####################################\n");
	printk("content of sanity1 ==== %p\n", ez_adapter->sanity_check1);
	printk("Address of sanity1 ==== %p\n", &ez_adapter->sanity_check1);
	printk("content of sanity ==== %p\n", ez_adapter->sanity_check);
		printk("Address of sanity ==== %p\n", &ez_adapter->sanity_check);
		printk("sizeof EZ_ADAPTER = %d\n", sizeof(EZ_ADAPTER));
		printk("sizeof ez_dev_t = %d\n", sizeof(ez_dev_t));
#ifdef IF_UP_DOWN
	printk("Interface Cnt: ap: %d, cli: %d\n",ez_adapter->ez_intf_count_current_ap,
												ez_adapter->ez_intf_count_current_cli);
#endif

		printk("###################################################################\n");
#if 0

	ez_adapter->sanity_check2 = &ez_adapter->sanity_check2;
			
	ez_adapter->sanity_check3 = &ez_adapter->sanity_check3;
	ez_adapter->sanity_check4 = &ez_adapter->sanity_check4;
	ez_adapter->sanity_check5 = &ez_adapter->sanity_check5;
	ez_adapter->sanity_check6 = &ez_adapter->sanity_check6;
	ez_adapter->sanity_check7 = &ez_adapter->sanity_check7;
		

	ez_adapter->sanity_check = &ez_adapter->sanity_check;
		printk("##############################MOD#####################################\n");
		printk("content of sanity1 ==== %p\n", ez_adapter->sanity_check1);
		printk("Address of sanity1 ==== %p\n", &ez_adapter->sanity_check1);
		printk("content of sanity2 ==== %p\n", ez_adapter->sanity_check2);
		printk("Address of sanity2 ==== %p\n", &ez_adapter->sanity_check2);
		printk("content of sanity3 ==== %p\n", ez_adapter->sanity_check3);
		printk("Address of sanity3 ==== %p\n", &ez_adapter->sanity_check3);
		printk("content of sanity4 ==== %p\n", ez_adapter->sanity_check4);
		printk("Address of sanity4 ==== %p\n", &ez_adapter->sanity_check4);
		printk("content of sanity5 ==== %p\n", ez_adapter->sanity_check5);
		printk("Address of sanity5 ==== %p\n", &ez_adapter->sanity_check5);
		printk("content of sanity6 ==== %p\n", ez_adapter->sanity_check6);
		printk("Address of sanity6 ==== %p\n", &ez_adapter->sanity_check6);
		printk("content of sanity7 ==== %p\n", ez_adapter->sanity_check7);
		printk("Address of sanity7 ==== %p\n", &ez_adapter->sanity_check7);


		printk("content of sanity ==== %p\n", ez_adapter->sanity_check);
		printk("Address of sanity ==== %p\n", &ez_adapter->sanity_check);
		printk("sizeof EZ_ADAPTER = %d\n", sizeof(EZ_ADAPTER));
		printk("sizeof ez_dev_t = %d\n", sizeof(ez_dev_t));
		printk("###################################################################\n");
#endif		
		if(!ez_adapter->default_group_data_band)
		{
			ez_adapter->default_group_data_band = ez_init_params->default_group_data_band;
		}
	return ezdev;
}
#if 0
int ez_init_hook(
	ez_init_params_t *init_params)
{
	int allocated_band= -1;
	struct _ez_security *ez_sec_info;
	ez_dev_t *ezdev;	
	int i;
	allocated_band = ez_allocate_or_update_band(&ez_adapter,init_params);
	ez_dealloc_non_ez_band(init_params);

	if (allocated_band == -1)
		return -1;

	if (init_params->ezdev_type == EZDEV_TYPE_AP) {
		ezdev = &ez_adapter->ez_band_info[allocated_band].ap_ezdev;
		COPY_MAC_ADDR(ezdev->bssid,init_params->mac_add);
	}
	else
	{
		ezdev = &ez_adapter->ez_band_info[allocated_band].cli_ezdev;
	}
	ez_sec_info = &ezdev->ez_security;
	*ezdev->channel = init_params->channel;
	COPY_MAC_ADDR(ezdev->if_addr,init_params->mac_add);

	for (i = 0; i < 10; i ++)
	{
		ez_adapter->best_ap_rssi_threshld[i] = -110;
	}
	ez_adapter->best_ap_rssi_threshld_max = 1;

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("------> %s(): ezdev_type = 0x%x\n", __FUNCTION__, ezdev->ezdev_type));
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("%s - ez_adapter->configured_status = %d\n", 
			__FUNCTION__, ez_adapter->configured_status));
	ez_hex_dump("Group ID ", ez_sec_info->group_id, ez_sec_info->group_id_len);
		ez_gen_dh_public_key(ez_sec_info);
	
	ez_sec_info->ez_max_scan_delay = EZ_MAX_SCAN_DELAY;
	ez_sec_info->ez_scan_delay = 0;

		/* 
			Generate random Self Nonce
		*/
	ezdev->driver_ops->GenRandom(ezdev, ezdev->if_addr, &ez_sec_info->self_nonce[0]);

		if (ezdev->ezdev_type == EZDEV_TYPE_AP)
		{
			
			EZ_SET_CAP_CONFIGRED(ez_sec_info->capability);
			
			if (ez_sec_info->go_internet)
				EZ_SET_CAP_INTERNET(ez_sec_info->capability);
			else
				EZ_CLEAR_CAP_INTERNET(ez_sec_info->capability);
		}

	//!Timers

	ez_sec_info->ez_loop_chk_timer = init_params->ez_loop_chk_timer;
	ez_sec_info->ez_scan_pause_timer = init_params->ez_scan_pause_timer;
	ez_sec_info->ez_scan_timer = init_params->ez_scan_timer;
	ez_sec_info->ez_stop_scan_timer = init_params->ez_stop_scan_timer;	
	ez_sec_info->ez_group_merge_timer = init_params->ez_group_merge_timer;

	ez_timer_init(ezdev, ez_sec_info->ez_loop_chk_timer, ez_loop_chk_timeout);
	ez_timer_init(ezdev, ez_sec_info->ez_scan_pause_timer, ez_scan_pause_timeout);
	ez_timer_init(ezdev, ez_sec_info->ez_group_merge_timer, ez_group_merge_timeout);
	ez_timer_init(ezdev, ez_sec_info->ez_stop_scan_timer, ez_stop_scan_timeout);
	ez_timer_init(ezdev, ez_sec_info->ez_scan_timer, ez_scan_timeout);

	ez_sec_info->first_loop_check = FALSE;
	ez_sec_info->ez_is_connection_allowed = FALSE;
}
#endif


unsigned long ez_build_beacon_ie_hook(
	ez_dev_t *ezdev,
	unsigned char *frame_buf)
{
	unsigned int capability;
	unsigned long frame_len;
	unsigned long tmp_len;
	beacon_info_tag_t beacon_info_tag;
	interface_info_t other_band_info;
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	
	frame_len = 0;
	tmp_len = 0;
	capability = cpu2be32(ezdev->ez_security.capability);

	/*
		Insert Capability TLV
	*/
	ez_insert_tlv(EZ_TAG_CAPABILITY_INFO, 
		(unsigned char *)&capability, 
		EZ_CAPABILITY_LEN, 
		frame_buf, 
		&tmp_len);
	frame_len += tmp_len;
#ifdef EZ_API_SUPPORT
	if (ezdev->ez_security.ez_api_mode != CONNECTION_OFFLOAD) {
#endif
	NdisZeroMemory(&beacon_info_tag,sizeof(beacon_info_tag_t));
	NdisCopyMemory(beacon_info_tag.network_weight, ez_ad->device_info.network_weight, NETWORK_WEIGHT_LEN);
	NdisCopyMemory(&beacon_info_tag.node_number,&ez_ad->device_info.ez_node_number,sizeof(EZ_NODE_NUMBER));
	ez_get_other_band_info(ezdev, &other_band_info);
	NdisCopyMemory(beacon_info_tag.other_ap_mac,other_band_info.shared_info.ap_mac_addr,MAC_ADDR_LEN);
	beacon_info_tag.other_ap_channel = other_band_info.shared_info.channel_info.channel;
	
	ez_insert_tlv(EZ_TAG_BEACON_INFO, 
		(unsigned char *)&beacon_info_tag, 
		sizeof(beacon_info_tag_t), 
		frame_buf+frame_len, 
		&tmp_len);
	frame_len += tmp_len;
	
	ez_insert_tlv(EZ_TAG_OPEN_GROUP_ID, 
		(unsigned char *)(ezdev->ez_security.open_group_id), 
		ezdev->ez_security.open_group_id_len, 
		frame_buf+frame_len, 
		&tmp_len);
	frame_len += tmp_len;
#ifdef EZ_API_SUPPORT
	}
#endif

	return frame_len;
}

unsigned long ez_build_probe_request_ie_hook(
	ez_dev_t *ezdev,
	unsigned char *frame_buf)
{
	struct _ez_security *ez_sec_info;
	unsigned int capability;
	unsigned long frame_len;
	unsigned long tmp_len;

	frame_len = 0;
	tmp_len = 0;
	ez_sec_info = (struct _ez_security *)&ezdev->ez_security;
	capability = cpu2be32(ezdev->ez_security.capability);

	/*
		Insert capability TLV
	*/
	ez_insert_tlv(EZ_TAG_CAPABILITY_INFO, 
		(unsigned char *)&capability, 
		EZ_CAPABILITY_LEN, 
		frame_buf + frame_len, 
		&tmp_len);
	frame_len += tmp_len;
	/*
		Insert Supplicant DH Public Key TLV
	*/
	ez_insert_tlv(EZ_TAG_SDH_PUBLIC_KEY, 
			&ez_sec_info->self_pke[0], 
			EZ_RAW_KEY_LEN, 
			frame_buf + frame_len, 
			&tmp_len);
	frame_len += tmp_len;

	return frame_len;
}

unsigned long ez_build_probe_response_ie_hook(
	ez_dev_t *ezdev,
	unsigned char *frame_buf)
{
	struct _ez_security *ez_sec_info;
	unsigned long frame_len;
	unsigned long tmp_len;	
	unsigned int capability;
	
	frame_len = 0;
	tmp_len = 0;
	ez_sec_info = (struct _ez_security *)&ezdev->ez_security;
	capability = cpu2be32(ez_sec_info->capability);
	
	/*
		Insert Authenticator DH Public Key TLV
	*/
	ez_insert_tlv(EZ_TAG_ADH_PUBLIC_KEY, 
			&ez_sec_info->self_pke[0], 
			EZ_RAW_KEY_LEN, 
			frame_buf + frame_len, 
			&tmp_len);
	frame_len += tmp_len;
	return frame_len;
}

unsigned long ez_build_auth_request_ie_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	unsigned char *frame_buf)
{
	struct _ez_security *ez_sec_info;
	unsigned long frame_len;
	unsigned long tmp_len;
	unsigned char *entrypted_data;
	unsigned int entrypted_data_len;
	struct _ez_peer_security_info *ez_peer = NULL;
	unsigned char mic[EZ_MIC_LEN];

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("%s()\n", __FUNCTION__));

	entrypted_data_len = 0;
	frame_len = 0;
	tmp_len = 0;
	ez_sec_info = &ezdev->ez_security;

	ez_peer = ez_peer_table_search_by_addr_hook(ezdev, peer_addr);
	if (ez_peer) {
		/*
			Insert Encrypted Group ID
		*/
		EZ_MEM_ALLOC(NULL, &entrypted_data, ez_sec_info->group_id_len + EZ_AES_KEY_ENCRYPTION_EXTEND);
		if (entrypted_data) {
			/* encrypt */
			ezdev->driver_ops->AES_Key_Wrap(ezdev,
						(unsigned char *)ez_sec_info->group_id, 
						ez_sec_info->group_id_len, 
						ez_peer->sw_key, LEN_PTK_KEK, 
						entrypted_data, &entrypted_data_len);
			
			ez_insert_tlv(EZ_TAG_GROUP_ID, 
				entrypted_data, 
				entrypted_data_len, 
				frame_buf + frame_len, 
				&tmp_len);
			frame_len += tmp_len;
			
			EZ_MEM_FREE( entrypted_data);
		}
		ez_insert_tlv(EZ_TAG_OPEN_GROUP_ID, 
						ez_sec_info->open_group_id, 
						ez_sec_info->open_group_id_len, 
						frame_buf + frame_len, 
						&tmp_len);
					frame_len += tmp_len;


		/*
			Insert Supplicant Nonce TLV
		*/
		ez_insert_tlv(EZ_TAG_SNONCE, &ez_sec_info->self_nonce[0], EZ_NONCE_LEN, frame_buf+frame_len, &tmp_len);
		frame_len += tmp_len;

		/*
			Insert MIC for validation
		*/
		ez_calculate_mic(ezdev, ez_peer->sw_key, frame_buf, frame_len, &mic[0]);		
		ez_insert_tlv(EZ_TAG_MIC, &mic[0], EZ_MIC_LEN, frame_buf+frame_len, &tmp_len);
		frame_len += tmp_len;
	}
	else {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
					("%s(): cannot find ez_peer\n", __FUNCTION__));
	}
	return frame_len;
}

unsigned long ez_build_auth_response_ie_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	unsigned char *frame_buf)
{
	struct _ez_security *ez_sec_info;
	unsigned long frame_len;
	unsigned long tmp_len;
	unsigned char *entrypted_gmk;
	unsigned int entrypted_gmk_len;
	unsigned char *entrypted_data;
	unsigned int entrypted_data_len;
	unsigned char ez_psk_len;
	unsigned char mic[EZ_MIC_LEN];
	struct _ez_peer_security_info *ez_peer = NULL;
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("%s()\n", __FUNCTION__));
	ez_peer = ez_peer_table_search_by_addr_hook(ezdev, peer_addr);
	if (ez_peer) {

	} else {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("%s(): cannot find ez_peer\n", __FUNCTION__));
		return 0;
	}

	frame_len = 0;
	tmp_len = 0;
	ez_sec_info = (struct _ez_security *)&ezdev->ez_security;

	/*
		Insert Encrypted Group ID
	*/
	EZ_MEM_ALLOC(NULL, &entrypted_data, ez_sec_info->group_id_len + EZ_AES_KEY_ENCRYPTION_EXTEND);	
	if (entrypted_data) {
		/* encrypt */
		ezdev->driver_ops->AES_Key_Wrap(ezdev,
					(unsigned char *)ez_sec_info->group_id, ez_sec_info->group_id_len, 
					ez_peer->sw_key, LEN_PTK_KEK, 
					entrypted_data, &entrypted_data_len);
		
		ez_insert_tlv(EZ_TAG_GROUP_ID, 
			entrypted_data, 
			entrypted_data_len, 
			frame_buf + frame_len, 
			&tmp_len);
		frame_len += tmp_len;
		
		EZ_MEM_FREE( entrypted_data);
	}
			ez_insert_tlv(EZ_TAG_OPEN_GROUP_ID, 
							ez_sec_info->open_group_id, 
							ez_sec_info->open_group_id_len, 
							frame_buf + frame_len, 
							&tmp_len);
						frame_len += tmp_len;
	

	/*
		Insert Encrypted PMK if need.
	*/
		{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO, 
					("%s() - EZ configured status is configured. Insert encrypted PMK.\n", 
					__FUNCTION__));
		EZ_MEM_ALLOC(NULL, &entrypted_gmk, LEN_PMK + EZ_AES_KEY_ENCRYPTION_EXTEND);	
		if (entrypted_gmk) {
			//ez_hex_dump("AP PMK", &ezdev->ez_security.this_band_info.pmk[0], LEN_PMK);
			/* encrypt */
			ezdev->driver_ops->AES_Key_Wrap(ezdev,
						&ezdev->ez_security.this_band_info.pmk[0], LEN_PMK, 
						ez_peer->sw_key, LEN_PTK_KEK, 
						entrypted_gmk, &entrypted_gmk_len);
			
			ez_insert_tlv(EZ_TAG_PMK, 
				entrypted_gmk, 
				entrypted_gmk_len, 
				frame_buf + frame_len, 
				&tmp_len);
			frame_len += tmp_len;
			
			EZ_MEM_FREE( entrypted_gmk);
		}
	}

	ez_psk_len = strlen(ezdev->ez_security.this_band_info.psk);
	
	ez_insert_tlv(EZ_TAG_PSK_LEN, 
		&ez_psk_len,
		1, 
		frame_buf + frame_len, 
		&tmp_len);
	frame_len += tmp_len;

	/*insert encrypted psk */
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO, 
					("%s() - EZ configured status is configured. Insert encrypted PSK.\n", 
					__FUNCTION__));
		if (ez_psk_len % AES_KEYWRAP_BLOCK_SIZE !=0) {
			ez_psk_len += AES_KEYWRAP_BLOCK_SIZE - (ez_psk_len % AES_KEYWRAP_BLOCK_SIZE);
		}
		
		EZ_MEM_ALLOC(NULL, &entrypted_gmk, ez_psk_len + EZ_AES_KEY_ENCRYPTION_EXTEND);
		if (entrypted_gmk) {
			ezdev->driver_ops->AES_Key_Wrap(ezdev,
				&ezdev->ez_security.this_band_info.psk[0], ez_psk_len, 
				ez_peer->sw_key, LEN_PTK_KEK, 
				entrypted_gmk, &entrypted_gmk_len);
			ez_insert_tlv(EZ_TAG_PSK, 
				entrypted_gmk, 
				entrypted_gmk_len, 
				frame_buf + frame_len, 
				&tmp_len);
			frame_len += tmp_len;
			EZ_MEM_FREE( entrypted_gmk);
		}
	}
	
	/*
		Insert Authenticator Nonce TLV
	*/
	ez_insert_tlv(EZ_TAG_ANONCE, &ez_sec_info->self_nonce[0], EZ_NONCE_LEN, frame_buf + frame_len, &tmp_len);
	frame_len += tmp_len;

	/*
		Insert MIC for validation
	*/
	NdisZeroMemory(&mic[0], EZ_MIC_LEN);
	ez_calculate_mic(ezdev, ez_peer->sw_key, frame_buf, frame_len, &mic[0]);
	ez_insert_tlv(EZ_TAG_MIC, &mic[0], EZ_MIC_LEN, frame_buf+frame_len, &tmp_len);
	frame_len += tmp_len;

	return frame_len;
}


unsigned long ez_build_assoc_request_ie_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	unsigned char *frame_buf,
	unsigned int frame_buf_len)
{

	unsigned long frame_len;
	unsigned long tmp_len;
	unsigned char mic[EZ_MIC_LEN];
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	struct _ez_peer_security_info *ez_peer = NULL;

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("%s()\n", __FUNCTION__));

	frame_len = 0;
	tmp_len = 0;

	ez_peer = ez_peer_table_search_by_addr_hook(ezdev, peer_addr);
	if (ez_peer) {
		/*
			Insert MIC for validation
		*/
		NdisZeroMemory(&mic[0], EZ_MIC_LEN);
		/*
			Because driver cannot know the vaule of duration and sequence now,
			exclude 802.11 header to calculate mic data.
		*/
		ez_calculate_mic(ezdev, ez_peer->sw_key, (frame_buf-frame_buf_len+LENGTH_802_11), frame_buf_len-LENGTH_802_11, &mic[0]);
		ez_insert_tlv(EZ_TAG_MIC, 
			(unsigned char *)&mic[0], 
			EZ_MIC_LEN, 
			frame_buf, 
			&tmp_len);
		frame_len += tmp_len;
		{
			unsigned int capability;
			capability = be2cpu32(ezdev->ez_security.capability);
			ez_insert_tlv(EZ_TAG_CAPABILITY_INFO, 
				(unsigned char *)&capability, 
				EZ_CAPABILITY_LEN, 
				frame_buf + frame_len, 
				&tmp_len);
			frame_len += tmp_len;
		}

#ifdef EZ_API_SUPPORT
		if (ezdev->ez_security.ez_api_mode != CONNECTION_OFFLOAD) {
#endif
{
		interface_info_tag_t shared_info[2];
		interface_info_t other_band_info;
		NdisCopyMemory(&shared_info[0],&ezdev->ez_security.this_band_info.shared_info,sizeof(interface_info_tag_t));
		if (ez_get_other_band_info(ezdev, &other_band_info)){
			NdisCopyMemory(&shared_info[1],&other_band_info.shared_info,sizeof(interface_info_tag_t));
		} else {
			NdisZeroMemory(&shared_info[1], sizeof(interface_info_tag_t));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("other band interface not activated\n"));
		}
		//ez_hex_dump("peer_addr", peer_addr, MAC_ADDR_LEN);
		//ez_hex_dump("other_band_info.cli_peer_ap_mac", other_band_info.cli_peer_ap_mac, MAC_ADDR_LEN);

		if (ez_apcli_is_link_duplicate(ezdev,peer_addr) == TRUE)
		{
			shared_info[0].link_duplicate = TRUE;
			shared_info[1].link_duplicate = TRUE;
		}
		ez_insert_tlv(EZ_TAG_INTERFACE_INFO, 
			(unsigned char *)&shared_info[0], 
			sizeof(shared_info), 
			frame_buf + frame_len, 
			&tmp_len);
		frame_len += tmp_len;
}
		ez_insert_tlv(EZ_TAG_NETWORK_WEIGHT, 
			(unsigned char *)(ez_ad->device_info.network_weight), 
			NETWORK_WEIGHT_LEN, 
			frame_buf+frame_len, 
			&tmp_len);
		frame_len += tmp_len;
#ifdef EZ_API_SUPPORT
		}
#endif
	}

	return frame_len;
}


unsigned long ez_build_assoc_response_ie_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	unsigned char *ap_gtk,
	unsigned int ap_gtk_len,
	unsigned char *frame_buf)
{
	struct _ez_security *ez_sec_info;
	struct _ez_peer_security_info *ez_peer = NULL;
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	unsigned long frame_len;
	unsigned long tmp_len;
	unsigned char mic[EZ_MIC_LEN];
	unsigned char *entrypted_gtk;
	unsigned int entrypted_gtk_len;
	unsigned char *other_band_pmk;
	unsigned char *other_band_psk;
	unsigned int encrypted_pmk_len;
	unsigned int encrypted_psk_len;
	NON_EZ_BAND_INFO_TAG non_ez_info_tag[MAX_NON_EZ_BANDS];
	NON_EZ_BAND_PSK_INFO_TAG non_ez_info_psk_tag[MAX_NON_EZ_BANDS];
	unsigned char other_band_ez_psk_len;
	//! Levarage from MP1.0 CL #170037
	NON_MAN_INFO_TAG non_man_info_tag;

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("%s()\n", __FUNCTION__));

	frame_len = 0;
	tmp_len = 0;
	ez_sec_info = (struct _ez_security *)&ezdev->ez_security;
	ez_peer = ez_peer_table_search_by_addr_hook(ezdev, peer_addr);
	ez_allocate_node_number_sta(ez_peer,FALSE);
	if (ez_peer == NULL)
	{
		ASSERT(FALSE);
		return 0;
	}
	EZ_MEM_ALLOC(NULL, &entrypted_gtk, ap_gtk_len + EZ_AES_KEY_ENCRYPTION_EXTEND);	
	if (entrypted_gtk && ez_peer) {
		NdisZeroMemory(entrypted_gtk, ap_gtk_len + EZ_AES_KEY_ENCRYPTION_EXTEND);

		//ez_hex_dump("AP GTK", ap_gtk, ap_gtk_len);
		/* encrypt */
		ezdev->driver_ops->AES_Key_Wrap(ezdev,
					ap_gtk, ap_gtk_len, ez_peer->sw_key, LEN_PTK_KEK, 
					 entrypted_gtk, &entrypted_gtk_len);
			
		/*
			Insert encrypted GTK.
		*/
		ez_insert_tlv(EZ_TAG_GTK, 
			entrypted_gtk, 
			entrypted_gtk_len, 
			frame_buf, 
			&tmp_len);
		frame_len += tmp_len;
			{
				UINT8 *entrypted_seed = NULL;
				UINT32 entrypted_seed_len = 0;
				if(ez_sec_info->gen_group_id)
				{
					EZ_MEM_ALLOC(NULL, &entrypted_seed,ez_sec_info->gen_group_id_len  + EZ_AES_KEY_ENCRYPTION_EXTEND);
					if(entrypted_seed)
					{
						NdisZeroMemory(entrypted_seed,ez_sec_info->gen_group_id_len  + EZ_AES_KEY_ENCRYPTION_EXTEND);
						ezdev->driver_ops->AES_Key_Wrap(ezdev,
									ez_sec_info->gen_group_id,ez_sec_info->gen_group_id_len, ez_peer->sw_key, LEN_PTK_KEK, 
									 entrypted_seed, &entrypted_seed_len);
							
						/*
							Insert encrypted Generated Group ID Seed.
						*/
						ez_insert_tlv(EZ_TAG_GROUPID_SEED, 
							entrypted_seed, 
							entrypted_seed_len, 
							frame_buf+ frame_len, 
							&tmp_len);
						frame_len += tmp_len;
						EZ_MEM_FREE( entrypted_seed);
					}
				}
				else
				{
					ez_insert_tlv(EZ_TAG_GROUPID_SEED, 
						entrypted_seed, 
						entrypted_seed_len, 
						frame_buf+ frame_len, 
						&tmp_len);
					frame_len += tmp_len;
				}
			}
#ifdef EZ_API_SUPPORT
				if (ezdev->ez_security.ez_api_mode != CONNECTION_OFFLOAD) {
#endif

		EZ_MEM_ALLOC(NULL, &other_band_pmk, EZ_PMK_LEN + EZ_AES_KEY_ENCRYPTION_EXTEND);
			if (other_band_pmk) {
				interface_info_t other_band_info;
				if (ez_get_other_band_info(ezdev, &other_band_info)) {
					
					/* encrypt */
					ezdev->driver_ops->AES_Key_Wrap(ezdev,
								other_band_info.pmk, EZ_PMK_LEN, ez_peer->sw_key, LEN_PTK_KEK, 
								 other_band_pmk, &encrypted_pmk_len);
					} else {
					NdisZeroMemory(other_band_pmk, EZ_PMK_LEN + EZ_AES_KEY_ENCRYPTION_EXTEND);
					if(ez_ad->band_count == 1)
					{
						NdisCopyMemory(&other_band_info.pmk,&ezdev->ez_security.other_band_info_backup.pmk,EZ_PMK_LEN);
						ezdev->driver_ops->AES_Key_Wrap(ezdev,
							other_band_info.pmk, EZ_PMK_LEN, ez_peer->sw_key, LEN_PTK_KEK, 
							 other_band_pmk, &encrypted_pmk_len);
					}	
					encrypted_pmk_len = EZ_PMK_LEN + EZ_AES_KEY_ENCRYPTION_EXTEND;
				}
				ez_insert_tlv(EZ_TAG_OTHER_BAND_PMK, 
					(unsigned char *)other_band_pmk, 
					encrypted_pmk_len, 
					frame_buf + frame_len, 
					&tmp_len);
				frame_len += tmp_len;
				EZ_MEM_FREE( other_band_pmk);
			} else {
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
					("%s() - Allocate encryption buffer fail!\n", 
					__FUNCTION__));
				
				EZ_MEM_FREE( entrypted_gtk);
				return 0;
			}
			
	{
		EZ_MEM_ALLOC(NULL, &other_band_psk, EZ_LEN_PSK + EZ_AES_KEY_ENCRYPTION_EXTEND);
			if (other_band_psk) {
				interface_info_t other_band_info;
				unsigned char length_with_padding;
				if (ez_get_other_band_info(ezdev, &other_band_info)) {
					other_band_ez_psk_len = strlen(other_band_info.psk);
					length_with_padding = other_band_ez_psk_len;
					if (length_with_padding % AES_KEYWRAP_BLOCK_SIZE !=0) {
						length_with_padding += AES_KEYWRAP_BLOCK_SIZE - (length_with_padding % AES_KEYWRAP_BLOCK_SIZE);
					}
					/* encrypt */
					ezdev->driver_ops->AES_Key_Wrap(ezdev,
						other_band_info.psk, length_with_padding, ez_peer->sw_key, LEN_PTK_KEK, 
						other_band_psk, &encrypted_psk_len);
					} else {
					NdisZeroMemory(other_band_psk, EZ_LEN_PSK + EZ_AES_KEY_ENCRYPTION_EXTEND);
					if(ez_ad->band_count == 1)
					{
						other_band_ez_psk_len = strlen(ezdev->ez_security.other_band_info_backup.psk);
						length_with_padding = other_band_ez_psk_len;
						if (length_with_padding % AES_KEYWRAP_BLOCK_SIZE !=0) {
							length_with_padding += AES_KEYWRAP_BLOCK_SIZE - (length_with_padding % AES_KEYWRAP_BLOCK_SIZE);
						}
						NdisCopyMemory(&other_band_info.psk,&ezdev->ez_security.other_band_info_backup.psk, other_band_ez_psk_len);
						ezdev->driver_ops->AES_Key_Wrap(ezdev,
							other_band_info.psk, length_with_padding, ez_peer->sw_key, LEN_PTK_KEK, 
							other_band_psk, &encrypted_psk_len);
					}	
					encrypted_psk_len = length_with_padding + EZ_AES_KEY_ENCRYPTION_EXTEND;
				}
				ez_insert_tlv(EZ_TAG_OTHER_BAND_PSK_LEN, 
					&other_band_ez_psk_len, 
					1, 
					frame_buf + frame_len, 
					&tmp_len);
				frame_len += tmp_len;
				ez_insert_tlv(EZ_TAG_OTHER_BAND_PSK, 
					(unsigned char *)other_band_psk, 
					encrypted_psk_len, 
					frame_buf + frame_len, 
					&tmp_len);
				frame_len += tmp_len;
				
				EZ_MEM_FREE(other_band_psk);
			} else {
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
					("%s() - Allocate encryption buffer fail!\n", 
					__FUNCTION__));
				
				EZ_MEM_FREE( entrypted_gtk);
				return 0;
			}
		}
	
#ifdef EZ_API_SUPPORT
					}
#endif
		/*
			Insert MIC TLV
		*/
		NdisZeroMemory(&mic[0], EZ_MIC_LEN);
		ez_calculate_mic(ezdev, ez_peer->sw_key, frame_buf, frame_len, &mic[0]);
		ez_insert_tlv(EZ_TAG_MIC, 
			(unsigned char *)&mic[0], 
			EZ_MIC_LEN, 
			frame_buf + frame_len, 
			&tmp_len);
		frame_len += tmp_len;
#ifdef EZ_API_SUPPORT
	if (ezdev->ez_security.ez_api_mode != CONNECTION_OFFLOAD) {
#endif		
	{
		device_info_t device_info;
		NdisCopyMemory(&device_info ,&ez_ad->device_info, sizeof(device_info_t));
		NdisCopyMemory(&device_info.ez_node_number ,&ez_peer->device_info.ez_node_number, sizeof(EZ_NODE_NUMBER));
		ez_insert_tlv(EZ_TAG_DEVICE_INFO, 
				(unsigned char *)(&device_info), 
				sizeof(device_info_t), 
				frame_buf+frame_len, 
				&tmp_len);
			frame_len += tmp_len;
	}

	{
		interface_info_tag_t shared_info[2];
		interface_info_t other_band_info;
		NdisZeroMemory(shared_info,sizeof(shared_info));
		NdisCopyMemory(&shared_info[0],&ezdev->ez_security.this_band_info.shared_info,sizeof(interface_info_tag_t));
		if (ez_get_other_band_info(ezdev, &other_band_info)) {
			NdisCopyMemory(&shared_info[1],&other_band_info.shared_info,sizeof(interface_info_tag_t));
		} else {
			NdisZeroMemory(&shared_info[1], sizeof(interface_info_tag_t));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("otherband interface not activated\n"));
			if (ezdev->ez_security.other_band_info_backup.interface_activated)
			{
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("--->will use backup\n"));
				shared_info[1].ssid_len = ezdev->ez_security.other_band_info_backup.shared_info.ssid_len;
				NdisCopyMemory(shared_info[1].ssid, ezdev->ez_security.other_band_info_backup.shared_info.ssid, ezdev->ez_security.other_band_info_backup.shared_info.ssid_len);
				NdisCopyMemory(&shared_info[1].channel_info, &ezdev->ez_security.other_band_info_backup.shared_info.channel_info, sizeof(channel_info_t));
#ifdef DOT11R_FT_SUPPORT
				FT_SET_MDID(shared_info[1].FtMdId,ezdev->ez_security.other_band_info_backup.shared_info.FtMdId);
#endif
			}
		}
 		ez_insert_tlv(EZ_TAG_INTERFACE_INFO, 
			(unsigned char *)&shared_info[0], 
			sizeof(shared_info), 
			frame_buf + frame_len, 
			&tmp_len);
		frame_len += tmp_len;
	}
#ifdef EZ_API_SUPPORT
	}
#endif


		EZ_MEM_FREE( entrypted_gtk);
		if (ez_is_triband_hook()){
			NdisZeroMemory(non_ez_info_tag, sizeof(non_ez_info_tag));
			NdisZeroMemory(non_ez_info_psk_tag, sizeof(non_ez_info_psk_tag));
			ez_prepare_non_ez_tag(&non_ez_info_tag[0],&non_ez_info_psk_tag[0], ez_peer);
			ez_insert_tlv(EZ_TAG_NON_EZ_CONFIG, 
				(unsigned char *)&non_ez_info_tag[0], 
				sizeof(non_ez_info_tag), 
				frame_buf + frame_len, 
				&tmp_len);
			frame_len += tmp_len;

			
			ez_insert_tlv(EZ_TAG_NON_EZ_PSK, 
				(unsigned char *)&non_ez_info_psk_tag[0], 
				sizeof(non_ez_info_psk_tag), 
				frame_buf + frame_len, 
				&tmp_len);
			frame_len += tmp_len;
		} else if(ez_ad->is_man_nonman) {
//! Levarage from MP1.0 CL#170037
			ez_prepare_non_man_tag(&non_man_info_tag, ez_peer);

			ez_insert_tlv(EZ_TAG_NON_MAN_CONFIG, 
				(unsigned char *)&non_man_info_tag, 
				sizeof(non_man_info_tag), 
				frame_buf + frame_len, 
				&tmp_len);
			frame_len += tmp_len;

		}

		return frame_len;


	}
	else {
		if (!entrypted_gtk) {
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
					("%s() - Allocate encryption buffer fail!\n", 
					__FUNCTION__));
		}
		if (!ez_peer) {
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
					("%s() - Cannot find information of easy setup peer!\n", 
					__FUNCTION__));
		}
		return 0;
	}
}

unsigned char ez_process_probe_request_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	void *msg,
	unsigned long msg_len)
{

	struct _ez_security *ez_sec_info;
	struct _ez_peer_security_info *ez_peer;
	unsigned short status;
	FRAME_802_11 *Fr;
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	int irq_flags;
	Fr = (PFRAME_802_11)msg;
	//ezdev not started yet.
	if(ezdev->driver_ops == NULL)
		return FALSE;
	/*
		Only process unicast probe request.
	*/
	if (!NdisEqualMemory(ezdev->if_addr, Fr->Hdr.Addr1, MAC_ADDR_LEN)) {
		return FALSE;
	}

	if (ez_is_roaming_ongoing_hook(ezdev->ez_ad))
	{
		return 2;
	}
	ez_peer = ez_peer_table_insert(ezdev, peer_addr);

	status = FALSE;
	EZ_IRQ_LOCK(ezdev->ez_peer_table_lock, irq_flags);
	if (ez_peer) {
		ez_sec_info = &ezdev->ez_security;
		status = ez_probe_request_sanity(msg, msg_len, ez_peer);
	}
	else
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("%s:EzPeer is NULL!!!\n", __FUNCTION__));
	}
	EZ_IRQ_UNLOCK(ezdev->ez_peer_table_lock, irq_flags);
	if (status == EZ_STATUS_CODE_SUCCESS) {

		ez_dev_t *apcli_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev;
		/*
			Peer found me and start to do easy setup.
			Stop apcli auto connect to prevent easy setup handshaking failed.
		*/
		if (apcli_ezdev->wdev){
			if (apcli_ezdev->driver_ops->ez_is_timer_running(apcli_ezdev, apcli_ezdev->ez_security.ez_scan_timer)
				&& apcli_ezdev->ez_security.this_band_info.interface_activated){
				apcli_ezdev->driver_ops->ez_cancel_timer(apcli_ezdev, apcli_ezdev->ez_security.ez_scan_timer);
			}

			if (apcli_ezdev->wdev &&
				apcli_ezdev->ez_security.keep_finding_provider
				&& apcli_ezdev->ez_security.this_band_info.interface_activated) {

				EZ_IRQ_LOCK(&apcli_ezdev->ez_security.ez_scan_pause_timer_lock, irq_flags);

				
				apcli_ezdev->driver_ops->apcli_stop_auto_connect(apcli_ezdev, TRUE);

				if (!apcli_ezdev->driver_ops->ez_is_timer_running(apcli_ezdev, apcli_ezdev->ez_security.ez_scan_pause_timer) ){
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
						("(%s) Set scan pause timer(time = %d). \n", __FUNCTION__, EZ_PAUSE_SCAN_TIME_OUT));
					apcli_ezdev->driver_ops->ez_set_timer(apcli_ezdev, apcli_ezdev->ez_security.ez_scan_pause_timer, EZ_PAUSE_SCAN_TIME_OUT);
				}
				else{// timer already set, imples another ez peer apcli also attempting ap, set the timer again
					apcli_ezdev->driver_ops->ez_cancel_timer(apcli_ezdev,apcli_ezdev->ez_security.ez_scan_pause_timer);

					EZ_IRQ_UNLOCK(&apcli_ezdev->ez_security.ez_scan_pause_timer_lock, irq_flags);
	                // In case timer got fired, release & retake lock to avoid the timeout handler reset state
					EZ_IRQ_LOCK(&apcli_ezdev->ez_security.ez_scan_pause_timer_lock, irq_flags)

					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
						("(%s) Set scan pause timer(time = %d). \n", __FUNCTION__, EZ_PAUSE_SCAN_TIME_OUT));
					apcli_ezdev->driver_ops->ez_set_timer(apcli_ezdev, 
						apcli_ezdev->ez_security.ez_scan_pause_timer, 
						EZ_PAUSE_SCAN_TIME_OUT);
				}

	    		EZ_IRQ_UNLOCK(&apcli_ezdev->ez_security.ez_scan_pause_timer_lock, irq_flags);

			}
		}

		if (ez_is_connection_allowed_hook(ezdev))
			return 1;
		else 
			return 2;
	}
	else {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("ProbeReq sanity failed\n"));
		ez_update_connection_hook(ezdev);
		ez_peer_table_delete(ezdev, peer_addr);
		return 0;
	}
}

void ez_process_beacon_probe_response_hook(
	ez_dev_t *ezdev,
	void *msg,
	unsigned long msg_len)
{
	FRAME_802_11 *Fr;
	struct _ez_peer_security_info *ez_peer = NULL;
	USHORT Status;
	Fr = (PFRAME_802_11)msg;

	ez_peer = ez_peer_table_insert(ezdev, Fr->Hdr.Addr2);
	
	if (ez_peer) {
		Status = ez_probe_beacon_response_sanity(msg, 
					msg_len, ez_peer);
		if (Status)
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("beacon_rsp sanity failed\n"));
	} 
	else
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("%s: ezpeer is null\n", __FUNCTION__));
	}
}

unsigned char ez_process_auth_request_hook(
	ez_dev_t *ezdev,
	void *auth_info_obj,
	void *msg,
	unsigned long msg_len)
{
	struct _ez_security *ez_sec_info = &ezdev->ez_security;
	AUTH_FRAME_INFO *auth_info;
	HEADER_802_11_EZ auth_hdr;
	unsigned short reason_code;
	unsigned char *pOutBuffer;
	unsigned long FrameLen;
	struct _ez_peer_security_info *ez_peer = NULL;

	if (ez_is_connection_allowed_hook(ezdev) == FALSE)
		return FALSE;

	EZ_MEM_ALLOC(NULL, &pOutBuffer, 1024);
	if (pOutBuffer == NULL) {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
				("%s() - no memory, cannot send auth response.\n", 
				__FUNCTION__));
		return FALSE;
	}
	
	auth_info = (AUTH_FRAME_INFO *)auth_info_obj;
	
	reason_code = 0;
	FrameLen = 0;
	
	ez_peer = ez_peer_table_search_by_addr_hook(ezdev, auth_info->addr2);
	if (ez_peer) {
		reason_code = ez_auth_request_sanity(msg, msg_len, ez_peer);

		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("peer_capabilities :%x\n",ez_peer->capability));
		//! if addr2 mathches the address provided by host, bypass group ID check	
		if (NdisEqualMemory(ez_sec_info->merge_peer_addr,auth_info->addr2,MAC_ADDR_LEN)){
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
					("%s(): different group id, Group Merge going on, allow Connection.\n",
					__FUNCTION__));
			
		}
		//! or if peer supports group merge and host has not given a specific address for merge, bypass group ID check
		else if (EZ_GET_CAP_ALLOW_MERGE(ez_peer->capability)
			&& MAC_ADDR_EQUAL(ez_sec_info->merge_peer_addr, BROADCAST_ADDR)){
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
					("%s(): allow Connection as both sides ignore group ID.\n",
					__FUNCTION__));
			
		}
		//! if above two checks fail, compare group ID 
		else {
		
		if ((reason_code == EZ_STATUS_CODE_SUCCESS) &&
				((ez_peer->group_id_len != ez_sec_info->group_id_len)||
			 	(!NdisEqualMemory(ez_sec_info->group_id, ez_peer->group_id, ez_peer->group_id_len)))) {
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
					("%s(): different group id, reject this auth request.\n",
					__FUNCTION__));
				ez_hex_dump("My Group ID", ez_sec_info->group_id, ez_sec_info->group_id_len);
				ez_hex_dump("Peer Group ID", ez_peer->group_id, ez_peer->group_id_len);
				reason_code = EZ_STATUS_CODE_INVALID_DATA;
			}
		}
	}
	else
		reason_code = EZ_STATUS_CODE_NO_RESOURCE;

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
		("AUTH - Send AUTH response (alg = 0x%x, reason = 0x%x)\n", 
		auth_info->auth_alg, reason_code));
	
	ezdev->driver_ops->MgtMacHeaderInit(ezdev, &auth_hdr, SUBTYPE_AUTH,
		0, auth_info->addr2,ezdev->if_addr, ezdev->bssid);

	auth_info->auth_alg = cpu2le16(auth_info->auth_alg);
	auth_info->auth_seq++;
	EzMakeOutgoingFrame(pOutBuffer, &FrameLen,
			sizeof(HEADER_802_11),	&auth_hdr,
			2,			&auth_info->auth_alg,
			2,			&auth_info->auth_seq,
			2,			&reason_code,
			END_OF_ARGS);

	if (reason_code == EZ_STATUS_CODE_SUCCESS) {
		FrameLen += ez_build_auth_response_ie_hook(ezdev, ez_peer->mac_addr, pOutBuffer+FrameLen);
	}

	ezdev->driver_ops->MiniportMMRequest(ezdev,pOutBuffer, FrameLen, FALSE);
	EZ_MEM_FREE(pOutBuffer);

	if (reason_code == EZ_STATUS_CODE_SUCCESS) { // Scan is already paused. 

		return TRUE;
	}
	else
		return FALSE;
}

USHORT ez_process_auth_response_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	void *msg,
	unsigned long msg_len)
{
	USHORT Status;
	struct _ez_peer_security_info *ez_peer = NULL;


	ez_peer = ez_peer_table_search_by_addr_hook(ezdev, peer_addr);
	if(ez_peer == NULL)
		return FALSE;

	NdisZeroMemory(&ez_peer->this_band_info.pmk[0], EZ_PMK_LEN);
	NdisCopyMemory(&ez_peer->this_band_info.pmk[0], &ez_peer->dh_key[0], EZ_PMK_LEN);
	if (ez_peer && 
		ez_auth_response_sanity(msg, msg_len, ez_peer)
			== EZ_STATUS_CODE_SUCCESS) {
			//! if peer addr matches addr by host, bypass group ID check			
			if (NdisEqualMemory(ezdev->ez_security.merge_peer_addr,peer_addr,MAC_ADDR_LEN)){
				Status = MLME_SUCCESS;
			}
			//! or if peer supports open merge, by pass groupID check
			else if (EZ_GET_CAP_ALLOW_MERGE(ezdev->ez_security.capability) && EZ_GET_CAP_ALLOW_MERGE(ez_peer->capability)){
					Status = MLME_SUCCESS;
			} else {
				if ((ezdev->ez_security.group_id_len == ez_peer->group_id_len)
					&& NdisEqualMemory(ezdev->ez_security.group_id, ez_peer->group_id, ez_peer->group_id_len))
						Status = MLME_SUCCESS;
				else {
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
						("%s(): different group id, reject this auth response.\n",
						__FUNCTION__));
					ez_hex_dump("My Group ID", ezdev->ez_security.group_id, ezdev->ez_security.group_id_len);
					ez_hex_dump("Peer Group ID", ez_peer->group_id, ez_peer->group_id_len);
					Status = MLME_INVALID_FORMAT;
					}
				}
			}
	else {
		Status = MLME_FAIL_NO_RESOURCE;
	}
	return Status;
}

unsigned short ez_process_assoc_request_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	UCHAR *easy_setup_mic_valid,
	unsigned char isReassoc,
	void *msg,
	unsigned long msg_len)
{
	struct _ez_peer_security_info *ez_peer = NULL;
	EZ_ADAPTER *ez_ad  = ezdev->ez_ad;
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
		("------> %s\n", 
		__FUNCTION__));
	
	*easy_setup_mic_valid = FALSE;
	ez_peer = ez_peer_table_search_by_addr_hook(ezdev, peer_addr);
	if (ez_peer) {
		if (ez_assoc_request_sanity(isReassoc, msg, msg_len, ez_peer)
				== EZ_STATUS_CODE_SUCCESS) {
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("is duplicate link: %d\n",
																ez_peer->this_band_info.shared_info.link_duplicate));
				if (ez_is_weight_same_mod(ez_ad->device_info.network_weight,ez_peer->device_info.network_weight)					
					&& !ez_peer->this_band_info.shared_info.link_duplicate
					)
					{
						return EZ_STATUS_CODE_LOOP;
					}
				if (ez_peer->this_band_info.shared_info.link_duplicate)
				{
					struct _ez_peer_security_info *other_band_cli_peer = ez_get_other_band_ez_peer(ezdev, ez_peer);
					if (other_band_cli_peer)
					{
					} else {		
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("Invalid Duplicate connection\n"));
						return EZ_STATUS_CODE_LOOP;
					}
				}
			*easy_setup_mic_valid = TRUE;
			return EZ_STATUS_CODE_SUCCESS;
		}
		else
			return EZ_STATUS_CODE_INVALID_DATA;
	}
	else
	{
		return EZ_STATUS_CODE_UNKNOWN;
	}

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
		("<------ %s\n", 
		__FUNCTION__));
}

unsigned short ez_process_assoc_response_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	void *msg,
	unsigned long msg_len)
{
	struct _ez_peer_security_info *ez_peer;

	ez_peer = ez_peer_table_search_by_addr_hook(ezdev, peer_addr);
	if (ez_peer) {
		return ez_assoc_response_sanity(msg, msg_len, ez_peer);
	}
	else
		return EZ_STATUS_CODE_UNKNOWN;
}


void ez_show_information_hook(
	ez_dev_t *ezdev)
{
	int j;
	struct _ez_security *ez_sec_info;
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;

	if (ezdev) {
		ez_sec_info = &ezdev->ez_security;
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("        -------------------------------        \n"));
		//if (ezdev->enable_easy_setup)
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Own Capability	 = 0x%04x\n", ez_sec_info->capability));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Own Group ID	 = "));
			for (j=0; j<ez_sec_info->group_id_len; j++) {
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%02x", ez_sec_info->group_id[j]));
			}
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Own DH PublicKey = "));
			for (j=0; j<EZ_RAW_KEY_LEN; j++) {
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%02x", ez_sec_info->self_pke[j]));
			}
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Own Nonce = "));
			for (j=0; j<EZ_NONCE_LEN; j++) {
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%02x", ez_sec_info->self_nonce[j]));
			}
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
#if 0
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("WDEV PMK  = "));
			for (j=0; j<LEN_PMK; j++) {
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%02x", ezdev->ez_security.this_band_info.pmk[j]));
			}
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("WDEV GTK  = "));
			for (j=0; j<LEN_MAX_GTK; j++) {
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%02x", ezdev->SecConfig.GTK[j]));
			}
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("WDEV PSK  = %s\n", ezdev->SecConfig.PSK));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
#endif
			
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("EZ Band Index  = %d\n", ezdev->ez_band_idx));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));

			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("EZ Band Count = %d\n", ez_ad->band_count));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));

			ez_show_peer_table_info(ezdev);
		}
	}
	else {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("%s - ezdev is NULL.\n", __FUNCTION__));
	}

	{
		for (j=0; j < EZDEV_NUM_MAX; j++)
		{
			ez_dev_t *ezdev = ez_ad->ezdev_list[j];
			if (ezdev && ezdev->wdev)
			{
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("		------------------------------- 	   \n"));
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Info for ezdev %d\n", ezdev->ez_band_idx));
				ez_show_interface_info(ezdev);
				ez_show_device_info(ez_ad->device_info);
			}
		}
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("        -------------------------------        \n"));
	}
}


INT ez_send_broadcast_deauth_proc_hook(ez_dev_t *ezdev) 
{ 
#if 1
	EZ_ADAPTER *ez_ad  = ezdev->ez_ad;
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%s()\n", __FUNCTION__));
	if (ez_is_triband_hook())
	{	
		ez_ad->non_ez_band_info[0].lut_driver_ops.ez_send_broadcast_deauth(ez_ad->non_ez_band_info[0].pAd, ez_ad->non_ez_band_info[0].non_ez_ap_wdev); 
		ez_ad->non_ez_band_info[1].lut_driver_ops.ez_send_broadcast_deauth(ez_ad->non_ez_band_info[1].pAd, ez_ad->non_ez_band_info[1].non_ez_ap_wdev); 
	} else 
	{
		if (ez_ad->ez_band_info[0].ap_ezdev.wdev)
		ez_ad->ez_band_info[0].lut_driver_ops.ez_send_broadcast_deauth(&ez_ad->ez_band_info[0].ap_ezdev);
		if (ez_ad->ez_band_info[1].ap_ezdev.wdev)
			ez_ad->ez_band_info[1].lut_driver_ops.ez_send_broadcast_deauth(&ez_ad->ez_band_info[1].ap_ezdev); 
	}
#endif	
	return TRUE; 
}


unsigned char ez_set_ezgroup_id_hook(
	ez_dev_t *ezdev,
	unsigned char *ez_group_id,
	unsigned int ez_group_id_len,
	unsigned char inf_idx)
{
	int i;
	
	struct _ez_security *ez_sec_info = &ezdev->ez_security;
	if (ez_sec_info->ez_group_id_len != 0) {
		EZ_MEM_FREE( ez_sec_info->ez_group_id);
		ez_sec_info->ez_group_id_len = 0;
	}
	ez_sec_info->ez_group_id_len = ez_group_id_len;
	EZ_MEM_ALLOC(NULL, &ez_sec_info->ez_group_id, ez_sec_info->ez_group_id_len);
	if (ez_sec_info->ez_group_id) {
		NdisZeroMemory(ez_sec_info->ez_group_id, ez_sec_info->ez_group_id_len);
		NdisCopyMemory(ez_sec_info->ez_group_id, ez_group_id, ez_sec_info->ez_group_id_len);
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("IF(ra%d) %s :: group id \n", 
			inf_idx, __FUNCTION__));
		for (i = 0; i < ez_sec_info->ez_group_id_len; i++) {
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("%02x ", ez_sec_info->ez_group_id[i]));
		}
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("\n"));
		return TRUE;
	}
	else {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("IF(ra%d) %s::Memory alloc fails\n\n", 
			inf_idx, __FUNCTION__));
		ez_sec_info->ez_group_id_len = 0;
		return FALSE;
	}
}



unsigned char ez_set_group_id_hook(
	ez_dev_t *ezdev,
	unsigned char *group_id,
	unsigned int group_id_len,
	unsigned char inf_idx)
{
	int i;
	struct _ez_security *ez_sec_info = &ezdev->ez_security;
	if (ez_sec_info->group_id_len != 0) {
		EZ_MEM_FREE( ez_sec_info->group_id);
		ez_sec_info->group_id_len = 0;
	}
	ez_sec_info->group_id_len = group_id_len;
	EZ_MEM_ALLOC(NULL, &ez_sec_info->group_id, ez_sec_info->group_id_len);
	if (ez_sec_info->group_id) {
		NdisZeroMemory(ez_sec_info->group_id, ez_sec_info->group_id_len);
		NdisCopyMemory(ez_sec_info->group_id, group_id, ez_sec_info->group_id_len);
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("IF(ra%d) %s :: group id \n", 
			inf_idx, __FUNCTION__));
		for (i = 0; i < ez_sec_info->group_id_len; i++) {
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("%02x ", ez_sec_info->group_id[i]));
		}
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("\n"));
		return TRUE;
	}
	else {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("IF(ra%d) %s::Memory alloc fails\n\n", 
			inf_idx, __FUNCTION__));
		ez_sec_info->group_id_len = 0;
		return FALSE;
	}
}


unsigned char ez_set_gen_group_id_hook(
	ez_dev_t *ezdev,
	unsigned char *gen_group_id,
	unsigned int gen_group_id_len,
	unsigned char inf_idx)
{
	int i;
	UCHAR hash_data[LEN_PMK];
	struct _ez_security *ez_sec_info = &ezdev->ez_security;
	if (ez_sec_info->gen_group_id_len != 0) 
	{
		EZ_MEM_FREE( ez_sec_info->gen_group_id);
		ez_sec_info->gen_group_id_len = 0;
	}

	NdisZeroMemory(&hash_data[0], LEN_PMK);
	ezdev->driver_ops->RT_SHA256(ezdev, gen_group_id, gen_group_id_len, &hash_data[0]);
	ez_set_group_id_hook(ezdev, &hash_data[0], LEN_PMK, inf_idx);

	ez_sec_info->gen_group_id_len = gen_group_id_len;
	EZ_MEM_ALLOC(NULL, &ez_sec_info->gen_group_id, ez_sec_info->gen_group_id_len);

	if (ez_sec_info->gen_group_id)
	{
		NdisZeroMemory(ez_sec_info->gen_group_id, ez_sec_info->gen_group_id_len);
		NdisCopyMemory(ez_sec_info->gen_group_id, gen_group_id, ez_sec_info->gen_group_id_len);
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
			("IF(ra%d) %s :: group id \n", 
			inf_idx, __FUNCTION__));
		for (i = 0; i < ez_sec_info->gen_group_id_len; i++) {
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
			("%02x ", ez_sec_info->gen_group_id[i]));
		}
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
			("\n"));
		return TRUE;
	}
	else 
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("IF(ra%d) %s::Memory alloc fails\n\n", 
			inf_idx, __FUNCTION__));
		ez_sec_info->gen_group_id_len = 0;
		return FALSE;
	}
}

void ez_set_rssi_threshold_hook(
	ez_dev_t *ezdev,
	char rssi_threshold)
{
	ezdev->ez_security.rssi_threshold = rssi_threshold;
}

void ez_set_max_scan_delay_hook(
	ez_dev_t *ezdev,
	UINT32 max_scan_delay)
{
	EZ_ADAPTER *ez_ad;
	ez_ad = ezdev->ez_ad;
	ez_ad->max_scan_delay = max_scan_delay;
}

void ez_set_api_mode_hook(
	ez_dev_t *ezdev,
	char ez_api_mode)
{
	EZ_ADAPTER *ez_ad;
	ez_ad = ezdev->ez_ad;
	ez_ad->ez_api_mode = ez_api_mode;
}

INT ez_merge_group_hook(ez_dev_t *ezdev, UCHAR *macAddress)
{
	struct _ez_security *ez_sec_info;
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	ez_sec_info = &ezdev->ez_security;

	if (ezdev->ezdev_type == EZDEV_TYPE_APCLI)
	{
		if (ezdev->driver_ops->get_apcli_enable(ezdev) == TRUE)
		{
			
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
			("CLI Interface already enabled.\n"));
			return FALSE;
		}
		

		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("IF(apcli%d) %s:: This command is from apcli interface now.\n", 
			ezdev->ez_band_idx, __FUNCTION__));

		if (ezdev->driver_ops->ez_is_timer_running
			(ezdev, ez_sec_info->ez_group_merge_timer))
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
			("Merge going on.\n"));
			return FALSE;
		}
		
		EZ_UPDATE_APCLI_CAPABILITY_INFO(ez_ad, EZ_SET_ACTION, ALLOW_MERGE, ezdev->ez_band_idx);
		COPY_MAC_ADDR(ez_sec_info->merge_peer_addr, macAddress);
					
	}
	else
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
			("%s:: This command is from ra interface now.\n", 
			__FUNCTION__));
		if (ezdev->driver_ops->ez_is_timer_running
			(ezdev, ez_sec_info->ez_group_merge_timer))
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
			("Merge going on.\n"));
			return FALSE;
		}
		
		EzStartGroupMergeTimer(ezdev);
		EZ_UPDATE_CAPABILITY_INFO(ezdev, EZ_SET_ACTION, ALLOW_MERGE);
		COPY_MAC_ADDR(ez_sec_info->merge_peer_addr, macAddress);
	}
	
	return TRUE;
}

void ez_apcli_force_ssid_hook(
	ez_dev_t *ezdev,
	unsigned char *ssid, 
	unsigned char ssid_len)
{
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,(" %s -->\n", __FUNCTION__));
	
	{
		NdisZeroMemory(ezdev->ez_security.ez_apcli_force_ssid,MAX_LEN_OF_SSID);
		NdisCopyMemory(ezdev->ez_security.ez_apcli_force_ssid,ssid,ssid_len);
		ezdev->ez_security.ez_apcli_force_ssid_len = ssid_len;
	}
}

void ez_set_force_bssid_hook(
	ez_dev_t *ezdev, 
	UCHAR *mac_addr)
{
	NdisCopyMemory(ezdev->ez_security.ez_apcli_force_bssid,
		mac_addr,MAC_ADDR_LEN);
}

void ez_set_push_bw_hook(ez_dev_t *ezdev, UINT8 same_bw_push)
{
	EZ_ADAPTER *ez_ad;
	ez_ad = ezdev->ez_ad;
	ez_ad->push_bw_config = same_bw_push;
}

void ez_handle_action_txstatus_hook(ez_dev_t *ezdev, UINT8 * Addr)
{
	struct _ez_peer_security_info *ez_peer = NULL;
	ez_peer = ez_peer_table_search_by_addr_hook(ezdev,Addr);
	if (ez_peer == NULL)
	{
		ASSERT(FALSE);
		return;
	}
	ez_peer->delete_in_differred_context = TRUE;
	if(ez_is_link_duplicate(ez_peer))
	{
		struct _ez_peer_security_info *ez_other_band_peer = ez_get_other_band_ez_peer(ezdev, ez_peer);
		ez_dev_t *ez_other_band_ezdev = ez_other_band_peer->ezdev;
		if (ez_other_band_peer)
		{
			ez_other_band_peer->delete_in_differred_context = TRUE;
			
			ez_other_band_ezdev->driver_ops->ez_send_unicast_deauth
				(ez_other_band_peer->ezdev, ez_other_band_peer->mac_addr);		
		} else {
			ASSERT(NULL);
		}
		
	}
	ezdev->driver_ops->ez_send_unicast_deauth(ezdev, Addr);	
}

void set_ssid_psk_hook(ez_dev_t *ezdev, 
	char *ssid1, char *pmk1, char *psk1, 
	char *ssid2, char *pmk2, char *psk2, 
	char *ssid3, char *pmk3, char *psk3, 
	char *EncrypType1, char *EncrypType2, 
	char *AuthMode1, char *AuthMode2)
{
	//! Leverage form MP.1.0 CL 170364
#ifdef IF_UP_DOWN
	if (!ez_all_intf_up_hook(ezdev->ez_ad)) {
		return;
	}
#endif
	ez_inform_all_interfaces(ezdev->ez_ad, ezdev, ACTION_UPDATE_CONFIG_STATUS);
	if (ez_is_triband_hook())
	{
		ez_update_triband_ssid_pmk(ezdev->ez_ad, ssid1, strlen(ssid1), pmk1, psk1, ssid2, strlen(ssid2), pmk2, psk2, ssid3, strlen(ssid3), pmk3, psk3, 
			EncrypType1, EncrypType2, AuthMode1, AuthMode2);
	}
#if 0
	else if(ez_ad->is_man_nonman){ 
	//! Levarage from MP1.0 CL#170037
		
		ez_update_man_plus_nonman_ssid_pmk(ezdev->ez_ad, ssid2, strlen(ssid2), pmk2, psk2, ssid1, strlen(ssid1), pmk1, psk1, 
			EncrypType1, AuthMode1);
	
	}
#endif	
	else {
		ez_update_ssid_pmk(ezdev->ez_ad, ssid1, strlen(ssid1), psk1, pmk1, ssid2, strlen(ssid2),psk2, pmk2, NULL);
	}

}


void ez_apcli_link_down_hook(ez_dev_t *ezdev,unsigned long Disconnect_Sub_Reason)
{	
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	ez_dev_t *ap_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev;
	ezdev->ez_security.first_scan = TRUE;
	EZ_CLEAR_CAP_CONNECTED(ap_ezdev->ez_security.capability);
	EZ_UPDATE_CAPABILITY_INFO(ap_ezdev, EZ_CLEAR_ACTION, CONNECTED);
	if (ezdev->ez_security.disconnect_by_ssid_update) 
	{
		ezdev->ez_security.disconnect_by_ssid_update = 0;
	}
	else 
	{
		if(Disconnect_Sub_Reason == APCLI_DISCONNECT_SUB_REASON_MNT_NO_BEACON)
		{
				ezdev->ez_security.ez_scan_same_channel = TRUE;
				NdisGetSystemUpTime(&ezdev->ez_security.ez_scan_same_channel_timestamp);
		}
		else
			ez_initiate_new_scan(ez_ad);
	}
}

BOOLEAN ez_update_connection_permission_hook(
	ez_dev_t *ezdev, enum EZ_CONN_ACTION action)
{
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	static ez_dev_t *ezdev_permission_pending = NULL;
	int i = 0;
	int irq_flags;



	
	if(!IS_SINGLE_CHIP_DBDC(ez_ad))
		EZ_IRQ_LOCK(&ez_ad->ez_conn_perm_lock, irq_flags)


	
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s-->%d\n", __FUNCTION__, action));
	if (ezdev)
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,(" bandIdx=%d\n", ezdev->ez_band_idx));
	switch (action)
	{
		case EZ_ALLOW_ALL:
		{
			BOOLEAN conn_wait_tmr_running = ez_ad->ez_connect_wait_ezdev->driver_ops->ez_is_timer_running(ez_ad->ez_connect_wait_ezdev
				, ez_ad->ez_connect_wait_timer);
			if (ez_is_connection_allowed_hook(ezdev))
			{
				if(conn_wait_tmr_running == FALSE){
				//	if(IS_SINGLE_CHIP_DBDC(pAd))
					for (i=0; i < MAX_EZ_BANDS; i++) {
/*				if (ez_ad->ez_band_info[i].ap_ezdev.wdev)*/
						{
							ez_ad->ez_band_info[i].ap_ezdev.ez_security.ez_is_connection_allowed = TRUE;
						}
/*				if (ez_ad->ez_band_info[i].cli_ezdev.wdev)*/
						{
							ez_ad->ez_band_info[i].cli_ezdev.ez_security.ez_is_connection_allowed = TRUE;
						}
					}
	
				}
			}
			else{
				if(!IS_SINGLE_CHIP_DBDC(ez_ad))
					EZ_IRQ_UNLOCK(&ez_ad->ez_conn_perm_lock, irq_flags);
				return FALSE;
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s: EZ_ALLOW_ALL action ignored as conn wait timer running\n", __FUNCTION__));				
			}
	
			break;
	
		}
		case EZ_DISALLOW_ALL:
		{
			
			//if(IS_SINGLE_CHIP_DBDC(pAd))
			for (i=0; i < MAX_EZ_BANDS; i++) {
/*		if (ez_ad->ez_band_info[i].ap_ezdev.wdev)*/
				{
					ez_ad->ez_band_info[i].ap_ezdev.ez_security.ez_is_connection_allowed = FALSE;
				}
/*		if (ez_ad->ez_band_info[i].cli_ezdev.wdev)*/
				{
					ez_ad->ez_band_info[i].cli_ezdev.ez_security.ez_is_connection_allowed = FALSE;
				}
			}
			break;
		}
		case EZ_ADD_DISALLOW:
			if (ezdev) {
				ezdev->ez_security.ez_is_connection_allowed = FALSE;
				ezdev->ez_security.ez_connection_permission_backup = FALSE;
			}
			break;
		case EZ_ADD_ALLOW:
			if (ezdev) {
				ezdev->ez_security.ez_is_connection_allowed = TRUE;
				ezdev->ez_security.ez_connection_permission_backup = TRUE;
			}
	
			break;
		case EZ_DISALLOW_ALL_ALLOW_ME:
			{
				if (ez_is_connection_allowed_hook(ezdev))
				{
				
					for (i=0; i < MAX_EZ_BANDS; i++) {
/*				if (ez_ad->ez_band_info[i].ap_ezdev.wdev)*/
						{
							ez_ad->ez_band_info[i].ap_ezdev.ez_security.ez_is_connection_allowed = FALSE;
						}
/*				if (ez_ad->ez_band_info[i].cli_ezdev.wdev)*/
						{
							ez_ad->ez_band_info[i].cli_ezdev.ez_security.ez_is_connection_allowed = FALSE;
						}
					}
					{
						if (ezdev) {
							ezdev->ez_security.ez_is_connection_allowed = TRUE;
						}
					}
					}
				else {
					if(!IS_SINGLE_CHIP_DBDC(ez_ad))
					EZ_IRQ_UNLOCK(&ez_ad->ez_conn_perm_lock, irq_flags);
					return FALSE;
				}
			}		
			break;
		case EZ_ALLOW_ALL_TIMEOUT:
		{
	
			for (i=0; i < MAX_EZ_BANDS; i++) {
/*				if (ez_ad->ez_band_info[i].ap_ezdev.wdev)*/
				{
					ez_ad->ez_band_info[i].ap_ezdev.ez_security.ez_is_connection_allowed = TRUE;
				}
/*				if (ez_ad->ez_band_info[i].cli_ezdev.wdev)*/
				{
					ez_ad->ez_band_info[i].cli_ezdev.ez_security.ez_is_connection_allowed = TRUE;
				}
			}

			ez_ad->ez_connect_wait_ezdev->ez_security.weight_update_going_on = FALSE;
				
			break;
		}
		case EZ_ENQUEUE_PERMISSION:
		{
			if (ezdev_permission_pending == NULL)
			{
				ezdev_permission_pending = ezdev;
			}
			break;
		}
		case EZ_DEQUEUE_PERMISSION:
		{
			if (ezdev_permission_pending == ezdev)
			{
				ezdev_permission_pending = NULL;
			}
			break;
		}
		default:
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,(" Unknown Action\n"));
	
	}
	if(!IS_SINGLE_CHIP_DBDC(ez_ad))
		EZ_IRQ_UNLOCK(&ez_ad->ez_conn_perm_lock, irq_flags);
	return TRUE;

}


BOOLEAN ez_is_connection_allowed_hook(ez_dev_t *ezdev)
{
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s:band idx=%d, is allowed=%d\n", __FUNCTION__, ezdev->ez_band_idx,ezdev->ez_security.ez_is_connection_allowed ));
#ifdef EZ_API_SUPPORT 
	if (ezdev->ez_security.ez_api_mode == CONNECTION_OFFLOAD)
	{
		return TRUE;
	}
#endif
#ifdef IF_UP_DOWN
	if (ezdev && !ez_all_intf_up_hook(ezdev->ez_ad))
		return FALSE;
#endif

	return ezdev->ez_security.ez_is_connection_allowed;
}


BOOLEAN ez_probe_rsp_join_action_hook(ez_dev_t *ezdev, 
	char *network_weight)
{
	EZ_ADAPTER *ez_ad= ezdev->ez_ad;
	if (ez_is_weight_same_mod(ez_ad->device_info.network_weight,
		network_weight))
	{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,(" Discard Probe Rsp as it is a child node\n"));
			return TRUE;
	}
	return FALSE;
}

void ez_update_connection_hook(ez_dev_t *ezdev)
{
	UCHAR sta_count=0;
	int i;
	EZ_BAND_INFO *ez_band;
	EZ_ADAPTER *ez_ad;

	ez_ad = ezdev->ez_ad;
	ez_band = &ez_ad->ez_band_info[ezdev->ez_band_idx];
	
	for (i=0; i<EZ_MAX_STA_NUM;i++)
	{
		if (ez_band->ez_peer_table[i].valid && ez_band->ez_peer_table[i].port_secured == FALSE)
			sta_count++;
	}
	if (sta_count <= 1)
	{
		ez_update_connection_permission_hook(ezdev,EZ_ALLOW_ALL);
	}
	else
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("--> don't allow all connections as still some connection pending on AP: %d\n", sta_count));

}

void ez_handle_pairmsg4_hook(ez_dev_t *ezdev, UCHAR *peer_mac)
{

	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	ez_dev_t *ap_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev;

#ifdef EZ_API_SUPPORT
	if (ezdev->ez_security.ez_api_mode == CONNECTION_OFFLOAD){
		return;
	}	
#endif	
	
	/*APCLI has connected to a non-easy setup AP, allocate itself a new node number according to the MAC address of the root AP and its own aid.*/
	COPY_MAC_ADDR(ezdev->bssid, peer_mac);
	COPY_MAC_ADDR(ezdev->ez_security.this_band_info.cli_peer_ap_mac, peer_mac);
	COPY_MAC_ADDR(ap_ezdev->ez_security.this_band_info.cli_peer_ap_mac, peer_mac);
	ezdev->ez_security.best_ap_rssi_threshold = 0;

	ezdev->ez_security.this_band_info.non_easy_connection = TRUE;
	
	update_and_push_weight(ezdev, peer_mac, NULL);

	//ez_apcli_allocate_self_node_number(&pEntry->ez_adapter->device_info.ez_node_number,pEntry);
	//ez_update_node_number(pAd,pEntry->wdev,pEntry->ez_adapter->device_info.ez_node_number);

	ezdev->ez_security.first_loop_check = TRUE;
	ez_chk_loop_thru_non_ez_ap(ez_ad, ezdev);

}


void ez_roam_hook(ez_dev_t *ezdev, 
	unsigned char bss_support_easy_setup,
	beacon_info_tag_t* bss_beacon_info,
	char *bss_bssid,
	UCHAR bss_channel)
{
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	int irq_flags;

	if (ezdev->ez_security.this_band_info.non_easy_connection)
	{
		printk("connected to non-easy, roam not supported\n");
	}
	if(bss_support_easy_setup)
	{
		if (NdisEqualMemory(ez_ad->device_info.network_weight,
			bss_beacon_info->network_weight,
			NETWORK_WEIGHT_LEN))
		{
			// connect within the same network
			// it means we provided the node number to that device.
			// break the link to that device. wait for weight change and then connect.
			struct _ez_peer_security_info *ez_link_peer = NULL, *ez_dup_link_peer = NULL;
			ez_link_peer = ez_find_link_to_roam_candidate(ez_ad, 
							ezdev,
							bss_beacon_info->node_number);
			if (ez_link_peer != NULL)
			{

				{
					if (((ez_dev_t *)(ez_link_peer->ezdev))->ezdev_type == EZDEV_TYPE_APCLI) {

						/*link to the target AP is apcli interface on which roaming is triggered.*/
						if (ez_link_peer->ezdev == ezdev)
						{
							//if(ez_link_peer->this_band_info.shared_info.link_duplicate == TRUE)
							if(ezdev->ez_security.this_band_info.shared_info.link_duplicate == TRUE)
								ez_dup_link_peer = ez_get_other_band_ez_peer(ez_link_peer->ezdev,ez_link_peer);

							ez_initiate_roam(ezdev,bss_bssid,bss_channel);
							//both apcli are connected to the same AP. 
							//break both and move to the new AP
							

							if (ez_dup_link_peer != NULL)
							{
									ez_hex_dump("APCLIDuplicateLinkAddr", ez_dup_link_peer->mac_addr, MAC_ADDR_LEN);
									ez_initiate_roam(ez_dup_link_peer->ezdev, ez_get_other_band_bssid(bss_beacon_info),
										ez_get_other_band_channel(bss_beacon_info));
							}
							else
							{
								//link is with some other AP. do nothing and don't break the link
							}
						}
						else
						{
							// if link to the device is through the other apcli, break the link and make it connect to the same AP
							ez_initiate_roam(ezdev,bss_bssid,bss_channel);
							ez_initiate_roam(ez_link_peer->ezdev, 
								ez_get_other_band_bssid(bss_beacon_info),
								ez_get_other_band_channel(bss_beacon_info));
						}
					}
					else
					{
					// link is through the AP interface
					// Break the linkto the target device.
					
						if(ez_link_peer->this_band_info.shared_info.link_duplicate == TRUE)
							ez_dup_link_peer = ez_get_other_band_ez_peer(ez_link_peer->ezdev,ez_link_peer);

						COPY_MAC_ADDR(((ez_dev_t *)(ez_link_peer->ezdev))->ez_security.ez_ap_roam_blocked_mac,ez_link_peer->mac_addr);
						ez_link_peer->ez_disconnect_due_roam = TRUE;
//! Levarage from MP1.0 CL#170063
						ez_link_peer->ezdev->driver_ops->ez_send_unicast_deauth
						(ez_link_peer->ezdev,ez_link_peer->mac_addr);
						if(ez_dup_link_peer)
						{
							ez_dup_link_peer->ez_disconnect_due_roam = TRUE;
							COPY_MAC_ADDR(((ez_dev_t *)(ez_dup_link_peer->ezdev))->ez_security.ez_ap_roam_blocked_mac,ez_dup_link_peer->mac_addr);
//! Levarage from MP1.0 CL#170063
							
							ez_dup_link_peer->ezdev->driver_ops->ez_send_unicast_deauth(ez_dup_link_peer->ezdev,ez_dup_link_peer->mac_addr);

						}
						if (ezdev->ez_security.this_band_info.shared_info.link_duplicate)
						{
							ez_dev_t * other_band_ezdev = ez_get_otherband_cli_ezdev(ezdev);
							ez_initiate_roam(ezdev,bss_bssid,bss_channel);
							ez_initiate_roam(other_band_ezdev,
								ez_get_other_band_bssid(bss_beacon_info),
								ez_get_other_band_channel(bss_beacon_info));
						}
						else
						{
							ez_initiate_roam(ezdev,bss_bssid,bss_channel);
						}
						
					}
				}
			}
			else
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("ez_link peer is NULL!!!\n"));
		}
		else
		{
			//weight is different, connect to it.
			/* if the link to be broken is the only link and it is the wt defining link, 
				break it, assign self a new node number/wt and then connect to the new bssid */
						// if going to another user configured, then break both links
			/*if link to be broken is not the wt defining link or there are 2 links, then break the link without updating the node number and wt and connect
			to the new one.*/
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("weight is different\n"));
			if (ez_ad->device_info.network_weight[0] == 0xf && ez_is_bss_user_configured(bss_beacon_info))
			{
				ez_dev_t *other_band_ezdev = NULL;
				ez_dev_t *wt_defining_ezdev = ez_ad->device_info.weight_defining_link.ezdev;
				if (ezdev->ez_security.user_configured == TRUE)
				{
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("This is User configured device, can't roam to another UserConfigured device\n"));
					return;
				}
				/*If AP interface is the wt defining link, then break it before roam*/
				if(wt_defining_ezdev && wt_defining_ezdev->ezdev_type == EZDEV_TYPE_AP)
				{
					struct _ez_peer_security_info *ez_peer = ez_peer_table_search_by_addr_hook(wt_defining_ezdev,
																			ez_ad->device_info.weight_defining_link.peer_mac);
					struct _ez_peer_security_info *ez_dup_link_peer=NULL;
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ApInterface is the wt defining link\n"));
					if (ez_peer == NULL)
					{
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("Ez_peer Not found!!!\n"));
						return;
					}
					if (ez_peer->this_band_info.shared_info.link_duplicate == TRUE)
						ez_dup_link_peer =ez_get_other_band_ez_peer(wt_defining_ezdev,
							ez_peer);

					COPY_MAC_ADDR(((ez_dev_t *)(ez_peer->ezdev))->ez_security.ez_ap_roam_blocked_mac,
						ez_peer->mac_addr);
					ez_peer->ez_disconnect_due_roam = TRUE;
//! Levarage from MP1.0 CL#170063
					ez_peer->ezdev->driver_ops->ez_send_unicast_deauth
						(ez_peer->ezdev,ez_peer->mac_addr);
					if(ez_dup_link_peer)
					{
						ez_dup_link_peer->ez_disconnect_due_roam = TRUE;
						COPY_MAC_ADDR(((ez_dev_t *)(ez_dup_link_peer->ezdev))->ez_security.ez_ap_roam_blocked_mac,ez_dup_link_peer->mac_addr);
//! Levarage from MP1.0 CL#170063
						ez_dup_link_peer->ezdev->driver_ops->ez_send_unicast_deauth
						(ez_dup_link_peer->ezdev, ez_dup_link_peer->mac_addr);
					}
					ez_initiate_roam(ezdev,bss_bssid,bss_channel);
				}
				else
				{
					/*If roaming APCLI is the wt defining link, then break it and roam only that interface.*/
					if((wt_defining_ezdev == ezdev) && (!ezdev->ez_security.this_band_info.shared_info.link_duplicate) )
					{
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Roam only APCLI interface\n"));
						ez_initiate_roam(ezdev,bss_bssid,bss_channel);
					}
					else
					{
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("both are user configured, roam both device."));
						other_band_ezdev = ez_get_otherband_cli_ezdev(ezdev);
						ez_initiate_roam(ezdev,bss_bssid,bss_channel);
						if (other_band_ezdev) {
							ez_initiate_roam(other_band_ezdev,
								ez_get_other_band_bssid(bss_beacon_info),
								ez_get_other_band_channel(bss_beacon_info));
						}
					}
				}
			}
			else
			{
				ez_initiate_roam(ezdev,bss_bssid,bss_channel);
			}
		}
	}
	else
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Roaming to Non easy setup AP\n"));
		//ez_hex_dump("Bssid", pBssEntry->Bssid,6);
		//ez_hex_dump("OwnWtMac");
		if (1)//MAC_ADDR_EQUAL(pBssEntry->Bssid,&ez_adapter->device_info.network_weight[1] ))
		{
			
			// both are connected to same AP, roam both interfaces otherwise loop will be created
			ez_dev_t * other_band_ezdev = NULL;
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Roam both interfaces\n"));
			other_band_ezdev = ez_get_otherband_cli_ezdev(ezdev);
			ez_initiate_roam(ezdev,bss_bssid,bss_channel);
			//ez_initiate_roam(other_band_wdev,ez_get_other_band_bssid(pBssEntry),ez_get_other_band_channel(pBssEntry));
			if (other_band_ezdev) {
			//! below leg will never be called.
#if 0
				MAC_TABLE_ENTRY *pEntry = NULL;
				pEntry = MacTableLookup(other_band_wdev->sys_handle,other_band_wdev->bssid);
				if (pEntry)
				{
						ez_hex_dump("address :", pEntry->Addr, 6);
						ez_peer = ez_peer_table_search_by_addr_hook(pEntry->wdev,pEntry->Addr);
						if (ez_peer == NULL)
						{
							  ASSERT(FALSE);
							  return;
						}
						ez_peer->delete_in_differred_context = TRUE;
						ez_peer->ez_disconnect_due_roam = TRUE;
				}
#endif		
				other_band_ezdev->driver_ops->ez_send_unicast_deauth
					(other_band_ezdev,
						other_band_ezdev->bssid);
			}
		} else {
			ez_initiate_roam(ezdev,bss_bssid,bss_channel);
		}
	}

		/*if roaming is initiated, disallow all pending connections and allow only roaming connection.*/
	//! Levarage from MP1.0 CL#170037
		 if (IS_DUAL_CHIP_DBDC(ez_ad) && !ez_is_triband_hook())
			EZ_IRQ_LOCK(&ez_ad->ez_mlme_sync_lock, irq_flags)
		ez_update_connection_permission_hook(ezdev,EZ_ALLOW_ALL_TIMEOUT);
		ez_update_connection_permission_hook(ezdev,EZ_ENQUEUE_PERMISSION);
		ez_update_connection_permission_hook(ezdev,EZ_DISALLOW_ALL_ALLOW_ME);
	//! Levarage from MP1.0 CL#170037
		 if (IS_DUAL_CHIP_DBDC(ez_ad) && !ez_is_triband_hook())
			EZ_IRQ_UNLOCK(&ez_ad->ez_mlme_sync_lock, irq_flags);
		ez_notify_roam(ez_ad, NULL, TRUE, NULL, 0);

}

BOOLEAN ez_set_roam_bssid_hook(ez_dev_t *ezdev, UCHAR *roam_bssid)
{
	
	if (ez_is_roaming_ongoing_hook(ezdev->ez_ad) == TRUE)
		return FALSE;// Roaming already ongoing on some interface.

	NdisGetSystemUpTime(&ezdev->ez_security.ez_roam_info.timestamp);

	NdisCopyMemory(ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid,roam_bssid,MAC_ADDR_LEN);

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("Set_EasySetup_ForceBssid_Proc (%2X:%2X:%2X:%2X:%2X:%2X)\n",
		ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid[0],
		ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid[1],
		ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid[2],
		ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid[3],
		ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid[4],
		ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid[5]));
	return TRUE;
}

void ez_reset_roam_bssid_hook(ez_dev_t *ezdev)
{
	COPY_MAC_ADDR(ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid,ZERO_MAC_ADDR);
	ezdev->ez_security.ez_roam_info.roam_channel =0;
	ezdev->ez_security.ez_roam_info.timestamp = 0;

}

channel_info_t *ez_get_channel_hook(ez_dev_t *ezdev)
{
	return &ezdev->ez_security.this_band_info.shared_info.channel_info;
}

BOOLEAN ez_get_push_bw_hook(ez_dev_t *ezdev)
{
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	return ez_ad->push_bw_config; 
}

BOOLEAN ez_did_ap_fallback_hook(ez_dev_t *ezdev)
{
	return ezdev->ez_security.ap_did_fallback;
}

BOOLEAN ez_ap_fallback_channel(ez_dev_t *ezdev)
{
	return ezdev->ez_security.fallback_channel;
}



void ez_prepare_security_key_hook(
	ez_dev_t *ezdev,
	unsigned char *peer_addr,
	unsigned char authenticator)
{
	struct _ez_security *ez_sec_info;
	struct _ez_peer_security_info *ez_peer;
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	int irq_flags;
	ez_dev_t *other_ezdev = ez_get_otherband_ap_ezdev(ezdev);
	ez_sec_info = &ezdev->ez_security;

	ez_peer = ez_peer_table_search_by_addr_hook(ezdev, peer_addr);
	if (ez_peer) {
		EZ_IRQ_LOCK(ezdev->ez_peer_table_lock, irq_flags);
		ez_gen_dh_private_key(ezdev, ez_peer);
		ez_compute_dh_key(ezdev, ez_peer);
		if (authenticator) {
#if 0
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
				("%s - Configure Status = %d\n", 
				__FUNCTION__, ez_ad->configured_status));
#endif
			ez_get_sw_encrypted_key(ez_sec_info, ez_peer, ezdev->if_addr, peer_addr);
			if (ez_ad->configured_status == EZ_UNCONFIGURED) {

				NdisZeroMemory(&ez_peer->this_band_info.pmk[0], EZ_PMK_LEN);
				NdisCopyMemory(&ez_peer->this_band_info.pmk[0], &ez_peer->dh_key[0], EZ_PMK_LEN);
				NdisCopyMemory(ez_sec_info->this_band_info.pmk, &ez_peer->dh_key[0], EZ_PMK_LEN);
				if (other_ezdev) {
					NdisCopyMemory(other_ezdev->ez_security.this_band_info.pmk,ez_peer->dh_key,EZ_PMK_LEN);
				}
				if (ez_ad->band_count == 1)
				{
					ez_dev_t *cli_ezdev = EZ_GET_EZBAND_CLIDEV(ez_ad,ez_peer->ez_band_idx);
					NdisCopyMemory(ezdev->ez_security.other_band_info_backup.pmk,ez_peer->dh_key,EZ_PMK_LEN);
					NdisCopyMemory(cli_ezdev->ez_security.other_band_info_backup.pmk, ez_peer->dh_key,EZ_PMK_LEN);
				}
				
			}
			else {
				NdisZeroMemory(&ez_peer->this_band_info.pmk[0], EZ_PMK_LEN);
				NdisCopyMemory(&ez_peer->this_band_info.pmk[0], &ezdev->ez_security.this_band_info.pmk[0], EZ_PMK_LEN);
				NdisCopyMemory(ez_sec_info->this_band_info.pmk, &ezdev->ez_security.this_band_info.pmk[0], EZ_PMK_LEN);
				if (other_ezdev) 
				{
					NdisCopyMemory(other_ezdev->ez_security.this_band_info.pmk,other_ezdev->ez_security.this_band_info.pmk,EZ_PMK_LEN);
				}
				//ez_hex_dump("PMK", ez_sec_info->this_band_info.pmk, EZ_PMK_LEN);
			}
		}
		else
			ez_get_sw_encrypted_key(ez_sec_info, ez_peer, peer_addr, ezdev->if_addr);
		EZ_IRQ_UNLOCK(ezdev->ez_peer_table_lock, irq_flags);
	}
}

enum_group_merge_action_t is_group_merge_candidate(unsigned int easy_setup_capability, 
	ez_dev_t *ezdev, 
	void *temp_bss_entry,
	UCHAR *Bssid)
{
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	if (EZ_GET_CAP_ALLOW_MERGE(ezdev->ez_security.capability) 
			&& MAC_ADDR_EQUAL(ezdev->ez_security.merge_peer_addr, BROADCAST_ADDR)) {
		if (!ezdev->driver_ops->ez_is_timer_running
			(ezdev, ezdev->ez_security.ez_group_merge_timer)){			
			EzStartGroupMergeTimer(ezdev);
		}
		if (EZ_GET_CAP_ALLOW_MERGE(easy_setup_capability))
		{	
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Searching for provider, Open merge allowed, MAC match\n"));
			if (temp_bss_entry != NULL){
				
				NdisZeroMemory(ezdev->ez_security.merge_peer_addr, MAC_ADDR_LEN);
				EZ_CLEAR_CAP_ALLOW_MERGE(ezdev->ez_security.capability);
				EZ_UPDATE_APCLI_CAPABILITY_INFO(ez_ad, EZ_CLEAR_ACTION, ALLOW_MERGE, ezdev->ez_band_idx);
				if (ezdev->driver_ops->ez_is_timer_running
					(ezdev, ezdev->ez_security.ez_group_merge_timer))
				{
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Group Merge completed, cancel timer here\n"));
					ezdev->driver_ops->ez_cancel_timer
						(ezdev, ezdev->ez_security.ez_group_merge_timer);
				}
				return TERMINATE_LOOP_MULTIPLE_AP_FOUND;
			} else {
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
					("%s(line.%d): found ap.\n", 
					__FUNCTION__, __LINE__));
				return CONTINUE_LOOP_TARGET_AP_FOUND;
			}
		} else {
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Searching for provider, Open Merge not allowed\n"));
			return CONTINUE_LOOP;
		}
	} 
	else if (EZ_GET_CAP_ALLOW_MERGE(ezdev->ez_security.capability) 
			&& EZ_GET_CAP_ALLOW_MERGE(easy_setup_capability)
			&& !MAC_ADDR_EQUAL(ezdev->ez_security.merge_peer_addr, ZERO_MAC_ADDR))
	{
		if (!ezdev->driver_ops->ez_is_timer_running
			(ezdev, ezdev->ez_security.ez_group_merge_timer)){			
			EzStartGroupMergeTimer(ezdev);
		}
		if (NdisEqualMemory(Bssid, ezdev->ez_security.merge_peer_addr, MAC_ADDR_LEN))
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Searching for provider, MAC match\n"));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
				("%s(line.%d): found ap.\n", 
				__FUNCTION__, __LINE__));
			return TERMINATE_LOOP_TARGET_AP_FOUND;
		} else 
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Searching for provider, MAC do not match\n"));
			return CONTINUE_LOOP;
		}
	}
	return EXIT_SWITCH_NOT_GROUP_MERGE;
}


void ez_process_action_frame_hook(
	ez_dev_t *ezdev,
	UCHAR *peer_mac,
	UCHAR *Msg,
	UINT msg_len)
{
	ez_dev_t *ezdev2p4 = ezdev;
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	unsigned char capability0;
	unsigned char tag;
	EID_STRUCT *pEid;
	unsigned char data_len;
	int Length;

	struct _ez_peer_security_info *ez_peer;
	ez_dev_t *ap_ezdev;
	BOOLEAN check_for_weight = TRUE;
	unsigned char *encrypted_data;
	unsigned char tag_count = 0;
	BOOLEAN group_id_updated = FALSE;
	unsigned char ez_this_band_psk_len;
	unsigned char ez_other_band_psk_len;
	
	capability0 = 0;
	pEid = (EID_STRUCT *)&Msg[LENGTH_802_11 + 2 + 3];

	if (ez_get_band(ezdev)){
		if (ez_get_otherband_ezdev(ezdev) != NULL) {
			ezdev2p4 = ez_get_otherband_ezdev(ezdev);
		}
	}
	ap_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev; 
	Length = 0;
	Length += LENGTH_802_11;
		
	Length += 2; //! action and category
	Length += 3; //! MTK OUI	
	

	ez_peer = ez_peer_table_search_by_addr_hook(ezdev, peer_mac);
	if (ez_peer == NULL)
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("Action frame from a non connected peer???\n"));
		return;
	}
	
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("------> %s (%02x:%02x:%02x:%02x:%02x:%02x)\n", __FUNCTION__,PRINT_MAC(peer_mac)));
	//! parse all the TAGS present in this action frame
	while ((Length + 2 + pEid->Len) <= msg_len){
		if (NdisEqualMemory(&pEid->Octet[0], &mtk_oui[0], MTK_OUI_LEN)) {
			capability0 = pEid->Octet[MTK_OUI_LEN];
			tag = pEid->Octet[EZ_TAG_OFFSET];
			if ((capability0 & MTK_VENDOR_EASY_SETUP) &&
				(tag == EZ_TAG_APCLI_ACTION_INFO)) {
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
					("%s(): this entry(%02x:%02x:%02x:%02x:%02x:%02x) is apcli.\n", 
					__FUNCTION__, PRINT_MAC(peer_mac)));
				//ezdev->driver_ops->ez_set_entry_apcli(ezdev, peer_mac, TRUE);
				ezdev->driver_ops->RtmpOSWrielessEventSendExt(ezdev, RT_WLAN_EVENT_CUSTOM, IW_WH_EZ_MY_AP_HAS_APCLI,
							    NULL, NULL, 0);
				continue;
			} 
			switch(tag)
			{
			case EZ_TAG_NOTIFY_ROAM:
				{
					
					ez_update_connection_permission_hook(ezdev, EZ_DISALLOW_ALL);
					ez_wait_for_connection_allow(ez_ad->ez_roam_time * EZ_SEC_TO_MSEC, ez_ad);
					ez_notify_roam(ez_ad, ez_peer, TRUE, NULL, 0);
					break;
				}
			case EZ_TAG_DELAY_DISCONNECT_COUNT:
				{
					//! disconect due to beacon loss will be delayed by delay_disconnect_count number of times sent by peer
					ezdev->ez_security.delay_disconnect_count = pEid->Octet[EZ_TAG_DATA_OFFSET];
					break;
				}
			case EZ_TAG_GROUP_ID_UPDATE:
				{
					//! flag to show if this action is triggered due to a group ID merge
					group_id_updated = pEid->Octet[EZ_TAG_DATA_OFFSET];
					//! flag to show that confg comaprision is reqyired
					tag_count++;
					break;
				}
			case EZ_TAG_OPEN_GROUP_ID:
				{
					NdisCopyMemory(ez_peer->open_group_id, &pEid->Octet[EZ_TAG_DATA_OFFSET], pEid->Octet[EZ_TAG_LEN_OFFSET]);
					ez_peer->open_group_id_len = pEid->Octet[EZ_TAG_LEN_OFFSET];
					tag_count++;
					break;
				}
			case EZ_TAG_GROUP_ID:
				{
					data_len = pEid->Octet[EZ_TAG_LEN_OFFSET];
					//! delete previous group ID corresponding to this peer
					if (ez_peer->group_id)
					{
						EZ_MEM_FREE( ez_peer->group_id);
					}
					//!allocate new memroy for new group ID
					EZ_MEM_ALLOC(NULL, &ez_peer->group_id, data_len);
					if (ez_peer->group_id && data_len) {
						NdisZeroMemory(ez_peer->group_id, data_len);
						encrypted_data = &pEid->Octet[EZ_TAG_DATA_OFFSET];
						/* AES */
						ezdev->driver_ops->AES_Key_Unwrap(ezdev,
										encrypted_data, data_len,
									   &ez_peer->sw_key[0], LEN_PTK_KEK, 
									   ez_peer->group_id, &ez_peer->group_id_len);	
		
					}
					tag_count++;
					break;
				}
			case EZ_TAG_GROUPID_SEED:
				{
					data_len = pEid->Octet[EZ_TAG_LEN_OFFSET];
					//! delete previous group ID corresponding to this peer
					if (ez_peer->gen_group_id)
					{
						EZ_MEM_FREE( ez_peer->gen_group_id);
					}
					//!allocate new memroy for new group ID
					EZ_MEM_ALLOC(NULL, &ez_peer->gen_group_id, data_len);
					if (ez_peer->gen_group_id && data_len) {
						NdisZeroMemory(ez_peer->gen_group_id, data_len);
						encrypted_data = &pEid->Octet[EZ_TAG_DATA_OFFSET];
						/* AES */
						ezdev->driver_ops->AES_Key_Unwrap(ezdev,
										encrypted_data, data_len,
									   &ez_peer->sw_key[0], LEN_PTK_KEK, 
									   ez_peer->gen_group_id, &ez_peer->gen_group_id_len);	
		

					}
					break;
				}

				case EZ_TAG_PMK:
					{
						unsigned char *encrypted_data;
						unsigned char *pmk;
						unsigned int pmk_len;
						encrypted_data = &pEid->Octet[EZ_TAG_DATA_OFFSET];
						data_len = pEid->Octet[EZ_TAG_LEN_OFFSET];
							
						EZ_MEM_ALLOC(NULL, &pmk, data_len+EZ_AES_KEY_ENCRYPTION_EXTEND);
						if (pmk) {
							/* AES */
							ezdev->driver_ops->AES_Key_Unwrap(ezdev,
											encrypted_data, data_len,
										   &ez_peer->sw_key[0], LEN_PTK_KEK, 
										   pmk, &pmk_len);
							NdisZeroMemory(&ez_peer->this_band_info.pmk[0], EZ_PMK_LEN);
							NdisCopyMemory(&ez_peer->this_band_info.pmk[0], pmk, EZ_PMK_LEN);
							EZ_MEM_FREE( pmk);
							tag_count++;
						}
						break;
					}
				case EZ_TAG_OTHER_BAND_PMK :
					{
						unsigned char *encrypted_data;
						unsigned char *pmk;
						unsigned int pmk_len;
						encrypted_data = &pEid->Octet[EZ_TAG_DATA_OFFSET];
						data_len = pEid->Octet[EZ_TAG_LEN_OFFSET];
				
						EZ_MEM_ALLOC(NULL, &pmk, data_len+EZ_AES_KEY_ENCRYPTION_EXTEND);
						if (pmk) {
							/* AES */
							ezdev->driver_ops->AES_Key_Unwrap(ezdev,
											encrypted_data, data_len,
										   &ez_peer->sw_key[0], LEN_PTK_KEK, 
										   pmk, &pmk_len);
							NdisZeroMemory(&ez_peer->other_band_info.pmk[0], EZ_PMK_LEN);
							NdisCopyMemory(&ez_peer->other_band_info.pmk[0], pmk, EZ_PMK_LEN);
							EZ_MEM_FREE( pmk);
							tag_count++;
						}
						break;
					}

				case EZ_TAG_PSK_LEN :
					{
						ez_this_band_psk_len = pEid->Octet[EZ_TAG_DATA_OFFSET];
						tag_count++;
						break;
					}

				case EZ_TAG_OTHER_BAND_PSK_LEN :
					{
						ez_other_band_psk_len = pEid->Octet[EZ_TAG_DATA_OFFSET];
						tag_count++;
						break;
					}

				case EZ_TAG_PSK :
					{
						unsigned char *encrypted_data;
						unsigned char *psk;
						unsigned int psk_len;
						encrypted_data = &pEid->Octet[EZ_TAG_DATA_OFFSET];
						data_len = pEid->Octet[EZ_TAG_LEN_OFFSET];
							
						EZ_MEM_ALLOC(NULL, &psk, data_len + EZ_AES_KEY_ENCRYPTION_EXTEND);
						if (psk) {
							/* AES */
							ezdev->driver_ops->AES_Key_Unwrap(ezdev,
								encrypted_data, data_len,
								&ez_peer->sw_key[0], LEN_PTK_KEK, 
								psk, &psk_len);
							NdisZeroMemory(&ez_peer->this_band_info.psk[0], EZ_LEN_PSK);
							NdisCopyMemory(&ez_peer->this_band_info.psk[0], psk, ez_this_band_psk_len);
							EZ_MEM_FREE(psk);
							tag_count++;
						}
						break;
					}
				
				case EZ_TAG_OTHER_BAND_PSK :
					{
						unsigned char *encrypted_data;
						unsigned char *psk;
						unsigned int psk_len;
						encrypted_data = &pEid->Octet[EZ_TAG_DATA_OFFSET];
						data_len = pEid->Octet[EZ_TAG_LEN_OFFSET];
							
						EZ_MEM_ALLOC(NULL, &psk, data_len + EZ_AES_KEY_ENCRYPTION_EXTEND);
						if (psk) {
							/* AES */
							ezdev->driver_ops->AES_Key_Unwrap(ezdev,
								encrypted_data, data_len,
								&ez_peer->sw_key[0], LEN_PTK_KEK, 
								psk, &psk_len);
							NdisZeroMemory(&ez_peer->other_band_info.psk[0], EZ_LEN_PSK);
							NdisCopyMemory(&ez_peer->other_band_info.psk[0], psk, ez_other_band_psk_len);
							EZ_MEM_FREE(psk);
							tag_count++;
						}
						break;
					}
				case EZ_TAG_DEVICE_INFO	:
					{
						NdisCopyMemory(&ez_peer->device_info, &pEid->Octet[EZ_TAG_DATA_OFFSET],sizeof(device_info_t));
						ez_hex_dump("EZ_TAG_NETWORK_WEIGHT",ez_peer->device_info.network_weight, NETWORK_WEIGHT_LEN);
/*Redundant check same check is already present below 
in case of dup link deauth to other band is not present so removing this
*/
#if 0
						if (ez_is_loop_formed(ez_peer))
						{
								ezdev->driver_ops->ez_send_unicast_deauth
									(ezdev, ez_peer->mac_addr);
                            //todo: don't do deauth if only first bytes change between ) 0xE/0x0 (forced) but root same.
							break;
						}
#endif
						if (NdisEqualMemory(ez_peer->device_info.network_weight,ez_ad->device_info.network_weight,NETWORK_WEIGHT_LEN))
						{
								EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("SameWtRx Disconnect\n"));
							// if link is duplicate, then break both links
#if 0
							if(ez_peer->this_band_info.shared_info.link_duplicate)
#else
							if(ez_is_link_duplicate(ez_peer))
#endif
							{
								struct _ez_peer_security_info *ez_other_band_peer = NULL;
								ez_dev_t *other_band_ezdev = NULL;

								other_band_ezdev = ez_get_otherband_ezdev(ezdev);
								ez_other_band_peer = ez_get_other_band_ez_peer(ezdev,ez_peer);
								if(ez_other_band_peer)
								{
									EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("Disconnect the duplicate link.\n"));
									other_band_ezdev->driver_ops->ez_send_unicast_deauth
											(other_band_ezdev,ez_other_band_peer->mac_addr);
								}
							}
								ezdev->driver_ops->ez_send_unicast_deauth
									(ezdev, ez_peer->mac_addr);
                            //todo: don't do deauth if only first bytes change between ) 0xE/0x0 (forced) but root same.
							break;
						}
						if (ez_peer->device_info.network_weight[0] & BIT(7))
						{
							EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO,("will update weight here\n"));
							//ez_peer->device_info.network_weight[0] &= ~(BIT(7));
							check_for_weight = FALSE;
							//update_and_push_weight(ad, ez_peer, ez_peer->device_info.network_weight);
						}  
                        
						tag_count++;
						
						break;
					}
				case EZ_TAG_INTERFACE_INFO :
					{
						interface_info_tag_t *shared_info = (interface_info_tag_t *)&pEid->Octet[EZ_TAG_DATA_OFFSET];
						unsigned char link_duplicate;
						link_duplicate = ez_peer->this_band_info.shared_info.link_duplicate;
						NdisCopyMemory(&ez_peer->this_band_info.shared_info,&shared_info[0], sizeof(interface_info_tag_t));
						NdisCopyMemory(&ez_peer->other_band_info.shared_info,&shared_info[1], sizeof(interface_info_tag_t));
						ez_peer->this_band_info.shared_info.link_duplicate = link_duplicate;
						ez_peer->other_band_info.shared_info.link_duplicate = link_duplicate;
						tag_count++;
						break;
					}
				case EZ_TAG_COUSTOM_DATA :
					{
						ez_custom_data_cmd_t *p_custom_data;
						unsigned char datalen;
						p_custom_data = (ez_custom_data_cmd_t *)&pEid->Octet[EZ_TAG_DATA_OFFSET];
						datalen = (unsigned char )pEid->Octet[EZ_TAG_LEN_OFFSET];

						ez_notify_roam(ez_ad, ez_peer, FALSE, p_custom_data, datalen);
						Custom_EventHandle(ez_peer->ezdev, p_custom_data, datalen);
						
					}
					break; 

				case EZ_TAG_NON_EZ_CONFIG:
					{
						NdisCopyMemory(&ez_peer->non_ez_band_info[0], &pEid->Octet[EZ_TAG_DATA_OFFSET],sizeof(NON_EZ_BAND_INFO_TAG) * MAX_NON_EZ_BANDS);
						tag_count++;
						break;
					}
				
				case EZ_TAG_NON_EZ_PSK:
					{
						NdisCopyMemory(&ez_peer->non_ez_psk_info[0], &pEid->Octet[EZ_TAG_DATA_OFFSET],sizeof(NON_EZ_BAND_PSK_INFO_TAG) * MAX_NON_EZ_BANDS);
						tag_count++;
						break;
					}
				case EZ_TAG_NON_MAN_CONFIG:
					{
					//! Levarage from MP1.0 CL#170037
						NdisCopyMemory(&ez_peer->non_man_info, &pEid->Octet[EZ_TAG_DATA_OFFSET],sizeof(NON_MAN_INFO_TAG));
						tag_count++;
						break;
					}
				
				}
			}
		Length = Length + 2 + pEid->Len;  /* Eid[1] + Len[1]+ content[Len]*/
		pEid = (PEID_STRUCT)((unsigned char *)pEid + 2 + pEid->Len);
	
	}


	if (ez_is_triband_hook())
	{
		tag_count -=2;
	} else if(ez_ad->is_man_nonman) {
		//! Levarage form MP1.0 CL #170037
		tag_count -=1;
	}
	
	if (tag_count == EZ_UDATE_CONFIG_TAG_COUNT) {
		
		BOOLEAN need_process_action_frame = FALSE;
		//! weight update is not already going on, we can safely process the action frame.
#ifdef IF_UP_DOWN
		/*if all interfaces are not up then discard the action frame and trigger disconnection*/
		if (!ez_all_intf_up_hook(ez_ad))
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("Action frame received when all interfaces not up\n"));
			need_process_action_frame = FALSE;
		}
        else
#endif
        if (ez_ad->ez_connect_wait_ezdev->ez_security.weight_update_going_on == FALSE) {
			need_process_action_frame = TRUE;
		} else {

			//! if weight update is already going on, we must process action frames only from our WDL
			if (ez_peer->ezdev->ezdev_type == EZDEV_TYPE_AP)
			{
				if (MAC_ADDR_EQUAL(ez_peer->mac_addr,ez_ad->device_info.weight_defining_link.peer_mac) || 
						MAC_ADDR_EQUAL(ez_peer->other_band_info.shared_info.cli_mac_addr,ez_ad->device_info.weight_defining_link.peer_mac) )
					{
						need_process_action_frame = TRUE;
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("config update by CLI WDL in critical phase\n"));	
					} 				
			} else {
				if (MAC_ADDR_EQUAL(ez_peer->mac_addr,ez_ad->device_info.weight_defining_link.peer_mac) || 
					MAC_ADDR_EQUAL(ez_peer->other_band_info.shared_info.ap_mac_addr,ez_ad->device_info.weight_defining_link.peer_mac) )
				{
					need_process_action_frame = TRUE;
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("config update by CLI WDL in critical phase\n"));	
				}				
			}
		}

		if (need_process_action_frame)
		{
			push_and_update_config(ez_ad, ezdev,ez_peer,group_id_updated ? 0: check_for_weight, FALSE ,group_id_updated);
		} else {

			//! if weight update is going on and we received action frame from a non-wdl link, we should disconnect the link
			//! We reached uptill here means the action frame received have entirely different weight like the case below
			//! W1----->W2<------W3
			//! if for some reason both W1 and W3 are forcing weights with forced bit set, a network will be formed with a mix of W1 and W3
			//! hence disconnection is mandatory in this case
#if 0
			if(ez_peer->this_band_info.shared_info.link_duplicate)
#else
			if(ez_is_link_duplicate(ez_peer))
#endif
			{
				struct _ez_peer_security_info *ez_other_band_peer = NULL;
				ez_dev_t *other_band_ezdev = NULL;
			
				other_band_ezdev = ez_get_otherband_ezdev(ezdev);
				ez_other_band_peer = ez_get_other_band_ez_peer(ezdev,ez_peer);
				if(ez_other_band_peer)
				{
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("config update by non WDL duplicate link in critical phase.\n"));
					other_band_ezdev->driver_ops->ez_send_unicast_deauth
							(other_band_ezdev,ez_other_band_peer->mac_addr);
				}
			}
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("config update by non WDL in critical phase.\n"));
			ezdev->driver_ops->ez_send_unicast_deauth(ezdev, ez_peer->mac_addr);

		}
	}
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("<------ %s\n", __FUNCTION__));
}

void ez_peer_table_maintenance_hook(EZ_ADAPTER *ez_ad)
{
	ULONG now;
	struct _ez_peer_security_info *ez_peer = NULL;
	int i,j, sta_count= 0, flag=0;
	
	if (ez_ad == NULL) {
		printk("\n %s() ERROR ez_adapter is null\n", __FUNCTION__);
		return ;
	}
#ifndef IF_UP_DOWN
	if (ez_ad->band_count == 1) {
		if (!ez_is_triband_hook()) {
			printk("%s:is not triband mode, incorrect band_count==1.\n", __FUNCTION__);
			return ;
		}
	} else if (ez_ad->band_count != MAX_EZ_BANDS) {
		printk("%s:incorrect band_count=%d.\n", __FUNCTION__, ez_ad->band_count);
		return ;
	}
#else
	if (!ez_all_intf_up_hook(ez_ad)) {
		return;
	}
#endif
	NdisGetSystemUpTime(&now);

	for (i=0; i< MAX_EZ_BANDS; i++)
	{
		ez_dev_t *ezdev = &ez_ad->ez_band_info[i].ap_ezdev;
		ez_dev_t *apcli_ezdev = &ez_ad->ez_band_info[i].cli_ezdev;
		if(ezdev->wdev == NULL)
			continue;
			ez_peer = ez_peer_table_search_by_addr_hook(ezdev,ez_ad->device_info.weight_defining_link.peer_mac);
			if (ez_peer && (ez_peer->ezdev == ezdev))
			{
				if (RTMP_TIME_AFTER(now,ez_ad->device_info.weight_defining_link.ap_time_stamp 
					+ ez_ad->ez_wdl_missing_time * ezdev->os_hz))
				{
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,(" Weight defining CLI missing\n"));
					ez_hex_dump("peer AP", ez_ad->device_info.weight_defining_link.peer_ap_mac, 6);
					
					ez_peer->delete_in_differred_context = FALSE;
					ezdev->driver_ops->ez_send_unicast_deauth
						(ezdev,ez_ad->device_info.weight_defining_link.peer_mac);
				}
				
			}
			
			if(ezdev == ez_ad->device_info.weight_defining_link.ezdev && (ez_peer == NULL))
			{
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,(" Wt defining entry did not connect ezdev\n"));
				ASSERT(FALSE);
			}

			
		for (j=0; j<EZ_MAX_STA_NUM; j++)
		{
			if ((EZ_GET_EZBAND_BAND(ezdev->ez_ad,ezdev->ez_band_idx)->ez_peer_table[j].valid == TRUE) 
				&& (EZ_GET_EZBAND_BAND(ezdev->ez_ad,ezdev->ez_band_idx)->ez_peer_table[j].port_secured== FALSE))
			{
				sta_count++;
				if (RTMP_TIME_AFTER(now,
					EZ_GET_EZBAND_BAND(ezdev->ez_ad,ezdev->ez_band_idx)->ez_peer_table[j].creation_time 
					+ ez_ad->ez_peer_entry_age_out_time * ezdev->os_hz))
				{
					void *pEntry = NULL;
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,(" %s Deleting peer entry due to ageout\n", __FUNCTION__));
					ez_hex_dump("Mac",(PUCHAR)EZ_GET_EZBAND_BAND(ezdev->ez_ad,ezdev->ez_band_idx)->ez_peer_table[j].mac_addr, MAC_ADDR_LEN);
					pEntry = (VOID *)ezdev->driver_ops->ez_get_pentry(ezdev, EZ_GET_EZBAND_BAND(ezdev->ez_ad,ezdev->ez_band_idx)->ez_peer_table[j].mac_addr);
					if (pEntry){
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Mac table entry was formed, send unicast de-auth\n"));
						ez_peer = ez_peer_table_search_by_addr_hook(ezdev,
							EZ_GET_EZBAND_BAND(ezdev->ez_ad,ezdev->ez_band_idx)->ez_peer_table[j].mac_addr);
						if (ez_peer == NULL)
						{
							ASSERT(FALSE);
							return;
						}
						
						ez_peer->delete_in_differred_context = FALSE;
						 ezdev->driver_ops->ez_send_unicast_deauth
						 	(ezdev,ez_peer->mac_addr);
					} else {
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("No mac table entry present, just remove from ez_table\n"));
						ez_peer_table_delete(ezdev,EZ_GET_EZBAND_BAND(ezdev->ez_ad,ezdev->ez_band_idx)->ez_peer_table[j].mac_addr);
					}
					sta_count--;
					flag=1;
				}
			}
				
		}
		
		if (sta_count == 0 && flag==1)
		{
			flag = 0;
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,(" %s Updating connection permission\n", __FUNCTION__));
			ez_update_connection_permission_hook(ezdev,EZ_ALLOW_ALL);
		}
		
		if(apcli_ezdev->wdev == NULL)
			continue;

		if (apcli_ezdev->ez_security.internal_force_connect_bssid &&
				apcli_ezdev->ez_security.internal_force_connect_bssid_timeout == FALSE)
		{
			if (RTMP_TIME_AFTER(now, apcli_ezdev->ez_security.force_connect_bssid_time_stamp 
				+ ez_ad->ez_force_connect_bssid_time * ezdev->os_hz))
			{
				apcli_ezdev->ez_security.internal_force_connect_bssid= FALSE;
				apcli_ezdev->ez_security.internal_force_connect_bssid_timeout = TRUE;
			}
		}
		if (apcli_ezdev->ez_security.ez_scan_same_channel == TRUE 
			&& RTMP_TIME_AFTER(now,apcli_ezdev->ez_security.ez_scan_same_channel_timestamp + ez_ad->ez_scan_same_channel_time * ezdev->os_hz))
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,(" %s Timeout for scan same channel, scan all channels now.\n", __FUNCTION__));
			apcli_ezdev->ez_security.ez_scan_same_channel = FALSE;
		}

	}

	ez_apcli_check_roaming_status(ez_ad);
}


BOOLEAN ez_port_secured_hook(
	ez_dev_t *ezdev,
	UCHAR *peer_mac,
	unsigned char ap_mode)
{
	struct _ez_security *ez_sec_info = &ezdev->ez_security;
	device_info_to_app_t dev_info;	
	struct _ez_peer_security_info *ez_peer;
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	BOOLEAN allow_ap_connection = FALSE;
	BOOLEAN ret = TRUE;

	int check_for_weight = TRUE;
	enum_config_update_action_t config_update_action = ACTION_NOTHING;


	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("------> %s()\n", __FUNCTION__));



#ifdef EZ_API_SUPPORT
		if (ezdev->ez_security.ez_api_mode == CONNECTION_OFFLOAD)
		{
			ez_port_secured_for_connection_offload();
			return TRUE;
		}
#endif

	ez_peer = ez_peer_table_search_by_addr_hook(ezdev, peer_mac);
	if (ez_peer == NULL){
		
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
				("%s(): Ez_peer is NULL \n", __FUNCTION__));
		ez_hex_dump("MAC",peer_mac, MAC_ADDR_LEN);
		return FALSE;
	}
 	ez_peer->port_secured = TRUE;
	

	ezdev->ez_security.first_scan = FALSE;
	ezdev->ez_security.best_ap_rssi_threshold = 0;

	if (ap_mode) {
		ez_dev_t *cli_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev;
		if (cli_ezdev->wdev) {
			int irq_flags;
			EZ_IRQ_LOCK(&cli_ezdev->ez_security.ez_scan_pause_timer_lock, irq_flags);

			cli_ezdev->driver_ops->ez_cancel_timer
				(cli_ezdev,cli_ezdev->ez_security.ez_scan_pause_timer);
			EZ_IRQ_UNLOCK(&cli_ezdev->ez_security.ez_scan_pause_timer_lock, irq_flags);
		}

		//ezdev->driver_ops->ez_set_entry_apcli(ezdev, peer_mac, TRUE);
		/* Install PTK */
		ez_install_ptk(ez_peer, TRUE);

		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("connected peer capabilities --->%x\n", ez_peer->capability));
		if (EZ_GET_CAP_ALLOW_MERGE(ez_peer->capability)) {
			NdisZeroMemory(ez_sec_info->merge_peer_addr, MAC_ADDR_LEN);
			EZ_CLEAR_CAP_ALLOW_MERGE(ez_sec_info->capability);
			EZ_UPDATE_CAPABILITY_INFO(ezdev, EZ_CLEAR_ACTION, ALLOW_MERGE);
			if (ezdev->driver_ops->ez_is_timer_running
				(ezdev, ezdev->ez_security.ez_group_merge_timer))
			{
				ezdev->driver_ops->ez_cancel_timer(ezdev, 
					ezdev->ez_security.ez_group_merge_timer);

			}
		}
		ez_peer->port_secured = TRUE;
		{
			UCHAR sta_count=0;
			int i;
			for (i=0; i<EZ_MAX_STA_NUM;i++)
			{
				if (ez_ad->ez_band_info[ezdev->ez_band_idx].ez_peer_table[i].valid 
					&& ez_ad->ez_band_info[ezdev->ez_band_idx].ez_peer_table[i].port_secured == FALSE
					&& ez_ad->ez_band_info[ezdev->ez_band_idx].ez_peer_table[i].ezdev == ezdev) {
					sta_count++;
					allow_ap_connection = TRUE;
				}
			}
			if (sta_count == 0)
			{
					ez_update_connection_permission_hook(ezdev,EZ_ALLOW_ALL);
			}
			else
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("-> don't allow all connections as still some connection pending on AP\n"));
		}

		{
			if (ez_peer->this_band_info.shared_info.link_duplicate)
			{
				struct _ez_peer_security_info *other_band_cli_peer = ez_get_other_band_ez_peer(ezdev, ez_peer);
				ez_hex_dump("EZ_PEER MAC = ", peer_mac, MAC_ADDR_LEN);
				ezdev->driver_ops->ez_mark_entry_duplicate(ezdev, peer_mac);
				if (other_band_cli_peer != NULL) {
					other_band_cli_peer->this_band_info.shared_info.link_duplicate = TRUE;
					other_band_cli_peer->other_band_info.shared_info.link_duplicate = TRUE;
					other_band_cli_peer->ezdev->driver_ops->ez_mark_entry_duplicate
										(other_band_cli_peer->ezdev, other_band_cli_peer->mac_addr);
				} else {
					ASSERT(FALSE);
				}
			}
		}
	}
	else {
		ez_dev_t *ap_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev;
		int irq_flags ;
		ezdev->ez_security.keep_finding_provider = FALSE;
		ezdev->driver_ops->ez_cancel_timer
			(ezdev, ezdev->ez_security.ez_scan_timer);
		//ezdev->driver_ops->ez_cancel_timer
		//	(ezdev, ezdev->ez_security.ez_stop_scan_timer);

		EZ_IRQ_LOCK(&ezdev->ez_security.ez_scan_pause_timer_lock, irq_flags);
		ezdev->driver_ops->ez_cancel_timer
			(ezdev, ezdev->ez_security.ez_scan_pause_timer);

		ezdev->driver_ops->apcli_stop_auto_connect
				(ezdev, FALSE);
		EZ_IRQ_UNLOCK(&ezdev->ez_security.ez_scan_pause_timer_lock, irq_flags);

		COPY_MAC_ADDR(ezdev->bssid,peer_mac);
		/* Install Key */
		ez_install_ptk(ez_peer, FALSE);
		ez_apcli_install_gtk(ez_peer);
		
		EZ_SET_CAP_CONNECTED(ap_ezdev->ez_security.capability);
		ap_ezdev->driver_ops->UpdateBeaconHandler(ap_ezdev, IE_CHANGE);

		//! clear off group merge related params
		NdisZeroMemory(ez_sec_info->merge_peer_addr, MAC_ADDR_LEN);
		EZ_CLEAR_CAP_ALLOW_MERGE(ez_sec_info->capability);
		EZ_UPDATE_APCLI_CAPABILITY_INFO(ez_ad, EZ_CLEAR_ACTION, ALLOW_MERGE, ezdev->ez_band_idx);
		if ( ezdev->driver_ops->ez_is_timer_running
			(ezdev, ezdev->ez_security.ez_group_merge_timer))
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Group Merge completed, cancel timer here\n"));
			ezdev->driver_ops->ez_cancel_timer
     		(ezdev,ezdev->ez_security.ez_group_merge_timer);

		}
		
		ezdev->ez_security.delay_disconnect_count = 0;

		if(!MAC_ADDR_EQUAL(ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid,ZERO_MAC_ADDR))
		{
			NdisZeroMemory(ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid,MAC_ADDR_LEN);
			NdisZeroMemory(ezdev->ez_security.ez_ap_roam_blocked_mac,MAC_ADDR_LEN);
			ez_update_connection_permission_hook(ezdev,EZ_DEQUEUE_PERMISSION);
		}
#ifdef EZ_DUAL_BAND_SUPPORT
		if (ez_apcli_is_link_duplicate(ezdev,ez_peer->mac_addr))//(wdev->ez_security.internal_force_connect_bssid)
		{
			
			ezdev->ez_security.internal_force_connect_bssid = FALSE;
			ezdev->ez_security.this_band_info.shared_info.link_duplicate = TRUE;
#if 0
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("wdev_idx=0x%x, wdev_type=0x%x, func_idx=0x%x : Duplicate Link made\n",
				wdev->wdev_idx,wdev->wdev_type,wdev->func_idx));
#endif
		} 
		else
		{
			if (check_for_weight == TRUE)
			{
				if (ez_is_weight_same_mod(ez_ad->device_info.network_weight,ez_peer->device_info.network_weight))
				{
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("Wt same when link is not duplicate!!! Loop!!!\n"));
					ezdev->ez_security.internal_force_connect_bssid_timeout = FALSE;
					ezdev->driver_ops->ez_send_unicast_deauth
						(ezdev,ez_peer->mac_addr);
					return FALSE;
				}
			}

		}
		ezdev->ez_security.internal_force_connect_bssid_timeout = FALSE;
		COPY_MAC_ADDR(ezdev->ez_security.this_band_info.cli_peer_ap_mac, ez_peer->mac_addr);
				
		
		COPY_MAC_ADDR(ezdev->ez_security.this_band_info.cli_peer_ap_mac, ez_peer->mac_addr);
		
		ez_inform_all_interfaces(ez_ad, ezdev, ACTION_UPDATE_DUPLICATE_LINK_ENTRY);
#endif
		ezdev->ez_security.ez_scan_same_channel = FALSE;

		//! check if configuration update and push is required
		config_update_action = push_and_update_config(ez_ad, ezdev, ez_peer, check_for_weight,TRUE,FALSE);
		//! if configuration push os required, send an action frame to connected AP
		if (config_update_action == ACTION_PUSH)
		{

//! Levarage from MP1.0 CL 170192
			updated_configs_t *updated_configs = NULL;
			NDIS_STATUS NStatus;			
#ifdef EZ_PUSH_BW_SUPPORT
			BOOLEAN this_band_changed = FALSE;
			BOOLEAN other_band_changed = FALSE;
			ez_dev_t *other_band_ap_ezdev = NULL;
			//if( ((PRTMP_ADAPTER)(wdev->sys_handle))->push_bw_config )
				ez_chk_bw_config_different(ezdev,ez_peer,&this_band_changed,&other_band_changed);
#endif
//! Levarage from MP1.0 CL 170192
			NStatus = EZ_MEM_ALLOC(NULL, (UCHAR **)&updated_configs, sizeof(updated_configs_t));
        		if(NStatus != NDIS_STATUS_SUCCESS)
        		{
                		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%s() allocate memory failed \n", __FUNCTION__));
						ASSERT(FALSE);
                		return FALSE;
       			}

			ezdev->driver_ops->ez_restore_cli_config(ezdev);

			//! initialize a local structure that will hold all the inforation to be pushed to peer AP
//! Levarage from MP1.0 CL 170192
			ez_init_updated_configs_for_push(updated_configs, ezdev);
			//! send updated configurations to peer 
//! Levarage from MP1.0 CL 170192
			if(send_action_update_config(ez_ad, ez_peer, ezdev, updated_configs, TRUE, FALSE) == FALSE)
			{
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,
					("%s(): Error: Could not send action frame for push. Disconnect\n", __FUNCTION__));
				if(ez_peer) {
					ez_peer->delete_in_differred_context = TRUE;
					ezdev->driver_ops->ez_send_unicast_deauth(ezdev,ez_peer->mac_addr);
				}
				ez_update_connection_permission_hook(ezdev,EZ_ALLOW_ALL);
			}

			ezdev->driver_ops->ez_restore_channel_config(ap_ezdev);
			other_band_ap_ezdev = ez_get_otherband_ap_ezdev(ezdev);
			if (other_band_ap_ezdev){
				other_band_ap_ezdev->driver_ops->ScanTableInit(other_band_ap_ezdev);
				other_band_ap_ezdev->driver_ops->UpdateBeaconHandler(other_band_ap_ezdev, IE_CHANGE);
			}

			
			//update peer info to new config			
			if( ez_ad->push_bw_config )
				update_peer_record(ez_ad, ezdev, this_band_changed, other_band_changed);
//! Levarage from MP1.0 CL#170037
			if(!ez_is_triband_hook() && !ez_ad->is_man_nonman)
			{
				ez_prepare_device_info_to_app(ez_ad, &dev_info);
				dev_info.is_push = 1;
				ezdev->driver_ops->RtmpOSWrielessEventSendExt(ezdev, RT_WLAN_EVENT_CUSTOM, OID_WH_EZ_MAN_DEAMON_EVENT,
								NULL, (void *)&dev_info, sizeof(device_info_to_app_t));
			
			}

			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Initiate New scan after push\n"));
			ez_initiate_new_scan_hook(ez_ad);
//! Levarage from MP1.0 CL 170192
			EZ_MEM_FREE(updated_configs);
		}
		else if (config_update_action == ACTION_NOTHING)
		{
			if(ezdev->ez_security.this_band_info.shared_info.link_duplicate == TRUE)
			{
				struct _ez_peer_security_info *ez_other_band_peer = ez_get_other_band_ez_peer(ezdev,ez_peer);
				if(ez_other_band_peer == NULL)
				{
					ASSERT(FALSE);
					return FALSE;
				}
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s:duplicateLink: Update the node Number\n", __FUNCTION__));
				NdisCopyMemory(&ez_peer->device_info.ez_node_number,&ez_other_band_peer->device_info.ez_node_number,sizeof(EZ_NODE_NUMBER));
			}
			else
				ez_restore_node_number(&ez_peer->device_info.ez_node_number);
			
			ez_update_connection_permission_hook(ezdev,EZ_ALLOW_ALL);
		}	
	}

	ez_inform_all_interfaces(ez_ad, ezdev, ACTION_UPDATE_CONFIG_STATUS);
	
	if (ap_mode) {
		ezdev->driver_ops->RtmpOSWrielessEventSendExt(ezdev, RT_WLAN_EVENT_CUSTOM, IW_WH_EZ_MY_AP_HAS_APCLI,
						    NULL, NULL, 0);
	}
	else {
		ezdev->driver_ops->RtmpOSWrielessEventSendExt(ezdev, RT_WLAN_EVENT_CUSTOM, IW_WH_EZ_MY_APCLI_CONNECTED,
						    NULL, NULL, 0);
	}

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("<------ %s()\n", __FUNCTION__));

    return ret;
}

BOOLEAN check_best_ap_rssi_threshold_hook(ez_dev_t *ezdev, char rssi)
{
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	if(rssi >= ez_ad->best_ap_rssi_threshld[ezdev->ez_security.best_ap_rssi_threshold - 1])
	{
		return TRUE;
	}
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%s:: RSSI = %d, Threshold = %d, REJECT CONNECTION\n", __FUNCTION__,
		rssi, ez_ad->best_ap_rssi_threshld[ezdev->ez_security.best_ap_rssi_threshold - 1]));
	return FALSE;

}

void ez_initiate_new_scan_hook(EZ_ADAPTER *ez_ad)
{

	//PRTMP_ADAPTER pAd = ad_obj;
	UCHAR i=0;
	for (i=0; i < MAX_EZ_BANDS; i++)
	{
		ez_dev_t* ezdev = &ez_ad->ez_band_info[i].cli_ezdev;
		ezdev->ez_security.first_scan = TRUE;
	}
}

void ez_handle_peer_disconnection_hook(ez_dev_t *ezdev, unsigned char * mac_addr)
{
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	int irq_flags;
	if (!IS_SINGLE_CHIP_DBDC(ez_ad))
		EZ_IRQ_LOCK(&ez_ad->ez_handle_disconnect_lock, irq_flags);	

	if(ezdev->ezdev_type == EZDEV_TYPE_AP){
		struct _ez_peer_security_info *ez_peer = ez_peer_table_search_by_addr_hook(ezdev, mac_addr);
		BOOLEAN last_port_secured_state;

		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("AP's Peer down\n"));
		if (ez_peer == NULL)
		{
#ifdef DUAL_CHIP	
			if (!IS_SINGLE_CHIP_DBDC(ez_ad))
				EZ_IRQ_UNLOCK(&ez_ad->ez_handle_disconnect_lock, irq_flags);	
#endif
			return;
		}

		last_port_secured_state = ez_peer->port_secured;

		if (ez_peer->this_band_info.shared_info.link_duplicate)
		{
			struct _ez_peer_security_info *ez_other_band_peer = ez_get_other_band_ez_peer(ezdev, ez_peer);

			// even though entry would be deleted later, just clearing anyway
			ez_peer->this_band_info.shared_info.link_duplicate = FALSE;
			ez_peer->other_band_info.shared_info.link_duplicate = FALSE;


			ezdev->driver_ops->ez_reset_entry_duplicate(ezdev, ez_peer->mac_addr);
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("ez_band_idx=0x%x, ezdev_type=0x%x, func_idx=0x%x : Clear self connected cli [%02x-%02x-%02x-%02x-%02x-%02x] mac_entry as duplicate link\n",
				ezdev->ez_band_idx,ezdev->ezdev_type,ezdev->ez_band_idx, 
				ez_peer->mac_addr[0],ez_peer->mac_addr[1],ez_peer->mac_addr[2],
				ez_peer->mac_addr[3],ez_peer->mac_addr[4],ez_peer->mac_addr[5]));

			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("duplicate cli link down, inform others\n"));

			if (ez_other_band_peer != NULL) {
				ez_other_band_peer->this_band_info.shared_info.link_duplicate = FALSE;
				ez_other_band_peer->other_band_info.shared_info.link_duplicate = FALSE;				
				ez_other_band_peer->ezdev->driver_ops->ez_reset_entry_duplicate(ez_other_band_peer->ezdev, 
					ez_other_band_peer->mac_addr);
			} else {
				ASSERT(FALSE);
			}

			ez_hex_dump("MAC Addr", mac_addr, 6);
			ez_hex_dump("weight_defining_link", ez_ad->device_info.weight_defining_link.peer_mac, 6);
			if (NdisEqualMemory(mac_addr, ez_ad->device_info.weight_defining_link.peer_mac,MAC_ADDR_LEN))
			{
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("weight defining link down, switch to other band\n"));
				ez_switch_wdl_to_other_band(ezdev, ez_other_band_peer);
			}

		} else {
		
			if (NdisEqualMemory(mac_addr, ez_ad->device_info.weight_defining_link.peer_mac,MAC_ADDR_LEN))
			{
					ez_peer->port_secured = FALSE; // Mark port_secured FALSE to avoid sending update_action frame to this device
					NdisGetSystemUpTime(&ez_peer->creation_time); // to avoid peer ageout logic to interfere
					
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("weight defining link down, update weight\n"));
					update_and_push_weight(ezdev, NULL, NULL);
					
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Initiate New scan after update wt\n"));
					ez_initiate_new_scan_hook(ezdev->ez_ad);
			}

		}
		if (last_port_secured_state != TRUE)
			ez_update_connection_hook(ezdev);

	} else {
		NdisZeroMemory(ezdev->ez_security.this_band_info.cli_peer_ap_mac, MAC_ADDR_LEN);
		NdisZeroMemory(ezdev->bssid, MAC_ADDR_LEN);
		NdisZeroMemory(ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev.ez_security.this_band_info.cli_peer_ap_mac, MAC_ADDR_LEN);
		//dump_stack();
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("CLI Link Down\n"));

		ez_cancel_loop_chk(ezdev);
		ezdev->ez_security.this_band_info.non_easy_connection = FALSE;

		if (ezdev->ez_security.this_band_info.shared_info.link_duplicate)
		{
			interface_info_t other_band_config;

			NdisZeroMemory(&other_band_config, sizeof(interface_info_t));
			ez_get_other_band_info(ezdev, &other_band_config);
			ezdev->ez_security.this_band_info.shared_info.link_duplicate = FALSE;
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("ez_band_idx=0x%x, ezdev_type=0x%x, func_idx=0x%x : duplicate link down, inform others\n",
				ezdev->ez_band_idx,ezdev->ezdev_type,ezdev->ez_band_idx));
			ez_inform_all_interfaces(ezdev->ez_ad,ezdev, ACTION_UPDATE_DUPLICATE_LINK_ENTRY);
			ez_hex_dump("MAC Addr", mac_addr, 6);
			ez_hex_dump("weight_defining_link", ez_ad->device_info.weight_defining_link.peer_mac, 6);

			if (NdisEqualMemory(mac_addr, ez_ad->device_info.weight_defining_link.peer_mac,MAC_ADDR_LEN))
			{
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("weight defining link down, switch to other band\n"));
				ez_switch_wdl_to_other_band(ezdev, &other_band_config);
			}
		} else {
			if (NdisEqualMemory(mac_addr,ez_ad->device_info.weight_defining_link.peer_mac,MAC_ADDR_LEN))
			{
				interface_info_t other_band_info;
				BOOLEAN other_band_active = FALSE;
				//PRTMP_ADAPTER ad = ezdev->ez_ad;
				struct _ez_peer_security_info *ez_other_band_peer = NULL;
				ez_dev_t  *other_band_ezdev = ez_get_otherband_cli_ezdev(ezdev);
				struct _ez_peer_security_info *ez_peer = ez_peer_table_search_by_addr_hook(ezdev, mac_addr);

					if (ez_peer)
						ez_peer->port_secured = FALSE; // Mark port_secured FALSE to avoid sending update_action frame to this device

					other_band_active = ez_get_other_band_info(ezdev, &other_band_info);
						
					if (other_band_active && !NdisEqualMemory(other_band_info.cli_peer_ap_mac,ZERO_MAC_ADDR,MAC_ADDR_LEN))
					{
						ez_other_band_peer = ez_peer_table_search_by_addr_hook(other_band_ezdev,other_band_info.cli_peer_ap_mac);
						if (ez_other_band_peer == NULL)
						{
							EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("weight defining link down, Other Band connected to non Easy, switch WDL to other band\n"));
							update_and_push_weight(other_band_ezdev, other_band_info.cli_peer_ap_mac, NULL);
						} else {
							EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("weight defining link down, update weight\n"));
							update_and_push_weight(ezdev, NULL, NULL);
						}
					} else {
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("weight defining link down, update weight\n"));
						update_and_push_weight(ezdev, NULL, NULL);
					}

			}
			else if (ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.support_ez_setup == 0) {
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
					("ez_handle_peer_disconnection: Non Easy Connection fail\n"));
				ez_update_connection_permission_hook(ezdev,EZ_ALLOW_ALL);
			}	
		}

	}
	ez_peer_table_delete(ezdev,mac_addr);
	if (!IS_SINGLE_CHIP_DBDC(ez_ad))
		EZ_IRQ_UNLOCK(&ez_ad->ez_handle_disconnect_lock, irq_flags);
}


void ez_set_ap_fallback_context_hook(ez_dev_t *ezdev, BOOLEAN fallback, unsigned char fallback_channel)
{
	if(ezdev){
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("\nez_set_ap_fallback_context: ezdev idx %x, type %x, fallback %x, chan %d\n",
			ezdev->ez_band_idx, ezdev->ezdev_type, fallback, fallback_channel));
		ezdev->ez_security.ap_did_fallback = fallback;
		ezdev->ez_security.fallback_channel = fallback_channel;
	}
}


struct _ez_peer_security_info *ez_peer_table_search_by_addr_hook(
	ez_dev_t *ezdev,
	unsigned char *addr)
{
	int i;
	struct _ez_peer_security_info *ez_peer;
	int irq_flags;

	ez_peer = NULL;
	if (ezdev->wdev)
	{
		EZ_IRQ_LOCK(ezdev->ez_peer_table_lock, irq_flags);
		for (i = 0; i < EZ_MAX_STA_NUM; i++) {		
			if (EZ_GET_EZBAND_BAND(ezdev->ez_ad,ezdev->ez_band_idx)->ez_peer_table[i].valid &&
				NdisEqualMemory(addr, EZ_GET_EZBAND_BAND(ezdev->ez_ad,ezdev->ez_band_idx)->ez_peer_table[i].mac_addr, MAC_ADDR_LEN)) {
				ez_peer = &EZ_GET_EZBAND_BAND(ezdev->ez_ad,ezdev->ez_band_idx)->ez_peer_table[i];
				break;
			}
		}
		EZ_IRQ_UNLOCK(ezdev->ez_peer_table_lock, irq_flags);
	}
	return ez_peer;
}


BOOLEAN ez_is_roaming_ongoing_hook(EZ_ADAPTER *ez_ad)
{
		int i=0;
		for (i=0; i < MAX_EZ_BANDS; i++) {
			ez_dev_t *cli_ezdev = &ez_ad->ez_band_info[i].cli_ezdev;
			if (cli_ezdev->wdev)
			{
				int apcli_enable = cli_ezdev->driver_ops->get_apcli_enable(cli_ezdev);
				if (apcli_enable == 0 || MAC_ADDR_EQUAL(cli_ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid,ZERO_MAC_ADDR))
					continue;
				else
					return TRUE;
			}
		}	
	return FALSE;
}


BOOLEAN ez_is_triband_hook(void)
{
	if(ez_adapter) {
		if (ez_adapter->band_count == 1 && ez_adapter->non_ez_band_count == 2) {
			return TRUE;
		}
	}
	return FALSE;
}


void * ez_get_otherband_ad_hook(ez_dev_t *ezdev)
{
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	return ez_ad->ez_band_info[ezdev->ez_band_idx ^ 1].pAd;
}


unsigned short ez_check_for_ez_enable_hook(
	void *msg,
	unsigned long msg_len
	)
{	
	unsigned char tag_count;
	FRAME_802_11 *Fr;
	unsigned char *Ptr;
	EID_STRUCT *pEid;
	unsigned long Length;
	unsigned char tag;
	unsigned char data_len;
	unsigned char capability0;
	unsigned int capability;
	
	tag_count = 0;
	Length = 0;
	Fr = (PFRAME_802_11)msg;
	
	Ptr = Fr->Octet;
	Length += LENGTH_802_11;
		
	pEid = (PEID_STRUCT) Ptr;

	/* get variable fields from payload and advance the pointer*/
	while ((Length + 2 + pEid->Len) <= msg_len)    
	{
		switch(pEid->Eid)
		{
			case IE_VENDOR_SPECIFIC:				
				if (NdisEqualMemory(&pEid->Octet[0], &mtk_oui[0], MTK_OUI_LEN)) {
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO, 
						("%s() - Found MTK Proprietary OUI.\n", __FUNCTION__));
					capability0 = pEid->Octet[MTK_OUI_LEN];
					if (capability0 & MTK_VENDOR_EASY_SETUP) {
						tag = pEid->Octet[EZ_TAG_OFFSET];
						data_len = pEid->Octet[EZ_TAG_LEN_OFFSET];
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO, 
							("%s() - tag = %d, data_len = %d\n", 
							__FUNCTION__, tag, data_len));
#ifdef EZ_NETWORK_MERGE_SUPPORT
						if (tag == EZ_TAG_CAPABILITY_INFO) {
						//! aditional capability tag is added in probe request so that AP is aware of CLIs group merge capability
							if (data_len == EZ_CAPABILITY_LEN) {
								NdisCopyMemory(&capability, 
									&pEid->Octet[EZ_TAG_DATA_OFFSET], 
									EZ_CAPABILITY_LEN);
								
								tag_count++;
							}
#endif
							else
								return EZ_STATUS_CODE_INVALID_DATA;
						}							
					}
				}
				break;
			default:
				break;
		}

		Length = Length + 2 + pEid->Len;  /* Eid[1] + Len[1]+ content[Len]*/
		pEid = (PEID_STRUCT)((unsigned char *)pEid + 2 + pEid->Len);
	}

	if (tag_count == 1) {
		return EZ_STATUS_CODE_SUCCESS;
	}
	else
		return EZ_STATUS_CODE_INVALID_DATA;
}

/*
=====================================================================
Description:
	This function is used for 
	- handle internet status OID 
	- Update beacon accordingly
	
=====================================================================
*/


void ez_internet_msghandle_hook(ez_dev_t *ezdev, p_internet_command_t p_internet_command)
{
	if ((ezdev->ezdev_type == EZDEV_TYPE_AP))
	{
		ezdev->ez_security.go_internet = (unsigned char)p_internet_command->Net_status;

		if(p_internet_command->Net_status == 1)
		{
			EZ_UPDATE_CAPABILITY_INFO(ezdev, EZ_SET_ACTION, INTERNET);
		}
		else
		{
			EZ_UPDATE_CAPABILITY_INFO(ezdev, EZ_CLEAR_ACTION, INTERNET);
		}				
		ez_inform_all_interfaces(ezdev->ez_ad,ezdev,ACTION_UPDATE_INTERNET_STATUS);

	}
	
}

void ez_custom_data_handle_hook(EZ_ADAPTER *ez_ad,ez_custom_data_cmd_t *p_custom_data, int length)
{
	ez_notify_roam(ez_ad, NULL,FALSE,p_custom_data,length);
}
void ez_acquire_lock_hook(EZ_ADAPTER *ez_ad, ez_dev_t *ezdev,unsigned char lock_id)
{
	int flags =0;
	if (ez_ad)
	{
		if(lock_id == MLME_SYNC_LOCK)
		{
			EZ_IRQ_LOCK(&ez_ad->ez_mlme_sync_lock,flags)
		} else if (lock_id == BEACON_UPDATE_LOCK){
			EZ_IRQ_LOCK(&ez_ad->ez_beacon_update_lock,flags)
		} else if (lock_id == EZ_MINIPORT_LOCK){
				EZ_IRQ_LOCK(&ez_ad->ez_miniport_lock,flags)
		}
	} else if (ezdev){
		if(lock_id == SCAN_PAUSE_TIMER_LOCK)
		{
			EZ_IRQ_LOCK(&ezdev->ez_security.ez_scan_pause_timer_lock,flags)
		}

	}
}

void ez_release_lock_hook(EZ_ADAPTER *ez_ad, ez_dev_t *ezdev,unsigned char lock_id)
{
	//int flags;
	if (ez_ad)
	{
		if(lock_id == MLME_SYNC_LOCK)
		{
			EZ_IRQ_UNLOCK(&ez_ad->ez_mlme_sync_lock, NULL)
		} else if (lock_id == BEACON_UPDATE_LOCK){
			EZ_IRQ_UNLOCK(&ez_ad->ez_beacon_update_lock, NULL)
		} else if (lock_id == EZ_MINIPORT_LOCK){
				EZ_IRQ_UNLOCK(&ez_ad->ez_miniport_lock, NULL)
		}
	}
	else if (ezdev){
			if(lock_id == SCAN_PAUSE_TIMER_LOCK)
			{
				EZ_IRQ_UNLOCK(&ezdev->ez_security.ez_scan_pause_timer_lock, NULL)
			}
		}
}

BOOLEAN ez_is_weight_same_hook(ez_dev_t *ezdev, UCHAR *peer_weight)
{
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	return ez_is_weight_same_mod(ez_ad->device_info.network_weight,peer_weight);
}
BOOLEAN ez_is_other_band_mlme_running_hook(ez_dev_t *ezdev)
{
	ez_dev_t *other_band_ezdev = ez_get_otherband_ezdev(ezdev);
	//printk("received ezdev = %p\n", ezdev);
	if (other_band_ezdev){
		//printk("chipop addr =%p\n", (void *)other_band_ezdev->driver_ops->is_mlme_running);
		return other_band_ezdev->driver_ops->is_mlme_running(other_band_ezdev); 
	} else {
		return FALSE;
	}
}
void ez_triband_insert_tlv_hook(EZ_ADAPTER *ez_ad, 
	unsigned int tag_ID, UCHAR * buffer, ULONG* tag_len)
{
	if (tag_ID == EZ_TAG_CAPABILITY_INFO)
	{
		unsigned int capability;
		capability = cpu2be32(ez_ad->ez_band_info[0].cli_ezdev.ez_security.capability);
		ez_insert_tlv(EZ_TAG_CAPABILITY_INFO,
			(unsigned char *)&capability,
			EZ_CAPABILITY_LEN,
			buffer,
			tag_len);
	} else if (tag_ID== EZ_TAG_NON_EZ_BEACON)
	{
		ez_insert_tlv(EZ_TAG_NON_EZ_BEACON,
			NULL,
			0,
			buffer,
			tag_len);

	}
}

void increment_best_ap_rssi_threshold_hook(ez_dev_t *ezdev)
{
	struct _ez_security *ez_security = &ezdev->ez_security;
	ez_security->best_ap_rssi_threshold++;
	if (ez_security->best_ap_rssi_threshold > BEST_AP_RSSI_THRESHOLD_LEVEL_MAX)
	{
		ez_security->best_ap_rssi_threshold = BEST_AP_RSSI_THRESHOLD_LEVEL_MAX; 
	}
	
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%s:: ez_security->best_ap_rssi_threshold = %d\n", __FUNCTION__, ez_security->best_ap_rssi_threshold));
}

void ez_ap_peer_beacon_action_hook(ez_dev_t *ezdev, unsigned char * mac_addr, int peer_capability)
{
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	if (MAC_ADDR_EQUAL(mac_addr, ez_ad->device_info.weight_defining_link.peer_ap_mac))
	{
			NdisGetSystemUpTime(&ez_ad->device_info.weight_defining_link.ap_time_stamp);
			if (!EZ_GET_CAP_CONNECTED(peer_capability) 
				&& !NdisEqualMemory(ez_ad->device_info.weight_defining_link.peer_ap_mac,
					ez_ad->device_info.weight_defining_link.peer_mac,MAC_ADDR_LEN))
			{
				EZ_DEBUG(DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_ERROR,("My WDL CLI is not connected\n"));
				ezdev->driver_ops->ez_send_unicast_deauth(ezdev,ez_ad->device_info.weight_defining_link.peer_mac);
			}
	}
}


/*
Determines whether Rx group packet is to be dropped by ApCli interface
for duplicate CLI links with NonEz AP
*/
BOOLEAN ez_apcli_rx_grp_pkt_drop_hook(ez_dev_t *ezdev,UCHAR *pDestAddr)
{
	struct _ez_security *ez_sec_info = &ezdev->ez_security;
	unsigned char is_5g_band = 0;
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;

	if( !(pDestAddr && MAC_ADDR_IS_GROUP(pDestAddr) ) )
		return FALSE;

#if 0
	if( (ez_sec_info->this_band_info.non_easy_connection == TRUE) && 
		ez_sec_info->this_band_info.shared_info.link_duplicate && 
		(!( 
//! Levarage from MP1.0 CL#170037
#if defined (CONFIG_WIFI_PKT_FWD) 
	(*ezdev->channel >= H_CHANNEL_BIGGER_THAN) || 
#endif
	(*ezdev->channel > 14) ))){ // just chan 14 check should be enough but kept as in other places
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("ez_apcli_rx_grp_pkt_drop: Grp packet on non2g duplicate link !!!\n"));
		return TRUE;
	}
#else
	if( (*ezdev->channel >= H_CHANNEL_BIGGER_THAN) || (*ezdev->channel > 14) )
		is_5g_band = TRUE;

	if( (ez_sec_info->this_band_info.non_easy_connection == TRUE) && 
		ez_sec_info->this_band_info.shared_info.link_duplicate )
	{
		if((ez_ad->default_group_data_band == EZ_DROP_GROUP_DATA_BAND24G) && (is_5g_band == FALSE))
		{
			return TRUE;
		}
		else if ((ez_ad->default_group_data_band == EZ_DROP_GROUP_DATA_BAND5G) && (is_5g_band == TRUE))
		{
			return TRUE;
		}
	}
#endif
	return FALSE;
}

int  ez_handle_send_packets_hook(ez_dev_t *ezdev, PNDIS_PACKET pPacket)
{
	
	UCHAR *pDestAddr = GET_OS_PKT_DATAPTR(pPacket);
	if((ezdev->driver_ops->ez_is_timer_running(ezdev, ezdev->ez_security.ez_loop_chk_timer)) 
		&& (ezdev->ez_security.first_loop_check)
		&&  (MAC_ADDR_IS_GROUP(pDestAddr))){ // only source runs timer, so role chk not required
		return 0;
	}
	
	//hex_dump("RTMPSendPackets: Eth Hdr: ",pDestAddr,14);
	
	if(pDestAddr && MAC_ADDR_IS_GROUP(pDestAddr)){ // group packet
		if( ez_apcli_tx_grp_pkt_drop_hook(ezdev, (struct sk_buff *)pPacket) == TRUE)
		{
			return 0;
		}			
	}
	return 1;
}
void send_delay_disconnect_to_peers_hook(ez_dev_t *ap_ezdev)
{
	int index = 0;
	EZ_ADAPTER *ez_ad = ap_ezdev->ez_ad;
	struct _ez_peer_security_info * ez_peer_table = ez_ad->ez_band_info[ap_ezdev->ez_band_idx].ez_peer_table;
	
	//! first send an action frame to EZ peers so that they do not disconnect
	for (index = 0; index < EZ_MAX_STA_NUM; index ++)
	{
		if (ez_peer_table[index].port_secured && (ez_peer_table[index].ezdev == ap_ezdev)){
			send_action_delay_disconnect(ez_ad, ap_ezdev, &ez_peer_table[index], EZ_DELAY_DISCONNECT_FOR_PBC);
		}	
	}
	
}

BOOLEAN ez_sta_rx_pkt_handle_hook(ez_dev_t *ezdev, UCHAR *pPayload, UINT MPDUtotalByteCnt)
{

	if((ezdev->driver_ops->ez_is_timer_running(ezdev, ezdev->ez_security.ez_loop_chk_timer)) 
		&& (ezdev->ez_security.first_loop_check)){
       EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("wdev_idx:0x%x=> Drop Rx pkt as loop check trigerred by this source\n",ezdev->ez_band_idx));
	    return TRUE;
	}
	if(ezdev->ez_security.loop_chk_info.loop_chk_role == DEST){
		
		if(NdisCmpMemory(pPayload,BROADCAST_ADDR,MAC_ADDR_LEN) != 0){
			return FALSE;
		}

		// data pkt chk
		if( (MPDUtotalByteCnt >= 40) &&
			(pPayload[12] == IPV4TYPE[0]) && 
			(pPayload[13] == IPV4TYPE[1]) && 
			(pPayload[14] == 0x45) &&
			(pPayload[23] == 0xFD))
		{
			//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("wdev_idx=0x%x, wdev_type=0x%x, func_idx=0x%x : DEST role APCLi got Rx Pkt\n",
			//	wdev->wdev_idx,wdev->wdev_type,wdev->func_idx));
			if(ez_is_loop_pkt_rcvd_hook(ezdev,&pPayload[6], &pPayload[34]) == TRUE){
				//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Got a loop detect pkt\n"));
				return TRUE;
			}
		}
		return FALSE;
	}
	return FALSE;
}

BOOLEAN ez_set_open_group_id_hook(
	ez_dev_t *ezdev,
	unsigned char *open_group_id,
	unsigned int open_group_id_len,
	unsigned char inf_idx)
{
	int i;
	struct _ez_security *ez_sec_info = &ezdev->ez_security;
	if (ez_sec_info->open_group_id_len != 0) {
		NdisZeroMemory(ez_sec_info->open_group_id,OPEN_GROUP_MAX_LEN);
		ez_sec_info->open_group_id_len = 0;
	}
	if(open_group_id_len > OPEN_GROUP_MAX_LEN)
		return FALSE;
	ez_sec_info->open_group_id_len = open_group_id_len;
	//EZ_MEM_ALLOC(NULL, &ez_sec_info->group_id, ez_sec_info->group_id_len);
		NdisZeroMemory(ez_sec_info->open_group_id, OPEN_GROUP_MAX_LEN);
		NdisCopyMemory(ez_sec_info->open_group_id, open_group_id, ez_sec_info->open_group_id_len);
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("IF(ra%d) %s :: open_group id \n", 
			inf_idx, __FUNCTION__));
		for (i = 0; i < ez_sec_info->open_group_id_len; i++) {
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("%02x ", ez_sec_info->open_group_id[i]));
		}
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("\n"));
	return TRUE;
}


/*
Determines whether Tx group packet is to be dropped by Ap interface
for duplicate CLI links ONLY
*/
BOOLEAN ez_ap_tx_grp_pkt_drop_to_ez_apcli_hook(ez_dev_t *ezdev, struct sk_buff *pSkb)
{
#if defined(CONFIG_WIFI_PKT_FWD) || defined(CONFIG_WIFI_PKT_FWD_MODULE)
	unsigned int recv_from = 0, band_from = 0;
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;

	band_from = RTMP_GET_PACKET_BAND(pSkb); 
	recv_from = RTMP_GET_PACKET_RECV_FROM(pSkb);

	if ((band_from==0) && (recv_from==0) ){
		if((ez_ad->default_group_data_band == EZ_DROP_GROUP_DATA_BAND5G) &&
			((*ezdev->channel >= H_CHANNEL_BIGGER_THAN) || (*ezdev->channel > 14))){ // just chan 14 check should be enough but kept as in other places
			return TRUE;
		} else if((ez_ad->default_group_data_band == EZ_DROP_GROUP_DATA_BAND24G) &&
					!(*ezdev->channel > 14)) {
			return TRUE;
		} 
	}
	if(is_other_band_rcvd_pkt(ezdev,pSkb) == TRUE){
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_ap_tx_grp_pkt_drop_to_ez_apcli: other band rcvd grp packet !!!\n"));
		return TRUE;
	}

	//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_ap_tx_grp_pkt_drop_to_ez_apcli : not other band rcvd pkt\n"));
#endif

	return FALSE;
}



/*
Determines whether Tx group packet is to be dropped by apcli interface

If both apcli interfaces connected to same root ap, then each apcli will
drop packets recvd on other band ap/apcli & allow any other packet.

*/

BOOLEAN ez_apcli_tx_grp_pkt_drop_hook(ez_dev_t *ezdev,struct sk_buff *pSkb)
{
	struct _ez_security *ez_sec_info = &ezdev->ez_security;
#if defined(CONFIG_WIFI_PKT_FWD) || defined(CONFIG_WIFI_PKT_FWD_MODULE)

	unsigned int recv_from = 0, band_from = 0;
	unsigned char is_5g_band = 0;
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;

	if( (ez_sec_info->this_band_info.non_easy_connection == TRUE) &&
		RTMP_IS_PACKET_APCLI(pSkb)
	  )
	{
		if( (ez_sec_info->loop_chk_info.loop_chk_role == SOURCE) || 
		    (ez_sec_info->loop_chk_info.loop_chk_role == DEST) ){

			if(is_other_band_cli_rcvd_pkt(ezdev, pSkb) == TRUE){
				//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_apcli_tx_group_packet_drop: DROP other band CLI rcvd grp packet from non-easy AP!!!\n"));
				return TRUE;
			}
		}
		//else{
		//	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ERROR: Allow Grp packet as Loop check not yet triggered\n"));
		//}
	}
#endif

	if(!ez_sec_info->this_band_info.shared_info.link_duplicate){
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_apcli_tx_group_packet_drop: Both ApCli not connected to same device \n"));
		return FALSE;
	}

#if defined(CONFIG_WIFI_PKT_FWD) || defined(CONFIG_WIFI_PKT_FWD_MODULE)
	band_from = RTMP_GET_PACKET_BAND(pSkb); 
	recv_from = RTMP_GET_PACKET_RECV_FROM(pSkb);

#if 0
	if( (band_from==0) && (recv_from==0) && 
		( (*ezdev->channel >= H_CHANNEL_BIGGER_THAN) || (*ezdev->channel > 14) )){ // just chan 14 check should be enough but kept as in other places
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_apcli_tx_group_packet_drop: DROP untagged grp packet on non2g duplicate link !!!\n"));
		return TRUE;
	}
#else
	if( (*ezdev->channel >= H_CHANNEL_BIGGER_THAN) || (*ezdev->channel > 14) )
		is_5g_band = TRUE;

	if( (band_from==0) && (recv_from==0))
	{
		if((ez_ad->default_group_data_band == EZ_DROP_GROUP_DATA_BAND24G) && (is_5g_band == FALSE))
			return TRUE;
		else if ((ez_ad->default_group_data_band == EZ_DROP_GROUP_DATA_BAND5G) && (is_5g_band == TRUE))
			return TRUE;
	}
#endif
	if(is_other_band_cli_rcvd_pkt(ezdev,pSkb) == TRUE) {
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_apcli_tx_group_packet_drop: DROP other band CLI rcvd grp packet from non-easy AP!!!\n"));
		return TRUE;
	}
	if(RTMP_GET_PACKET_IGMP(pSkb)) {
		return FALSE;
	}
	if(is_other_band_rcvd_pkt(ezdev,pSkb) == TRUE){
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_apcli_tx_group_packet_drop: DROP other band rcvd grp packet !!!\n"));
		return TRUE;
	}
#endif

	return FALSE;
}

VOID ez_connection_allow_all_hook(EZ_ADAPTER *ez_ad, unsigned char default_pmk_valid)
{
	ez_dev_t *ezdev, *cli_ezdev;
	int apcli_enable_count = 0;
	int i=0;

#ifdef IF_UP_DOWN
	if(!ez_all_ez_intf_up(ez_ad)) {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,
				("------> %s(): Error: All Ez interfaces not up\n", __func__));
	}
#endif
	for (i = 0; i < MAX_EZ_BANDS; i++)
	{
		ezdev = &ez_adapter->ez_band_info[i].ap_ezdev;
		cli_ezdev = &ez_adapter->ez_band_info[i].cli_ezdev;
		if (ezdev->wdev == NULL)
		{
			continue;
		}

		if (ez_adapter->band_count == 1)
		{
			ez_init_other_band_backup(ezdev, cli_ezdev);
		}

		NdisCopyMemory(ezdev->ez_security.this_band_info.shared_info.ap_mac_addr,
			ezdev->if_addr,MAC_ADDR_LEN);
		
		NdisCopyMemory(cli_ezdev->ez_security.this_band_info.shared_info.ap_mac_addr,
			ezdev->if_addr,MAC_ADDR_LEN);

	NdisCopyMemory(ezdev->ez_security.this_band_info.shared_info.cli_mac_addr,
		cli_ezdev->if_addr,MAC_ADDR_LEN);
	
	NdisCopyMemory(cli_ezdev->ez_security.this_band_info.shared_info.cli_mac_addr,
		cli_ezdev->if_addr,MAC_ADDR_LEN);

	//! Levarage from MP1.0 CL 170210
#ifdef DEDICATED_MAN_AP		
		// Remove this and make dedicated when all interface has ApcliEnable=0
		if(cli_ezdev->wdev != NULL){
			if(cli_ezdev->driver_ops->get_apcli_enable(cli_ezdev) == TRUE) {
				apcli_enable_count += 1;
			}
		}
#endif

	}

	//! Levarage from MP1.0 CL 170210
#ifdef DEDICATED_MAN_AP
		if(apcli_enable_count == 0) {
			ez_ad->device_info.network_weight[0] = 0xf;
#ifdef EZ_DFS_SUPPORT
			ez_ad->dedicated_man_ap = TRUE;
#endif
		} else {
			ez_ad->device_info.network_weight[0] = 0x0;
#ifdef EZ_DFS_SUPPORT
			ez_ad->dedicated_man_ap = FALSE;
#endif
			}
		ez_ad->configured_status = EZ_CONFIGURED;

		if ((ez_ad->ez_band_info[0].ap_ezdev.wdev) && (*(ez_ad->ez_band_info[0].ap_ezdev.channel) > 14) && ez_ad->band_count == 2)
		{
			NdisCopyMemory(&ez_ad->device_info.network_weight[1] 
			,ez_ad->ez_band_info[1].ap_ezdev.bssid,MAC_ADDR_LEN);
		} else {
			NdisCopyMemory(&ez_ad->device_info.network_weight[1] 
			,ez_ad->ez_band_info[0].ap_ezdev.bssid,MAC_ADDR_LEN);	
		}

		ez_allocate_node_number(&ez_ad->device_info.ez_node_number,
			&ez_ad->ez_band_info[0].ap_ezdev);
		
		for (i = 0; i < MAX_EZ_BANDS; i++)
		{
			void *wdev = ez_adapter->ez_band_info[i].ap_ezdev.wdev;
			if(wdev != NULL) {
				ez_ad->ez_band_info[i].lut_driver_ops.UpdateBeaconHandler(&ez_ad->ez_band_info[i].ap_ezdev, IE_CHANGE);
			}
		}
#ifdef CONFIG_PUSH_VER_SUPPORT
			if(default_pmk_valid)
				ez_ad->device_info.network_weight[NETWORK_WEIGHT_LEN - 1] = 1;
#endif
		
#endif
		if(ez_adapter->band_count == 1 && ez_adapter->non_ez_band_count == 2)
		{
			ez_init_triband_config();
		}
		ez_update_connection_permission_hook(&ez_ad->ez_band_info[0].ap_ezdev, EZ_ALLOW_ALL_TIMEOUT);
		
}


void ez_stop_hook(
	ez_dev_t * ezdev)
{
	struct _ez_security *ez_sec_info = NULL;
#ifdef IF_UP_DOWN
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
#endif
	ez_dev_t *ez_connect_wait_ezdev = ez_ad->ez_connect_wait_ezdev;
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("------> %s(): wdev_type = 0x%x\n", __FUNCTION__, ezdev->ezdev_type));
//! Levarage from MP1.0 CL#170037
	OS_NdisFreeSpinLock(&ezdev->ez_security.ez_scan_pause_timer_lock);
	if(ez_is_connection_allowed_hook(ezdev)) {
		ez_update_connection_permission_hook(ezdev,EZ_ALLOW_ALL);
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("------> %s():line is %d", __FUNCTION__, __LINE__));
	}
	else if ((ez_connect_wait_ezdev == ezdev)
		&& (ez_connect_wait_ezdev->ez_security.weight_update_going_on)){
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("------> %s():line is %d", __FUNCTION__, __LINE__));
		ez_update_connection_permission_hook(ezdev,EZ_ALLOW_ALL_TIMEOUT);
	}
	ez_dealloc_band(ezdev);

#ifdef IF_UP_DOWN
	if (ez_ad->ez_intf_count_current_ap == 0
		&& ez_ad->ez_intf_count_current_cli == 0)
		ez_ad->ez_all_intf_up_once = FALSE;
#endif

	ez_sec_info = &ezdev->ez_security;

	if(ez_sec_info->group_id) {
		  EZ_MEM_FREE(ez_sec_info->group_id);
		  ez_sec_info->group_id = NULL;
		  ez_sec_info->group_id_len = 0;
	}

	if(ez_sec_info->ez_group_id) {
		  EZ_MEM_FREE(ez_sec_info->ez_group_id);
		  ez_sec_info->ez_group_id = NULL;
		  ez_sec_info->ez_group_id_len = 0;
	}

	if(ez_sec_info->gen_group_id) {
		  EZ_MEM_FREE(ez_sec_info->gen_group_id);
		  ez_sec_info->gen_group_id = NULL;
		  ez_sec_info->gen_group_id_len = 0;
	}

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("<------ %s()\n", __FUNCTION__));
}

VOID ez_scan_timeout_hook(
	ez_dev_t *ezdev)
{
	struct _ez_security *ez_sec_info;
    BOOLEAN scan_paused = FALSE;

	int flags, scan_one_channel = FALSE;

	ez_sec_info = &ezdev->ez_security;
	EZ_IRQ_LOCK(&ez_sec_info->ez_scan_pause_timer_lock, flags);
    if(ezdev->driver_ops->ez_is_timer_running(ezdev, ez_sec_info->ez_scan_pause_timer))
		scan_paused = TRUE;
	EZ_IRQ_UNLOCK(&ez_sec_info->ez_scan_pause_timer_lock, flags);

    if(!scan_paused){
		if (!MAC_ADDR_EQUAL(ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid, ZERO_MAC_ADDR)
			|| ezdev->ez_security.ez_scan_same_channel)
			scan_one_channel = TRUE;
		ezdev->driver_ops->ez_ApSiteSurvey_by_wdev(ezdev, NULL, 0, FALSE,
			scan_one_channel);
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
				("%s - Scan for finding ap.\n",
				__FUNCTION__));
    }
	else{
		  EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			    ("%s - Scan paused.\n", 
		    	__FUNCTION__));
	}

}
#if 0
void ez_stop_scan_timeout_hook(ez_dev_t *ezdev)
{
	if (ezdev->driver_ops->ez_is_timer_running(ezdev, ezdev->ez_security.ez_scan_timer)) {
				ezdev->driver_ops->ez_cancel_timer(ezdev, ezdev->ez_security.ez_scan_timer);
			}
}
#endif
void ez_group_merge_timeout_hook(ez_dev_t *ezdev)
{
		
	NdisZeroMemory(ezdev->ez_security.merge_peer_addr, MAC_ADDR_LEN);
	EZ_CLEAR_CAP_ALLOW_MERGE(ezdev->ez_security.capability);
	if (ezdev->ezdev_type == EZDEV_TYPE_AP)
	{
		ezdev->driver_ops->UpdateBeaconHandler(ezdev, IE_CHANGE);
	}
}


/* Loop Check timeout handler*/
VOID ez_loop_chk_timeout_hook(
	ez_dev_t *ezdev)
{
	struct _ez_security *ez_sec_info = NULL;
	 BOOLEAN last_loop_detect_status = FALSE;
	 UCHAR dup_to_non_dup_trans_detect_count = 0;
	
		ez_sec_info = &ezdev->ez_security;


			if(ez_sec_info->first_loop_check){
				last_loop_detect_status = ez_sec_info->dest_loop_detect;
				dup_to_non_dup_trans_detect_count = 0;
			}
			ez_sec_info->first_loop_check = FALSE;

			//ez_sec_info->loop_chk_info.loop_chk_role = NONE;
			//ez_set_other_band_cli_loop_chk_info(ezdev->ez_ad,ezdev,FALSE);

			if(ez_sec_info->dest_loop_detect != TRUE){

				if(dup_to_non_dup_trans_detect_count != 0){
					dup_to_non_dup_trans_detect_count++;					
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Non duplicate link confirm : %d\n", dup_to_non_dup_trans_detect_count));
				}
				else if(last_loop_detect_status != ez_sec_info->dest_loop_detect){
					dup_to_non_dup_trans_detect_count++;
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Not a duplicate link now : %d\n", dup_to_non_dup_trans_detect_count));
				}
				else{
					dup_to_non_dup_trans_detect_count = 0;
				}

				if(dup_to_non_dup_trans_detect_count >= 2){
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_loop_chk_timeout: Clear duplciate link status\n"));
					ez_sec_info->this_band_info.shared_info.link_duplicate = FALSE;
					ez_inform_all_interfaces(ezdev->ez_ad,ezdev, ACTION_UPDATE_DUPLICATE_LINK_ENTRY);

					dup_to_non_dup_trans_detect_count = 0;
				}
			}
			last_loop_detect_status = ez_sec_info->dest_loop_detect;

			
			if(!MAC_ADDR_EQUAL(ezdev->bssid, ZERO_MAC_ADDR)){
				//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_loop_chk_timeout: Recheck for Loop\n"));
				ez_chk_loop_thru_non_ez_ap(ezdev->ez_ad,ezdev);
			}
			else{
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("ERROR !!! ez_loop_chk_timeout: not found mac entry\n"));
			}

}


BOOLEAN ez_is_loop_pkt_rcvd_hook(ez_dev_t *ezdev, 
	UINT8* loop_check_source, UINT8 * loop_check_cli)	
{
	
	if(NdisCmpMemory(loop_check_source, ezdev->ez_security.loop_chk_info.source_mac,MAC_ADDR_LEN) != 0){ // return 0 for same??
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Dest: Source pkt not rcvd\n"));
		return FALSE;
	}

	{
		//NdisZeroMemory(&wdev->ez_security.loop_chk_info,sizeof(LOOP_CHK_INFO));

		if( !NdisCmpMemory(loop_check_cli,ezdev->ez_security.this_band_info.shared_info.cli_mac_addr,MAC_ADDR_LEN) )
		{
			//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Loop Pkt detected for this device\n"));

			if(ezdev->ez_security.this_band_info.shared_info.link_duplicate != TRUE)			
			{
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("Loop detected by Dest, Mark link duplicate\n"));
				ezdev->ez_security.this_band_info.shared_info.link_duplicate = TRUE;
			}

			// call always as source is doing periodc loop check
			ez_inform_other_band_cli_loop_detect(ezdev);

		}

	    return TRUE;		
	}
	return FALSE;
}

VOID APTribandRestartNonEzReqAction_hook(
		EZ_ADAPTER *ez_ad)
{
	int band_count;
	for (band_count = 0; band_count < MAX_NON_EZ_BANDS; band_count++)
	{
		if (!ez_ad->non_ez_band_info[band_count].need_restart)
		{
			continue;
		}
		ez_ad->non_ez_band_info[band_count].lut_driver_ops.restart_ap(ez_ad->non_ez_band_info[band_count].non_ez_ap_wdev);
		ez_ad->non_ez_band_info[band_count].need_restart = FALSE;
	}

}


void ez_allocate_or_update_non_ez_band_hook(void *wdev, void *ad, UINT32 ezdev_type, CHAR func_idx, non_ez_driver_ops_t *driver_ops, unsigned char *channel)
{
	ez_dev_t *ezdev = ezdev;
	int band_count = 0;
	BOOLEAN found_band_entry = FALSE;
	EZ_ADAPTER *ez_ad = ez_adapter;
	for (band_count = 0; band_count < ez_ad->non_ez_band_count; band_count++)
	{
		if((ez_ad->non_ez_band_info[band_count].pAd == ad) && ez_ad->non_ez_band_info[band_count].func_idx == func_idx) 
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("%s::Found band entry at index: %d\n", __FUNCTION__ ,band_count));
			if((ezdev_type == EZDEV_TYPE_AP))
			{
				//ASSERT(ez_ad->non_ez_band_info[band_count].non_ap_ezdev == NULL);
				ez_ad->non_ez_band_info[band_count].non_ez_ap_wdev = wdev;
				ez_ad->non_ez_band_info[band_count].channel = channel;
				NdisCopyMemory(&ez_ad->non_ez_band_info[band_count].lut_driver_ops, driver_ops, sizeof(non_ez_driver_ops_t));
#ifdef IF_UP_DOWN
			if (ez_ad->non_ez_intf_count_current_ap < ez_ad->non_ez_intf_count_config_ap)
				ez_ad->non_ez_intf_count_current_ap++;
#endif
			} 
			else {
				//ASSERT(ez_ad->non_ez_band_info[band_count].non_ez_cli_ezdev == NULL);
				ez_ad->non_ez_band_info[band_count].non_ez_cli_wdev = wdev;				
#ifdef IF_UP_DOWN
				if (ez_ad->non_ez_intf_count_current_cli < ez_ad->non_ez_intf_count_config_cli)
					ez_ad->non_ez_intf_count_current_cli++;
#endif
			} 
			found_band_entry = TRUE;
			//ezdev->ez_band_idx = band_count;
		
		}
	}
	if (!(found_band_entry) && (ez_ad->non_ez_band_count < MAX_NON_EZ_BANDS))
	{
		for (band_count = 0; band_count < MAX_NON_EZ_BANDS; band_count++)
		{
			if (ez_ad->non_ez_band_info[band_count].non_ez_cli_wdev == NULL 
				&& ez_ad->non_ez_band_info[band_count].non_ez_ap_wdev == NULL)
			{
				break;
			} else {
				continue;
			}
		}
		if (band_count == MAX_NON_EZ_BANDS)
		{
			return;
		}
		EZ_DEBUG(DBG_CAT_INIT, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("%s:: add new band entry at index: %d\n", __FUNCTION__ ,band_count));

		ez_ad->non_ez_band_info[band_count].pAd = ad;
		ez_ad->non_ez_band_info[band_count].func_idx = func_idx;
		if((ezdev_type == EZDEV_TYPE_AP))
		{
			//ASSERT(ez_ad->non_ez_band_info[band_count].non_ap_ezdev == NULL);
			ez_ad->non_ez_band_info[band_count].non_ez_ap_wdev = wdev;	
			ez_ad->non_ez_band_info[band_count].channel = channel;
			NdisCopyMemory(&ez_ad->non_ez_band_info[band_count].lut_driver_ops,driver_ops,sizeof(non_ez_driver_ops_t));
		} 
		else {
			//ASSERT(ez_ad->non_ez_band_info[band_count].non_ez_cli_ezdev == NULL);
			ez_ad->non_ez_band_info[band_count].non_ez_cli_wdev = wdev; 			
		} 
		ez_ad->non_ez_band_count++; 	
	}
}


ULONG ez_BssTableSearchWithBssId(
     EZ_BSS_TABLE *Tab,
     PUCHAR     Bssid,
     UCHAR      Channel)
{
	UCHAR i;
	UINT BssNr = Tab->BssNr;

	for (i = 0; i < BssNr; i++)
	{
		if ((i < EZ_MAX_LEN_OF_BSS_TABLE) &&
			MAC_ADDR_EQUAL(&(Tab->BssEntry[i].Bssid), Bssid))
		{
			return i;
		}
	}
	return (ULONG)BSS_NOT_FOUND;
}

#if 0

ez_add_entry_in_apcli_tab(void *ad_obj, void *wdev_obj, ULONG bss_entry_idx)
{
	PRTMP_ADAPTER pAd = ad_obj;
	struct wifi_dev *wdev = wdev_obj;
	APCLI_STRUCT *apcli_entry = &pAd->ApCfg.ApCliTab[wdev->func_idx];
	BSS_TABLE Tab = &apcli_entry->MlmeAux.SsidBssTab;
	NdisCopyMemory(Tab->BssEntry[Tab->BssNr],pAd->ScanTab.BssEntry[bss_entry_idx],sizeof(BSS_ENTRY));
	Tab->BssNr++;
	Tab->BssOverlapNr++;
}

ez_ApCliBssTabInit(void *ad_obj, void *wdev_obj)
{
	PRTMP_ADAPTER pAd = ad_obj;
	struct wifi_dev *wdev = wdev_obj;
	APCLI_STRUCT *apcli_entry = &pAd->ApCfg.ApCliTab[wdev->func_idx];
	BSS_TABLE Tab = &apcli_entry->MlmeAux.SsidBssTab;
	BssTableInit(Tab);
}


ez_get_scan_table(void *ad_obj,EZ_BSS_TABLE *pEzBss)
{
	PRTMP_ADAPTER pAd = ad_obj;
	BSS_TABLE *Tab = pAd->ScanTab;
	UINT16 i;
	pEzBss->BssNr = Tab->BssBr;
	
	for (i = 0; i < Tab->BssNr; i++) 
	{
		COPY_MAC_ADDR(pEzBss->BssEntry[i].MacAddr, Tab->BssEntry[i].MacAddr);
		COPY_MAC_ADDR(pEzBss->BssEntry[i].Bssid, Tab->BssEntry[i].Bssid);
		pEzBss->BssEntry[i].Channel = Tab->BssEntry[i].Channel;
		pEzBss->BssEntry[i].CentralChannel = Tab->BssEntry[i].CentralChannel;
		pEzBss->BssEntry[i].Rssi = Tab->BssEntry[i].Rssi;
		pEzBss->BssEntry[i].SsidLen = Tab->BssEntry[i].SsidLen;
		NdisCopyMemory(pEzBss->BssEntry[i].Ssid, Tab->BssEntry[i].Ssid, pEzBss->BssEntry[i].Ssidlen);
		pEzBss->BssEntry[i].AKMMap = Tab->BssEntry[i].AKMMap;
		pEzBss->BssEntry[i].PairwiseCipher = Tab->BssEntry[i].PairwiseCipher;
		pEzBss->BssEntry[i].GroupCipher = Tab->BssEntry[i].GroupCipher;
		pEzBss->BssEntry[i].support_easy_setup = Tab->BssEntry[i].support_easy_setup;
		pEzBss->BssEntry[i].easy_setup_capability = Tab->BssEntry[i].easy_setup_capability;
		pEzBss->BssEntry[i].bConnectAttemptFailed = Tab->BssEntry[i].bConnectAttemptFailed;
		pEzBss->BssEntry[i].non_ez_beacon = Tab->BssEntry[i].non_ez_beacon;
		NdisCopyMemory(pEzBss->BssEntry[i].open_group_id,
				Tab->BssEntry[i].open_group_id,Tab->BssEntry[i].open_group_id_len);
		pEzBss->BssEntry[i].open_group_id_len = Tab->BssEntry[i].open_group_id_len;
		NdisCopyMemory(pEzBss->BssEntry[i].beacon_info,Tab->BssEntry[i].beacon_info,sizeof(beacon_info_tag_t));

	}

}

#endif
VOID ez_BssTableSortByRssi(
	EZ_BSS_TABLE *OutTab,
	BOOLEAN isInverseOrder)
{
	INT i, j;
	EZ_BSS_ENTRY *pTmpBss = NULL;


	/* allocate memory */
	ez_os_alloc_mem(NULL, (UCHAR **)&pTmpBss, sizeof(EZ_BSS_ENTRY));
	if (pTmpBss == NULL)
	{
		EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("%s: Allocate memory fail!!!\n", __FUNCTION__));
		return;
	}

	for (i = 0; i < OutTab->BssNr - 1; i++)
	{
		for (j = i+1; j < OutTab->BssNr; j++)
		{
			if (OutTab->BssEntry[j].Rssi > OutTab->BssEntry[i].Rssi ?
				!isInverseOrder : isInverseOrder)
			{
				if (OutTab->BssEntry[j].Rssi != OutTab->BssEntry[i].Rssi )
				{
					NdisMoveMemory(pTmpBss, &OutTab->BssEntry[j], sizeof(EZ_BSS_ENTRY));
					NdisMoveMemory(&OutTab->BssEntry[j], &OutTab->BssEntry[i], sizeof(EZ_BSS_ENTRY));
					NdisMoveMemory(&OutTab->BssEntry[i], pTmpBss, sizeof(EZ_BSS_ENTRY));
				}
			}
		}
	}

	if (pTmpBss != NULL)
		ez_os_free_mem(pTmpBss);
}


/*! \brief initialize BSS table
 *	\param p_tab pointer to the table
 *	\return none
 *	\pre
 *	\post

 IRQL = PASSIVE_LEVEL
 IRQL = DISPATCH_LEVEL

 */
VOID ez_BssTableInit(EZ_BSS_TABLE *Tab)
{
	int i;

	Tab->BssNr = 0;
	Tab->BssOverlapNr = 0;

	for (i = 0; i < EZ_MAX_LEN_OF_BSS_TABLE; i++)
	{
		//UCHAR *pOldAddr = Tab->BssEntry[i].pVarIeFromProbRsp;
		NdisZeroMemory(&Tab->BssEntry[i], sizeof(EZ_BSS_ENTRY));
		Tab->BssEntry[i].Rssi = -127;	/* initial the rssi as a minimum value */
	}
}

#if 0
VOID ez_BssTableSsidSort(
	IN void * ad_obj,
	IN void *wdev_obj,
	OUT EZ_BSS_TABLE *OutTab,
	IN CHAR Ssid[],
	IN UCHAR SsidLen)
{
	BSS_TABLE driver_out_tab;
	BssTableSsidSort(ad_obj,wdev_obj,&driver_out_tab,Ssid,SsidLen);
	ez_fill_out_table(&driver_out_tab,OutTab);
}
#endif
BOOLEAN check_best_ap_rssi_threshold(struct _ez_security *ez_security, EZ_BSS_ENTRY *bss_entry)
{
	if(bss_entry->Rssi >= ez_adapter->best_ap_rssi_threshld[ez_security->best_ap_rssi_threshold - 1])
	{
		return TRUE;
	}
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%s:: RSSI = %d, Threshold = %d, REJECT CONNECTION\n", __FUNCTION__,
		bss_entry->Rssi, ez_adapter->best_ap_rssi_threshld[ez_security->best_ap_rssi_threshold - 1]));
	return FALSE;
}

#if 0
ez_update_apcli_conn(void* ad_obj,void *ezdev, EZ_BSS_ENTRY bss_entry)
{
	PRTMP_ADAPTER pAd = ad_obj;
	struct wifi_dev *wdev = ((ez_dev_t *)ezdev)->wdev;
	struct wifi_dev *ap_wdev = &pAd->ApCfg.MBSSID[wdev->func_idx].wdev;
	PAPCLI_STRUCT apcli_entry;
	apcli_entry = &pAd->ApCfg.ApCliTab[wdev->func_idx];

	COPY_MAC_ADDR(apcli_entry->CfgApCliBssid, bss_entry->Bssid);
	NdisZeroMemory(apcli_entry->CfgSsid, MAX_LEN_OF_SSID);
	NdisMoveMemory(apcli_entry->CfgSsid, bss_entry->Ssid, bss_entry->SsidLen);
	apcli_entry->CfgSsidLen = bss_entry->SsidLen;
	wdev->SecConfig.AKMMap = bss_entry->AKMMap;
	wdev->SecConfig.PairwiseCipher = bss_entry->PairwiseCipher;
	wdev->SecConfig.GroupCipher = bss_entry->GroupCipher;
#ifdef EZ_NETWORK_MERGE_SUPPORT
	if(ezdev->ez_security.internal_force_connect_bssid == TRUE)
	{
		if (wdev->channel != bss_entry->Channel)
		{
			EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("stale scan list %d %d!!!\n",wdev->channel,bss_entry->Channel));
			//ASSERT(FALSE);
			ez_initiate_new_scan(pAd);
			return FALSE;
		}
	}
	//! adjust APCLI's operating bandwidth to that of peer
	ez_ApCliAutoConnectBWAdjust(pAd, wdev, bss_entry);
	ez_ApCliAutoConnectBWAdjust(pAd, ap_wdev, bss_entry);
#ifdef EZ_NETWORK_MERGE_SUPPORT
#ifdef EZ_DUAL_BAND_SUPPORT
	EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("ez_apcli_search_best_ap_configured\nAPCLI=> CurrChannel: %d, TarChannel: %d\n", 
		ezdev->ez_security.this_band_info.shared_info.channel_info.channel,
						bss_entry->Channel));
#ifdef EZ_PUSH_BW_SUPPORT
	//if( ((PRTMP_ADAPTER)(wdev->sys_handle))->push_bw_config )
	{
		EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("\tHT-BW: %d, CFG:%d OPER:%d\n",
							ezdev->ez_security.this_band_info.shared_info.channel_info.ht_bw,
							wlan_config_get_ht_bw(ezdev),wlan_operate_get_ht_bw(ezdev)));
		EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("\tVHT-BW: %d CFG:%d OPER:%d\n", 
							ezdev->ez_security.this_band_info.shared_info.channel_info.vht_bw,
							wlan_config_get_vht_bw(wdev),wlan_operate_get_vht_bw(wdev)));
	}
#else
	{
		EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("\tHT-BW: CFG:%d OPER:%d\n",
							wlan_config_get_ht_bw(wdev),wlan_operate_get_ht_bw(wdev)));
		EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("\tVHT-BW: CFG:%d OPER:%d\n", 
							wlan_config_get_vht_bw(wdev),wlan_operate_get_vht_bw(wdev)));
	}
#endif
	EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("\tEXTCHA: %d CFG:%d OPER:%d\n",
						wdev->ez_security.this_band_info.shared_info.channel_info.extcha,
						wlan_config_get_ext_cha(wdev),wlan_operate_get_ext_cha(wdev)));
	EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("AP=> CurrChannel: %d, TarChannel: %d\n", 
		ap_wdev->ez_security.this_band_info.shared_info.channel_info.channel,
						bss_entry->Channel));
#ifdef EZ_PUSH_BW_SUPPORT
	//if( ((PRTMP_ADAPTER)(ap_wdev->sys_handle))->push_bw_config )
	{
		EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("\tHT-BW: %d, CFG:%d OPER:%d\n",
							ap_ezdev->ez_security.this_band_info.shared_info.channel_info.ht_bw,
							wlan_config_get_ht_bw(ap_ezdev),wlan_operate_get_ht_bw(ap_ezdev)));
		EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("\tVHT-BW: %d CFG:%d OPER:%d\n", 
							ap_ezdev->ez_security.this_band_info.shared_info.channel_info.vht_bw,
							wlan_config_get_vht_bw(ap_ezdev),wlan_operate_get_vht_bw(ap_ezdev)));
	}
#else
	{
		EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("\tHT-BW: CFG:%d OPER:%d\n",
							wlan_config_get_ht_bw(ap_wdev),wlan_operate_get_ht_bw(ap_ezdev)));
		EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("\tVHT-BW: CFG:%d OPER:%d\n", 
							wlan_config_get_vht_bw(ap_wdev),wlan_operate_get_vht_bw(ap_wdev)));
	}
#endif

	EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("\tEXTCHA: %d CFG:%d OPER:%d\n", 
						ap_wdev->ez_security.this_band_info.shared_info.channel_info.extcha,
						wlan_config_get_ext_cha(ap_wdev),wlan_operate_get_ext_cha(ap_wdev)));
#endif
#endif

	wdev->ez_driver_params.do_not_restart_interfaces = 1;
							
	rtmp_set_channel(pAd,ap_wdev, bss_entry->Channel);

	wdev->ez_driver_params.do_not_restart_interfaces = 0;
#else
	if ((ap_wdev->channel != bss_entry->Channel) || (wdev->channel != bss_entry->Channel)) {
		EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("%s - Switch channel to ch.%d\n", __FUNCTION__, bss_entry->Channel));
		rtmp_set_channel(pAd,ap_wdev, bss_entry->Channel);
#ifdef APCLI_AUTO_CONNECT_SUPPORT
#ifdef APCLI_AUTO_BW_TMP
		if(ApCliAutoConnectBWAdjust(pAd, wdev, bss_entry))
			rtmp_set_channel(pAd, ap_wdev, bss_entry->Channel);
		 else
			EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("%s(): ApCliAutoConnectBWAdjust() return FALSE\n", __FUNCTION__));
		
#endif //APCLI_AUTO_BW_TMP
#endif //APCLI_AUTO_CONNECT_SUPPORT
	}
#endif

}
#endif
BOOLEAN ez_apcli_search_best_ap_configured(
	void *ez_ad_obj,
	void * ez_dev_obj,
	void *out_table_obj)
{
	EZ_ADAPTER *ez_ad = (EZ_ADAPTER *)ez_ad_obj;
	ez_dev_t *ezdev = ez_dev_obj;
	PEZ_BSS_TABLE out_table = (PEZ_BSS_TABLE)out_table_obj;
	//PAPCLI_STRUCT apcli_entry = (PAPCLI_STRUCT)apcli_entry_obj;
	//struct wifi_dev *wdev = &apcli_entry->wdev;
	BOOLEAN found_ap=FALSE;
	signed char rssi_threshold = ezdev->ez_security.rssi_threshold;
	EZ_BSS_ENTRY *bss_entry = NULL;
	unsigned long bss_idx;
	UCHAR i=0;
#ifdef EZ_NETWORK_MERGE_SUPPORT	
	EZ_BSS_ENTRY *temp_bss_entry = NULL;
	UCHAR temp_bss_index = 0;
	enum_group_merge_action_t group_merge_action;
#endif	
	//ez_dev_t *ap_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev;
#ifdef EZ_ROAM_SUPPORT
	BOOLEAN do_roam = FALSE;
#endif

	EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_ERROR,(" %s(apcli%d) -->\n", __FUNCTION__, ezdev->ez_band_idx));
	ezdev->support_ez_setup = FALSE;

	if (ezdev->ez_security.first_scan)
	{
		ezdev->ez_security.first_scan = FALSE;
		
		EZ_DEBUG(DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_TRACE, 
			("(%s) Scan again.\n", __FUNCTION__));
		ezdev->driver_ops->ez_ApSiteSurvey_by_wdev(ezdev, NULL, 0, FALSE,
			ezdev->ez_security.ez_scan_same_channel);
		return FALSE;
	}

// TODO: Raghav : add channel check. if forced by user. and scan only on that channel
	if (ezdev->ez_security.ez_apcli_force_ssid_len != 0)
	{
		if(out_table->BssNr == 0){ 
			/*
				Find all matching BSS in the lastest SCAN result (inBssTab)
				Sort by RSSI order
			*/
			ezdev->driver_ops->ez_BssTableSsidSort(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd,
						ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.wdev,
						out_table,(PCHAR) ezdev->ez_security.ez_apcli_force_ssid,
						ezdev->ez_security.ez_apcli_force_ssid_len);
			ez_BssTableSortByRssi(out_table, FALSE);
			ezdev->driver_ops->ez_sort_apcli_tab_by_rssi(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd,
						ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.wdev);

		}
	}
	else if(out_table->BssNr == 0){ /* no easy devices detected yet*/
		EZ_BSS_TABLE *ez_scan_tab = NULL;
		ez_os_alloc_mem(NULL, (PUCHAR *)&ez_scan_tab, sizeof(EZ_BSS_TABLE));
		if(ez_scan_tab == NULL)
			return FALSE;
		
		ez_BssTableInit(out_table);
		ezdev->driver_ops->ez_get_scan_table(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd,ez_scan_tab);
#ifdef EZ_DUAL_BAND_SUPPORT
				{
					interface_info_t other_band_info;
					EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("get other_band_info\n"));
					if (ez_get_other_band_info(ezdev,&other_band_info)){
						EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("get other CLI peer\n"));
						ez_hex_dump("other_band_info.cli_peer_ap_mac", other_band_info.cli_peer_ap_mac, 6);
						if (!MAC_ADDR_EQUAL(other_band_info.cli_peer_ap_mac, ZERO_MAC_ADDR)
							&& ! ezdev->ez_security.internal_force_connect_bssid_timeout
							&& MAC_ADDR_EQUAL(ezdev->ez_security.ez_apcli_force_bssid,ZERO_MAC_ADDR)
							&& MAC_ADDR_EQUAL(ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid,ZERO_MAC_ADDR)
							&& other_band_info.non_easy_connection == FALSE)
						{
							struct _ez_peer_security_info * ez_other_cli_peer_ap;
							ez_dev_t *other_band_ezdev = ez_get_otherband_cli_ezdev(ezdev);
							
							//PRTMP_ADAPTER ad = wdev->sys_handle;
							ez_other_cli_peer_ap = ez_peer_table_search_by_addr_hook(other_band_ezdev, other_band_info.cli_peer_ap_mac);
							//ez_other_cli_peer_ap = ez_peer_table_search_by_addr_hook(other_band_wdev, other_band_info.cli_peer_ap_mac);

							if ( ez_other_cli_peer_ap != NULL
								&& !MAC_ADDR_EQUAL(other_band_info.cli_peer_ap_mac, ZERO_MAC_ADDR)){

								ez_hex_dump("Other Band CLI connected to", other_band_info.cli_peer_ap_mac, MAC_ADDR_LEN);

								ez_hex_dump("will attempt connection to",ez_other_cli_peer_ap->other_band_info.shared_info.ap_mac_addr, MAC_ADDR_LEN);
															
								bss_idx = ez_BssTableSearchWithBssId(ez_scan_tab,ez_other_cli_peer_ap->other_band_info.shared_info.ap_mac_addr,0);
								if (bss_idx == BSS_NOT_FOUND)
								{
									found_ap = FALSE;
									if (!ezdev->ez_security.internal_force_connect_bssid) {
										NdisGetSystemUpTime(&ezdev->ez_security.force_connect_bssid_time_stamp);
										ezdev->ez_security.internal_force_connect_bssid = TRUE;
									}
									ez_os_free_mem(ez_scan_tab);
									ez_scan_tab = NULL;
									goto found;
								} else {
									//BSS_ENTRY *pBss = &pAd->ScanTab.BssEntry[bss_idx];
								
									//bss_entry = &out_table->BssEntry[0];
									//NdisMoveMemory(bss_entry, pBss, sizeof(BSS_ENTRY)); // Don't do out_table->BssNr++ as this is to be tried immediately;
									ezdev->driver_ops->ez_add_entry_in_apcli_tab(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd,
											ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.wdev,bss_idx);

									
									bss_entry = &ez_scan_tab->BssEntry[bss_idx];
									found_ap = TRUE;
									if (!ezdev->ez_security.internal_force_connect_bssid) {
										NdisGetSystemUpTime(&ezdev->ez_security.force_connect_bssid_time_stamp);
										ezdev->ez_security.internal_force_connect_bssid = TRUE;
									}
									ez_os_free_mem(ez_scan_tab);
									ez_scan_tab = NULL;
									goto found;

								}
							}
						} else {
							ezdev->ez_security.internal_force_connect_bssid = FALSE;
						}
					}
				}
#endif
	
			for (i = 0; i < ez_scan_tab->BssNr; i++) 
			{
				bss_entry = &ez_scan_tab->BssEntry[i];
				if (bss_entry->non_ez_beacon)
				{
					EZ_DEBUG(DBG_CAT_INIT, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("%s----> this is a triband repeater without MAN. Do not connect\n", __FUNCTION__));
					continue;
				}
#if 1
				if (!check_best_ap_rssi_threshold(&ezdev->ez_security, bss_entry))
				{
					continue;
				}
#endif
#ifdef EZ_NETWORK_MERGE_SUPPORT
				group_merge_action = is_group_merge_candidate(bss_entry->easy_setup_capability,ezdev, temp_bss_entry, bss_entry->Bssid);
				//ez_hex_dump("BSS_MAC",bss_entry->Bssid,MAC_ADDR_LEN);
				switch (group_merge_action)
					{
						case EXIT_SWITCH_NOT_GROUP_MERGE:
							break;
						case TERMINATE_LOOP_MULTIPLE_AP_FOUND:
							ezdev->driver_ops->ez_ApCliBssTabInit(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd,
											ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.wdev);
							found_ap = FALSE;
							
							ez_os_free_mem(ez_scan_tab);
							ez_scan_tab = NULL;
							goto found;
							break;
						case TERMINATE_LOOP_TARGET_AP_FOUND:
						
							if ((((*ezdev->channel <=14) && (bss_entry->Channel <= 14))
									|| ((*ezdev->channel > 14) && (bss_entry->Channel> 14)))
									&& (!ez_is_weight_same_mod(ez_adapter->device_info.network_weight,bss_entry->beacon_info.network_weight)))
							{
							
							NdisMoveMemory(&out_table->BssEntry[0], bss_entry, sizeof(EZ_BSS_ENTRY));
							out_table->BssNr++;
							//temp_bss_entry = &out_table->BssEntry[0];
							ezdev->driver_ops->ez_add_entry_in_apcli_tab(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd,
									ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.wdev,i);
								found_ap = TRUE;
								
								ez_os_free_mem(ez_scan_tab);
								ez_scan_tab = NULL;
								goto found;
							} 
							break;
						case CONTINUE_LOOP_TARGET_AP_FOUND:
							if ((((*ezdev->channel <=14) && (bss_entry->Channel <= 14)) 
									|| ((*ezdev->channel > 14) && (bss_entry->Channel > 14)))
									&&  !ez_is_weight_same_mod(ez_adapter->device_info.network_weight,bss_entry->beacon_info.network_weight)){

								NdisMoveMemory(&out_table->BssEntry[0], bss_entry, sizeof(EZ_BSS_ENTRY));
								out_table->BssNr++;
								temp_bss_entry = &out_table->BssEntry[0];

								ezdev->driver_ops->ez_add_entry_in_apcli_tab(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd,
										ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.wdev,i);

								temp_bss_index = i;
								EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("temp bss_entry = %s\n", temp_bss_entry->Ssid));
							}
							else{
								temp_bss_entry = NULL;
							}
							continue;
							break;
						case CONTINUE_LOOP:
							continue;
							break;
					}
				
#endif
				if(IS_SINGLE_CHIP_DBDC(ez_ad)){
					//printk("#########################This is single chip DBDC, check band sanity#################################\n");
					if( ( (*ezdev->channel <=14) && (bss_entry->Channel <= 14) ) ||
						( (*ezdev->channel > 14) && (bss_entry->Channel > 14) ) )
					{
					}
					else{
						//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
						//("%s(line.%d) - Ignore other band network.\n", 
						//__FUNCTION__, __LINE__));
						continue;
					}
				}
				if ( bss_entry->support_easy_setup &&
					ez_is_same_open_group_id(ezdev, bss_entry->open_group_id,bss_entry->open_group_id_len) == FALSE) {
					continue;
				}

#ifdef EZ_ROAM_SUPPORT
				if (!MAC_ADDR_EQUAL(ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid, ZERO_MAC_ADDR))
				{
					if (NdisEqualMemory(bss_entry->Bssid,ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid,MAC_ADDR_LEN))
					{
						if (ez_is_weight_same_mod(ez_adapter->device_info.network_weight,bss_entry->beacon_info.network_weight)
							&& ez_is_other_band_connection_to_same_bss(ezdev,&bss_entry->beacon_info) == FALSE)
						{
							EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("Roam AP found but wt is same. Wait for some time\n"));
							continue;
						}
						
						ezdev->driver_ops->ez_add_entry_in_apcli_tab(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd,
								ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.wdev,i);

						EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Roam ssid =%s\n", bss_entry->Ssid));
						ez_hex_dump("Roam BSSID", ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid, MAC_ADDR_LEN);
						found_ap = TRUE;
						do_roam = TRUE;
						
						ez_os_free_mem(ez_scan_tab);
						ez_scan_tab = NULL;
						goto found;
					}
					continue;
				}
#endif
				if (!ez_is_weight_same_mod(ez_adapter->device_info.network_weight,bss_entry->beacon_info.network_weight)
								|| ez_is_other_band_connection_to_same_bss(ezdev,&bss_entry->beacon_info))
				{
					EZ_BSS_ENTRY *pOutBss = &out_table->BssEntry[out_table->BssNr];
					interface_info_t other_band_info;
					NdisZeroMemory(&other_band_info,sizeof(interface_info_t));
					ez_get_other_band_info(ezdev, &other_band_info);

					if(!MAC_ADDR_EQUAL(other_band_info.cli_peer_ap_mac, ZERO_MAC_ADDR))
					{
						if(other_band_info.non_easy_connection
							&& (!bss_entry->support_easy_setup))
						{
						 	/* If other band connected to third party, will allow this band to connect only with third party */
							{
								NdisMoveMemory(pOutBss, bss_entry, sizeof(EZ_BSS_ENTRY));
								out_table->BssNr++;

								ezdev->driver_ops->ez_add_entry_in_apcli_tab(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd,
										ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.wdev,i);
							}
						}
						else /* Easy setup Enabled Devices */
						{
							/* If other band connected to Easy Device, will allow this band to connect only with Easy Device */
							if((!other_band_info.non_easy_connection)
								&& (bss_entry->support_easy_setup))
							{
								NdisMoveMemory(pOutBss, bss_entry, sizeof(EZ_BSS_ENTRY));
								out_table->BssNr++;

								ezdev->driver_ops->ez_add_entry_in_apcli_tab(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd,
										ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.wdev,i);
							}
						}
					}
					else 
					{
						NdisMoveMemory(pOutBss, bss_entry, sizeof(EZ_BSS_ENTRY));
						out_table->BssNr++;
						ezdev->driver_ops->ez_add_entry_in_apcli_tab(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd,
										ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.wdev,i);
					}
				}
			}
#ifdef EZ_NETWORK_MERGE_SUPPORT
			if (temp_bss_entry != NULL){
				bss_entry = temp_bss_entry;
				i = temp_bss_index; // no use as such

				found_ap = TRUE;
				
				ez_os_free_mem(ez_scan_tab);
				ez_scan_tab = NULL;
				EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_INFO,("setting bss_entry here, %s\n", bss_entry->Ssid));
				goto found;
			}
#endif
			EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("number of valid entries = %d\n", out_table->BssNr));
			ez_BssTableSortByRssi(out_table, FALSE);
			ezdev->driver_ops->ez_sort_apcli_tab_by_rssi(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd,
						ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.wdev);
			//Raghav: sort by a different algo

			ez_os_free_mem(ez_scan_tab);
			ez_scan_tab = NULL;
		}
	

#if 1
	for (i=0; i<out_table->BssNr; i++) {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("[%d] %02x:%02x:%02x:%02x:%02x:%02x RSSI=%d EZ=%d Attempted= %x, configured = %d, weight = %x:%x:%x:%x:%x:%x\n", i, 
			out_table->BssEntry[i].Bssid[0],
			out_table->BssEntry[i].Bssid[1],
			out_table->BssEntry[i].Bssid[2],
			out_table->BssEntry[i].Bssid[3],
			out_table->BssEntry[i].Bssid[4],
			out_table->BssEntry[i].Bssid[5],
			out_table->BssEntry[i].Rssi, out_table->BssEntry[i].support_easy_setup,
			out_table->BssEntry[i].bConnectAttemptFailed,
			EZ_GET_CAP_CONFIGRED(out_table->BssEntry[i].easy_setup_capability),
			PRINT_MAC(out_table->BssEntry[i].beacon_info.network_weight)));
	}
	
	
#endif
	if (!MAC_ADDR_EQUAL(ezdev->ez_security.ez_apcli_force_bssid, ZERO_MAC_ADDR))
	{
		/*if force bssid is given, then don't connect to any other bssid.*/
		for (i=0; i<out_table->BssNr; i++) {
			
			if(out_table->BssEntry[i].bConnectAttemptFailed){
				//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
				//	("%s(line.%d) - Skip already attempted candidate.\n", 
				//	__FUNCTION__, __LINE__));
				continue;
			}
			bss_entry = &out_table->BssEntry[i];
			if (MAC_ADDR_EQUAL(ezdev->ez_security.ez_apcli_force_bssid,bss_entry->Bssid)) {
				found_ap = TRUE;
				COPY_MAC_ADDR(ezdev->CfgApCliBssid, bss_entry->Bssid);
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
					("%s(line.%d) - Find peer with force bssid.\n", 
					__FUNCTION__, __LINE__));
				break;
			}
		}
		goto found;
	}

	/*
		Select normal ap which doesn't support easy setup and matches the SSID and RSSI threshold
	*/
	for (i = 0; i < out_table->BssNr; i++) {
		if(out_table->BssEntry[i].bConnectAttemptFailed){
				//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
				//	("%s(line.%d) - Skip already attempted candidate.\n", 
				//	__FUNCTION__, __LINE__));
				continue;
		}
		bss_entry = &out_table->BssEntry[i];
		if (bss_entry->Rssi >= rssi_threshold && (!bss_entry->support_easy_setup)
			&& (ezdev->CfgSsidLen != 0 && SSID_EQUAL(ezdev->CfgSsid, ezdev->CfgSsidLen,bss_entry->Ssid, bss_entry->SsidLen))
#if 0			
			&& IS_CIPHER_CCMP128(bss_entry->PairwiseCipher)
			&& IS_CIPHER_CCMP128(bss_entry->GroupCipher)
			&& IS_AKM_WPA2PSK(bss_entry->AKMMap)
#endif
			){

			 {
				found_ap = TRUE;
				COPY_MAC_ADDR(ezdev->CfgApCliBssid, bss_entry->Bssid);
				ez_hex_dump("ApcliSsid",ezdev->CfgSsid,ezdev->CfgSsidLen );
				ez_hex_dump("BssSsid",bss_entry->Ssid,bss_entry->SsidLen);
				EZ_DEBUG(DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_TRACE, 
					("%s(line.%d): found ap.\n", 
					__FUNCTION__, __LINE__));
				goto found;
			}
		}
	}

	/*Find configured Dedicated AP.*/
	for (i = 0; i < out_table->BssNr; i++) {
		if(out_table->BssEntry[i].bConnectAttemptFailed){
				//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
				//	("%s(line.%d) - Skip already attempted candidate.\n", 
				//	__FUNCTION__, __LINE__));
				continue;
		}
		bss_entry = &out_table->BssEntry[i];
		if (bss_entry->Rssi >= rssi_threshold && 
			(bss_entry->beacon_info.network_weight[0] == 0xF) &&
			(bss_entry->beacon_info.node_number.path_len == MAC_ADDR_LEN)			
			//SSID_EQUAL(apcli_entry->Ssid, apcli_entry->SsidLen,bss_entry->Ssid, bss_entry->SsidLen)
			){
			found_ap = TRUE;
			COPY_MAC_ADDR(ezdev->CfgApCliBssid, bss_entry->Bssid);
			EZ_DEBUG(DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_TRACE, 
				("%s(line.%d): Found Dedicated MAN AP.\n", 
				__FUNCTION__, __LINE__));
			goto found;
		}
	}

	/*Find configured AP having weight 0x0F */
	for (i = 0; i < out_table->BssNr; i++) {
		if(out_table->BssEntry[i].bConnectAttemptFailed){
			continue;
		}
		bss_entry = &out_table->BssEntry[i];
		if (bss_entry->Rssi >= rssi_threshold && 
		(bss_entry->beacon_info.network_weight[0] == 0xF)){
			
			found_ap = TRUE;
			COPY_MAC_ADDR(ezdev->CfgApCliBssid, bss_entry->Bssid);
			EZ_DEBUG(DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_TRACE, 
				("%s(line.%d): found configured ap having weight 0xF.\n", 
				__FUNCTION__, __LINE__));
			goto found;
		}
	}

	/*Find configured AP, having the same SSID.*/
	/*Find configured AP. with good RSSI.*/
	for (i = 0; i < out_table->BssNr; i++) {
		if(out_table->BssEntry[i].bConnectAttemptFailed){
				//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
				//	("%s(line.%d) - Skip already attempted candidate.\n", 
				//	__FUNCTION__, __LINE__));
				continue;
		}
		bss_entry = &out_table->BssEntry[i];
		if (bss_entry->support_easy_setup 
			&& bss_entry->Rssi >= rssi_threshold){
			found_ap = TRUE;
			ezdev->support_ez_setup = TRUE;
			COPY_MAC_ADDR(ezdev->CfgApCliBssid, bss_entry->Bssid);
			EZ_DEBUG(DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_TRACE, 
				("%s(line.%d): found ap.\n", 
				__FUNCTION__, __LINE__));
			goto found;
		}
	}

	/*
		Select normal ap with RSSI less than threshold which doesn't support easy setup and matches the SSID.
	*/
	for (i = 0; i < out_table->BssNr; i++) {
		if(out_table->BssEntry[i].bConnectAttemptFailed){
				//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
				//	("%s(line.%d) - Skip already attempted candidate.\n", 
				//	__FUNCTION__, __LINE__));
				continue;
		}
		bss_entry = &out_table->BssEntry[i];
		if ((!bss_entry->support_easy_setup)
			&& (ezdev->CfgSsidLen != 0 && SSID_EQUAL(ezdev->CfgSsid, ezdev->CfgSsidLen,bss_entry->Ssid, bss_entry->SsidLen))){
			found_ap = TRUE;
			COPY_MAC_ADDR(ezdev->CfgApCliBssid, bss_entry->Bssid);
			ez_hex_dump("ApcliSsid",ezdev->CfgSsid,ezdev->CfgSsidLen );
			ez_hex_dump("BssSsid",bss_entry->Ssid,bss_entry->SsidLen);
			EZ_DEBUG(DBG_CAT_MLME, CATCLIENT_APCLI, DBG_LVL_TRACE, 
				("%s(line.%d): found ap.\n", 
				__FUNCTION__, __LINE__));
			goto found;
		}
	}


	/*Find configured Dedicated AP.*/
	for (i = 0; i < out_table->BssNr; i++) {
		if(out_table->BssEntry[i].bConnectAttemptFailed){
				//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
				//	("%s(line.%d) - Skip already attempted candidate.\n", 
				//	__FUNCTION__, __LINE__));
				continue;
		}
		bss_entry = &out_table->BssEntry[i];
		if ((bss_entry->beacon_info.network_weight[0] == 0xF) &&
			(bss_entry->beacon_info.node_number.path_len == MAC_ADDR_LEN)			
			//SSID_EQUAL(apcli_entry->Ssid, apcli_entry->SsidLen,bss_entry->Ssid, bss_entry->SsidLen)
			){
			found_ap = TRUE;
			COPY_MAC_ADDR(ezdev->CfgApCliBssid, bss_entry->Bssid);
			EZ_DEBUG(DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_TRACE, 
				("%s(line.%d): Found Dedicated MAN AP.\n", 
				__FUNCTION__, __LINE__));
			goto found;
		}
	}


	

	/*Find configured AP having weight 0x0F */
	for (i = 0; i < out_table->BssNr; i++) {
		if(out_table->BssEntry[i].bConnectAttemptFailed){
			continue;
		}
		bss_entry = &out_table->BssEntry[i];
		if (bss_entry->beacon_info.network_weight[0] == 0xF){
			
			found_ap = TRUE;
			COPY_MAC_ADDR(ezdev->CfgApCliBssid, bss_entry->Bssid);
			EZ_DEBUG(DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_TRACE, 
				("%s(line.%d): found configured ap having weight 0xF.\n", 
				__FUNCTION__, __LINE__));
			goto found;
		}
	}

	/*Find configured AP. with good RSSI.*/
	for (i = 0; i < out_table->BssNr; i++) {
		if(out_table->BssEntry[i].bConnectAttemptFailed){
				//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
				//	("%s(line.%d) - Skip already attempted candidate.\n", 
				//	__FUNCTION__, __LINE__));
				continue;
		}
		bss_entry = &out_table->BssEntry[i];
		if (bss_entry->support_easy_setup){
			found_ap = TRUE;
			ezdev->support_ez_setup = TRUE;
			COPY_MAC_ADDR(ezdev->CfgApCliBssid, bss_entry->Bssid);
			EZ_DEBUG(DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_TRACE, 
				("%s(line.%d): found ap.\n", 
				__FUNCTION__, __LINE__));
			goto found;
		}
	}

found:
	if((bss_entry!=NULL) && (found_ap == TRUE)){
		// for single chip DBDC operation, as common scan table is present for both band interfaces,
		// and as there is possbility for scan table update/clear,
		// do safety check that bss entry is having valid values
		if(MAC_ADDR_EQUAL(bss_entry->Bssid, ZERO_MAC_ADDR)){
			EZ_DEBUG(DBG_CAT_MLME, CATCLIENT_APCLI, DBG_LVL_ERROR,("(%s) Chosen BSS entry not Valid\n",__FUNCTION__));
			found_ap = FALSE;
			out_table->BssNr = 0;
		}
	}

	if (!found_ap) {
		EZ_DEBUG(DBG_CAT_MLME, CATCLIENT_APCLI, DBG_LVL_TRACE, 
			("(%s) No matching AP\n", __FUNCTION__));
		//apcli_entry->MlmeAux.attempted_candidate_index = EZ_INDEX_NOT_FOUND;
		ezdev->attempted_candidate_index = EZ_INDEX_NOT_FOUND;
		if(!ezdev->driver_ops->ez_is_timer_running(ezdev,ezdev->ez_security.ez_scan_timer)){
			ULONG time;
			UINT32 random_time = (UINT32)(ezdev->driver_ops->RandomByte(ezdev)*100);
			if(ezdev->ez_security.internal_force_connect_bssid == TRUE)
			{
				EZ_DEBUG(DBG_CAT_MLME, CATCLIENT_APCLI, DBG_LVL_TRACE, 
					("Duplicate Connection... Scan after  2 seconds\n"));
				ezdev->driver_ops->ez_set_timer(ezdev,ezdev->ez_security.ez_scan_timer, 2000);
				//ezdev->ez_security.ez_scan_timer_running = TRUE;
				//ApSiteSurvey_by_wdev(pAd, NULL, SCAN_ACTIVE, FALSE, wdev);
			}
			else if (ezdev->ez_security.ez_scan_same_channel == TRUE)
			{
				if (!MAC_ADDR_EQUAL(ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid, ZERO_MAC_ADDR))
				{
					if (ezdev->ez_security.ez_roam_info.roam_channel != 0 && 
						*ezdev->channel != ezdev->ez_security.ez_roam_info.roam_channel)
					{
						/*Move to the bss channel if not already set.*/
						EZ_DEBUG(DBG_CAT_MLME, CATCLIENT_APCLI, DBG_LVL_ERROR, 
							("Roam: Curr Chan=%d RoamChannel=%d\n",*ezdev->channel,ezdev->ez_security.ez_roam_info.roam_channel));
					//	ap_ezdev->ez_security.do_not_restart_interfaces = 1;
						ezdev->driver_ops->ez_rtmp_set_channel(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd
								,ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev.wdev, ezdev->ez_security.ez_roam_info.roam_channel);
					//	ap_ezdev->ez_security.do_not_restart_interfaces = 0;
					}
				}
				ezdev->driver_ops->ez_set_timer(ezdev,ezdev->ez_security.ez_scan_timer, EZ_MAX_SCAN_TIME_OUT/2);
			}
			else
			{
			if (random_time > EZ_MAX_SCAN_TIME_OUT)
				random_time = EZ_MAX_SCAN_TIME_OUT;
				ezdev->ez_security.ez_scan_delay +=2000;
				if (ezdev->ez_security.ez_scan_delay > (ezdev->ez_security.ez_max_scan_delay + EZ_SCAN_DELAY_WAIT))
					ezdev->ez_security.ez_scan_delay = ezdev->ez_security.ez_max_scan_delay;
				if(ezdev->ez_security.ez_scan_delay > EZ_SCAN_DELAY_WAIT)
					time = random_time + ezdev->ez_security.ez_scan_delay - EZ_SCAN_DELAY_WAIT;
				else
					time = random_time;

				if (time == 0)
				{
					time = 2000;
				}
				ezdev->driver_ops->ez_set_timer(ezdev,ezdev->ez_security.ez_scan_timer, time);
				EZ_DEBUG(DBG_CAT_MLME, CATCLIENT_APCLI, DBG_LVL_TRACE,("Scan Delay Timeout = %d\n",(UINT32)time));
		//	ezdev->ez_security.ez_scan_timer_running = TRUE;
		}
		}

#ifdef SYSTEM_LOG_SUPPORT
		RTMPSendWirelessEvent(pAd, IW_WH_EZ_CONFIGURED_AP_SEARCHING, NULL, wdev->wdev_idx, 0);
#else /* SYSTEM_LOG_SUPPORT */
		EzRtmpOSWrielessEventSend(ezdev, RT_WLAN_EVENT_CUSTOM, IW_WH_EZ_CONFIGURED_AP_SEARCHING,
							NULL, NULL, 0);
#endif /* !SYSTEM_LOG_SUPPORT */
		return FALSE;
	}
	else {
#ifdef SYSTEM_LOG_SUPPORT
		RTMPSendWirelessEvent(pAd, IW_WH_EZ_CONFIGURED_AP_FOUND, NULL, wdev->wdev_idx, 0);
#else /* SYSTEM_LOG_SUPPORT */
		EzRtmpOSWrielessEventSend(ezdev, RT_WLAN_EVENT_CUSTOM, IW_WH_EZ_CONFIGURED_AP_FOUND,
							NULL, NULL, 0);
#endif /* !SYSTEM_LOG_SUPPORT */
		/*if we need to create an easy setup connection, then adopt the bss parameters*/
		ez_hex_dump("FoundAP MAC", bss_entry->Bssid, MAC_ADDR_LEN);
		ezdev->ez_security.ez_scan_delay = 0;
#if 0
		if((*ezdev->channel > 14) && (((EZ_ADAPTER *)(ezdev->ez_ad))->band_count == 2))
		{
			EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n band non zero connection\n"));
			NdisMoveMemory(ez_adapter->Peer2p4mac, bss_entry->beacon_info.other_ap_mac, MAC_ADDR_LEN);
		}
		else
		{
			EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n band zero connection\n"));
			NdisMoveMemory(ez_adapter->Peer2p4mac, bss_entry->Bssid, MAC_ADDR_LEN);		
		}
		
		printk("\n---------  ez_adapter->Peer2p4mac ----------------\n");

		for (i = 0; i < 6; ++i)
		  printk(" %02x", ez_adapter->Peer2p4mac[i]);

		printk("\n---------  ez_adapter->Peer2p4mac ----------------\n");
#endif		
		if (bss_entry->support_easy_setup == TRUE)
		{
			ezdev->support_ez_setup = TRUE;
		}
		
		ezdev->attempted_candidate_index = i;
		if(ezdev->driver_ops->ez_update_cli_conn(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd,ezdev,bss_entry) == FALSE)
			return FALSE;

		ezdev->driver_ops->ez_update_partial_scan(ez_ad,ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.wdev);
// TODO: Raghav: channel change check. only if force channel not there.
	}

	return TRUE;

}



BOOLEAN ez_apcli_search_best_ap_hook(
	void *ez_ad_obj,
	void *ezdev_obj,
	void *out_tab)
{
	EZ_ADAPTER *ez_ad = ez_ad_obj;
	ez_dev_t *ezdev = ezdev_obj;
	//RTMP_ADAPTER *ad;
	//APCLI_STRUCT *apcli_entry;
	EZ_BSS_TABLE *out_table = out_tab;
#ifdef EZ_API_SUPPORT

	EZ_BSS_TABLE *tmp_out_tab;
#endif
	//struct wifi_dev *wdev;
	//MLME_AUX *apcli_mlme_aux;
	//PRTMP_ADAPTER pAd = ad_obj;
	int index;

	//ad = (RTMP_ADAPTER *)ad_obj;
	//apcli_entry = (APCLI_STRUCT *)apcli_entry_obj;
	//apcli_mlme_aux = &apcli_entry->MlmeAux;
	//wdev = &apcli_entry->wdev;

	EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_ERROR,(" %s -->%d\n", __FUNCTION__,ezdev->ez_band_idx));
#ifdef EZ_API_SUPPORT
	if (ezdev->ez_security.ez_api_mode != CONNECTION_OFFLOAD){
#endif

#ifdef EZ_REGROUP_SUPPORT
		if(wdev->regrp_mode == REGRP_MODE_BLOCKED){
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
						("%s(line.%d) - Provider search Blocked for Regroup on this wdev_idx : %d.\n", 
						__FUNCTION__, __LINE__, wdev->wdev_idx));
			return FALSE;		
		}
#endif
	{
		ez_dev_t *other_band_ezdev = ez_get_otherband_cli_ezdev(ezdev);
		if (other_band_ezdev){
			if(!MAC_ADDR_EQUAL(other_band_ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid, ZERO_MAC_ADDR)
				&& MAC_ADDR_EQUAL(ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid, ZERO_MAC_ADDR))
			{
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, 
							("%s(line.%d) - Roam Ongoing on other band but not on current, differ connection.\n", 
							__FUNCTION__, __LINE__));
				return FALSE;				
			}
		}

	}
#ifdef EZ_DUAL_BAND_SUPPORT
	{
		interface_info_t other_band_info;
		EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("get other_band_info\n"));
		if (ez_get_other_band_info(ezdev,&other_band_info)){
			EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("get other CLI peer\n"));
			ez_hex_dump("other_band_info.cli_peer_ap_mac", other_band_info.cli_peer_ap_mac, 6);
			if (!MAC_ADDR_EQUAL(other_band_info.cli_peer_ap_mac, ZERO_MAC_ADDR)
				&& ! ezdev->ez_security.internal_force_connect_bssid_timeout
				&& MAC_ADDR_EQUAL(ezdev->ez_security.ez_apcli_force_bssid,ZERO_MAC_ADDR)
				&& MAC_ADDR_EQUAL(ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid,ZERO_MAC_ADDR)
				&& other_band_info.non_easy_connection == FALSE)
			{
					if (!ezdev->ez_security.internal_force_connect_bssid) {
						NdisGetSystemUpTime(&ezdev->ez_security.force_connect_bssid_time_stamp);
						ezdev->ez_security.internal_force_connect_bssid = TRUE;

						// reset pAd scan tab
						//pAd->ScanTab.BssNr = 0;
						ez_initiate_new_scan(ez_ad);
						out_table->BssNr = 0;
					}
			}
		}
	}
#endif

	if (ezdev->driver_ops->ez_is_timer_running(ezdev,ezdev->ez_security.ez_scan_timer)) {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
					("%s(line.%d) - Wait for next scan.\n", 
					__FUNCTION__, __LINE__));
		return FALSE;
	}

		if (ez_ad->ez_connect_wait_ezdev->driver_ops->ez_is_timer_running(ezdev,ez_ad->ez_connect_wait_timer)) {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
					("%s(line.%d) - Wait for connection allow.\n", 
					__FUNCTION__, __LINE__));
		return FALSE;
	}
		
		{
		ez_dev_t *ap_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev;
			//! first send an action frame to EZ peers so that they do not disconnect
			for (index = 0; index < EZ_MAX_STA_NUM; index ++)
			{
				if (ez_ad->ez_band_info[ezdev->ez_band_idx].ez_peer_table[index].port_secured){
					send_action_delay_disconnect(ez_ad, ap_ezdev,
						&ez_ad->ez_band_info[ezdev->ez_band_idx].ez_peer_table[index],
						ez_ad->ez_delay_disconnect_count);
				}
				
			}

		}

		//if(ez_ad->configured_status == EZ_CONFIGURED) //(EZ_GET_CAP_CONFIGRED(wdev->ez_security.capability))
		{
			return ez_apcli_search_best_ap_configured(ez_ad_obj,ezdev_obj,out_table);
		}
#if 0
		else if (ez_adapter.configured_status == EZ_UNCONFIGURED)
		{
			return ez_apcli_search_best_ap_unconfigured(ad,out_table,apcli_entry);
		}
#endif
#ifdef EZ_API_SUPPORT
		}
	else {
		
		ez_os_alloc_mem(NULL, (PUCHAR *)&tmp_out_tab, sizeof(EZ_BSS_TABLE));
		if(tmp_out_tab == NULL)
			return FALSE;
		ez_BssTableInit(tmp_out_tab);
		ezdev->driver_ops->ez_get_scan_table(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd,tmp_out_tab);
		if (!MAC_ADDR_EQUAL(ezdev->ez_security.ez_apcli_force_bssid,ZERO_MAC_ADDR))
		{
			ULONG bss_index;
			EZ_BSS_ENTRY *bss_entry;
			bss_index = ez_BssTableSearchWithBssId(tmp_out_tab, ezdev->ez_security.ez_apcli_force_bssid, 0);
			if (bss_index == BSS_NOT_FOUND)
			{
				// TODO: Hasan send failure to host
#ifdef SYSTEM_LOG_SUPPORT
			RTMPSendWirelessEvent(ad, IW_WH_EZ_MY_APCLI_DISCONNECTED, NULL, wdev->wdev_idx, 0);
#else /* SYSTEM_LOG_SUPPORT */
			EzRtmpOSWrielessEventSend(ezdev, RT_WLAN_EVENT_CUSTOM, IW_WH_EZ_MY_APCLI_DISCONNECTED,
						    NULL, NULL, 0);
#endif /* !SYSTEM_LOG_SUPPORT */
				return FALSE;
			} else {
				/*
					Provider found and I am joiner.
				*/
				bss_entry = &out_table->BssEntry[bss_index];
				ezdev->attempted_candidate_index = bss_index;
				if (ezdev->driver_ops->ez_update_cli_conn(ez_ad->ez_band_info[ezdev->ez_band_idx].pAd,ezdev,bss_entry) == FALSE)
				{
					ez_os_free_mem(tmp_out_tab);
					tmp_out_tab = NULL;
					return FALSE;
				}
#if 0
				COPY_MAC_ADDR(apcli_entry->CfgApCliBssid, bss_entry->Bssid);
				NdisZeroMemory(apcli_entry->CfgSsid, MAX_LEN_OF_SSID);
				NdisMoveMemory(apcli_entry->CfgSsid, bss_entry->Ssid, bss_entry->SsidLen);
				NdisMoveMemory(apcli_entry->MlmeAux.Ssid, bss_entry->Ssid, bss_entry->SsidLen);

				apcli_entry->CfgSsidLen = bss_entry->SsidLen;
				apcli_entry->MlmeAux.SsidLen = bss_entry->SsidLen;
				
				wdev->SecConfig.AKMMap = bss_entry->AKMMap;
				wdev->SecConfig.PairwiseCipher = bss_entry->PairwiseCipher;
				wdev->SecConfig.GroupCipher = bss_entry->GroupCipher;

				ap_wdev = &pAd->ApCfg.MBSSID[wdev->func_idx].wdev;

#ifdef EZ_NETWORK_MERGE_SUPPORT
				
				ez_ApCliAutoConnectBWAdjust(pAd, wdev, bss_entry);				
				ez_ApCliAutoConnectBWAdjust(pAd, ap_wdev, bss_entry);
				
				ap_wdev->ez_security.do_not_restart_interfaces = 1;
				rtmp_set_channel(pAd, ap_wdev, bss_entry->Channel);
				ap_wdev->ez_security.do_not_restart_interfaces = 0;
				
#endif				
				apcli_mlme_aux->attempted_candidate_index = bss_index;
#endif
				ez_os_free_mem(tmp_out_tab);
				tmp_out_tab = NULL;

				return TRUE;

			}
		} else {
			// TODO: Hasan send failure to host	
#ifdef SYSTEM_LOG_SUPPORT
						RTMPSendWirelessEvent(ad, IW_WH_EZ_MY_APCLI_DISCONNECTED, NULL, wdev->wdev_idx, 0);
#else /* SYSTEM_LOG_SUPPORT */
						EzRtmpOSWrielessEventSend(ezdev, RT_WLAN_EVENT_CUSTOM, IW_WH_EZ_MY_APCLI_DISCONNECTED,
										NULL, NULL, 0);
#endif /* !SYSTEM_LOG_SUPPORT */

			return FALSE;
		}
	}
#endif
	return FALSE;
}

#ifdef EZ_DFS_SUPPORT
BOOLEAN ez_update_channel_from_csa_hook(ez_dev_t *ezdev, UCHAR Channel)
{
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	BOOLEAN switch_channel = FALSE;
	ezdev->ez_security.this_band_info.shared_info.channel_info.channel = Channel;

	if (ezdev->ezdev_type == EZDEV_TYPE_APCLI)
	{
		ez_dev_t *ap_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev;
		ez_dev_t *other_band_ezdev = NULL;
		
		ap_ezdev->ez_security.this_band_info.shared_info.channel_info.channel = Channel;
		if(ezdev->ez_security.this_band_info.shared_info.link_duplicate)
		{
			
		}
		else {
			ezdev->driver_ops->ez_send_unicast_deauth(ezdev, ezdev->bssid);
			switch_channel = TRUE;
		}
	}
	return switch_channel;
}


void ez_reinit_configs_hook(void *ez_dev)
{	
	ez_dev_t *ezdev = ez_dev;
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	if (ez_ad->device_info.ez_node_number.path_len == 0x6)
		update_and_push_weight(ezdev, NULL, NULL);
}

EXPORT_SYMBOL(ez_update_channel_from_csa_hook);
EXPORT_SYMBOL(ez_reinit_configs_hook);
#endif


EXPORT_SYMBOL(ez_build_beacon_ie_hook);
EXPORT_SYMBOL(ez_init_hook);
EXPORT_SYMBOL(ez_get_adapter_hook);
EXPORT_SYMBOL(ez_build_probe_request_ie_hook);
EXPORT_SYMBOL(ez_build_probe_response_ie_hook);
EXPORT_SYMBOL(ez_build_auth_request_ie_hook);
EXPORT_SYMBOL(ez_build_auth_response_ie_hook);
EXPORT_SYMBOL(ez_build_assoc_request_ie_hook);
EXPORT_SYMBOL(ez_build_assoc_response_ie_hook);
EXPORT_SYMBOL(ez_process_probe_request_hook);
EXPORT_SYMBOL(ez_process_beacon_probe_response_hook);
EXPORT_SYMBOL(ez_process_auth_request_hook);
EXPORT_SYMBOL(ez_process_auth_response_hook);
EXPORT_SYMBOL(ez_process_assoc_request_hook);
EXPORT_SYMBOL(ez_process_assoc_response_hook);
EXPORT_SYMBOL(ez_show_information_hook);
EXPORT_SYMBOL(ez_send_broadcast_deauth_proc_hook);
EXPORT_SYMBOL(ez_set_ezgroup_id_hook);
EXPORT_SYMBOL(ez_set_group_id_hook);
EXPORT_SYMBOL(ez_set_gen_group_id_hook);
EXPORT_SYMBOL(ez_set_rssi_threshold_hook);
EXPORT_SYMBOL(ez_set_max_scan_delay_hook);
EXPORT_SYMBOL(ez_set_api_mode_hook);
EXPORT_SYMBOL(ez_merge_group_hook);
EXPORT_SYMBOL(ez_apcli_force_ssid_hook);
EXPORT_SYMBOL(ez_set_force_bssid_hook);
EXPORT_SYMBOL(ez_set_push_bw_hook);
EXPORT_SYMBOL(ez_handle_action_txstatus_hook);
EXPORT_SYMBOL(set_ssid_psk_hook);
EXPORT_SYMBOL(ez_apcli_link_down_hook);
EXPORT_SYMBOL(ez_update_connection_permission_hook);
EXPORT_SYMBOL(ez_is_connection_allowed_hook);
EXPORT_SYMBOL(ez_probe_rsp_join_action_hook);
EXPORT_SYMBOL(ez_update_connection_hook);
EXPORT_SYMBOL(ez_handle_pairmsg4_hook);
EXPORT_SYMBOL(ez_roam_hook);
EXPORT_SYMBOL(ez_set_roam_bssid_hook);
EXPORT_SYMBOL(ez_reset_roam_bssid_hook);
EXPORT_SYMBOL(ez_get_push_bw_hook);
EXPORT_SYMBOL(ez_get_channel_hook);
EXPORT_SYMBOL(ez_did_ap_fallback_hook);
EXPORT_SYMBOL(ez_ap_fallback_channel);
EXPORT_SYMBOL(ez_prepare_security_key_hook);
EXPORT_SYMBOL(ez_exit_hook);
EXPORT_SYMBOL(ez_process_action_frame_hook);
EXPORT_SYMBOL(check_best_ap_rssi_threshold_hook);
EXPORT_SYMBOL(ez_set_ap_fallback_context_hook);
EXPORT_SYMBOL(ez_peer_table_search_by_addr_hook);
EXPORT_SYMBOL(ez_is_triband_hook);
EXPORT_SYMBOL(ez_check_for_ez_enable_hook);
EXPORT_SYMBOL(ez_acquire_lock_hook);
EXPORT_SYMBOL(ez_release_lock_hook);
EXPORT_SYMBOL(ez_is_weight_same_hook);
EXPORT_SYMBOL(ez_is_other_band_mlme_running_hook);
EXPORT_SYMBOL(ez_triband_insert_tlv_hook);
EXPORT_SYMBOL(ez_handle_peer_disconnection_hook);
EXPORT_SYMBOL(ez_sta_rx_pkt_handle_hook);
EXPORT_SYMBOL(ez_apcli_rx_grp_pkt_drop_hook);
EXPORT_SYMBOL(ez_apcli_tx_grp_pkt_drop_hook);
EXPORT_SYMBOL(send_delay_disconnect_to_peers_hook);
EXPORT_SYMBOL(ez_internet_msghandle_hook);
EXPORT_SYMBOL(ez_custom_data_handle_hook);
EXPORT_SYMBOL(ez_is_roaming_ongoing_hook);
EXPORT_SYMBOL(ez_peer_table_maintenance_hook);
EXPORT_SYMBOL(ez_port_secured_hook);
EXPORT_SYMBOL(ez_ap_peer_beacon_action_hook);
EXPORT_SYMBOL(ez_handle_send_packets_hook);
EXPORT_SYMBOL(ez_set_open_group_id_hook);
EXPORT_SYMBOL(APTribandRestartNonEzReqAction_hook);
EXPORT_SYMBOL(ez_ap_tx_grp_pkt_drop_to_ez_apcli_hook);
EXPORT_SYMBOL(ez_start_hook);
EXPORT_SYMBOL(ez_stop_hook);
EXPORT_SYMBOL(ez_connection_allow_all_hook);
EXPORT_SYMBOL(ez_scan_timeout_hook);
//EXPORT_SYMBOL(ez_stop_scan_timeout_hook);
EXPORT_SYMBOL(ez_group_merge_timeout_hook);
EXPORT_SYMBOL(ez_loop_chk_timeout_hook);
EXPORT_SYMBOL(ez_need_bypass_rx_fwd_hook);
EXPORT_SYMBOL(increment_best_ap_rssi_threshold_hook);
EXPORT_SYMBOL(ez_allocate_or_update_non_ez_band_hook);
EXPORT_SYMBOL(ez_initiate_new_scan_hook);
EXPORT_SYMBOL(ez_apcli_search_best_ap_hook);

#ifdef IF_UP_DOWN
EXPORT_SYMBOL(ez_check_valid_hook);
EXPORT_SYMBOL(ez_all_intf_up_hook);
EXPORT_SYMBOL(ez_apcli_disconnect_both_intf_hook);
#endif
#endif	

static int ez_init_mod(void)
{
	printk("#############################################\n");
	printk("%s\n", __FUNCTION__);
	printk("#############################################\n");
	return 0;
}


static void ez_cleanup_mod(void)
{
	printk("#############################################\n");
	printk("%s\n", __FUNCTION__);
	printk("#############################################\n");
}


module_init(ez_init_mod);
module_exit(ez_cleanup_mod);

MODULE_AUTHOR("MediaTek Inc");
MODULE_LICENSE("Proprietary");
MODULE_DESCRIPTION("MediaTek Easy Setup Support Module\n"); 

