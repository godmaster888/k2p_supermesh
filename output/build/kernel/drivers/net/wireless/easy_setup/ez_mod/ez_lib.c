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
#include "linux/delay.h"
 
#include "ez_hooks_proto.h"
#include "ez_lib_proto.h"

 unsigned char __DH_G_VALUE[1] = {0x02};
 unsigned char __DH_P_VALUE[192] =
{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
    0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1,
    0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
    0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22,
    0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
    0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B,
    0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37,
    0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45,
    0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6,
    0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B,
    0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
    0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5,
    0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
    0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D,
    0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05,
    0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A,
    0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
    0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96,
    0x1C, 0x62, 0xF3, 0x56, 0x20, 0x85, 0x52, 0xBB,
    0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D,
    0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04,
    0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x23, 0x73, 0x27,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

extern UCHAR ZERO_MAC_ADDR[MAC_ADDR_LEN];  
unsigned char mtk_oui[MTK_OUI_LEN] = {0x00, 0x0C, 0xE7};
unsigned char ralink_oui[RALINK_OUI_LEN] = {0x00, 0x0C, 0x43};

inline BOOLEAN ez_is_ap_apcli(ez_dev_t *ezdev)
{
	if(ezdev->ezdev_type == EZDEV_TYPE_AP || ezdev->ezdev_type == EZDEV_TYPE_APCLI)
		return TRUE;
	else
		return FALSE;
}

unsigned char ez_gen_dh_public_key(
	ez_dev_t *ezdev)
{
	struct _ez_security *ez_sec_info = &ezdev->ez_security;
	
	unsigned char tmp_key[EZ_RAW_KEY_LEN];
	int idx;
	unsigned int dh_len;
	unsigned int DiffCnt;

	
	dh_len = EZ_RAW_KEY_LEN;
	
	/* Enrollee 192 random bytes for DH key generation */
	for (idx = 0; idx < EZ_RAW_KEY_LEN; idx++) {
		ez_sec_info->self_dh_random_seed[idx] = ezdev->driver_ops->RandomByte(ezdev);
	}
	NdisZeroMemory(&ez_sec_info->self_pke[0], EZ_RAW_KEY_LEN);
	printk("[EZ_MOD]DH_PublicKey_Generate address is %p", (void *)ezdev->driver_ops->DH_PublicKey_Generate);
	ezdev->driver_ops->DH_PublicKey_Generate(
		ezdev,
		__DH_G_VALUE, sizeof(__DH_G_VALUE),
		__DH_P_VALUE, sizeof(__DH_P_VALUE),
		&ez_sec_info->self_dh_random_seed[0], EZ_RAW_KEY_LEN,
		&ez_sec_info->self_pke[0], (UINT *) &dh_len);

	/* Need to prefix zero padding */
	if (dh_len < EZ_RAW_KEY_LEN)
	{
		DiffCnt = EZ_RAW_KEY_LEN - dh_len;

		NdisZeroMemory(&tmp_key[0], EZ_RAW_KEY_LEN);
		NdisCopyMemory(&tmp_key[DiffCnt], &ez_sec_info->self_pke[0], dh_len);
		NdisCopyMemory(&ez_sec_info->self_pke[0], &tmp_key[0], EZ_RAW_KEY_LEN);
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("%s: Do zero padding!\n", __FUNCTION__));
	}
	else if (dh_len > EZ_RAW_KEY_LEN) {
		
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("%s: dh_len(=%d) exceeds EZ_RAW_KEY_LEN(=%d)!\n", 
			__FUNCTION__,
			dh_len,
			EZ_RAW_KEY_LEN));

		return FALSE;
	}

	return TRUE;
}



 unsigned char ez_gen_dh_private_key(
	ez_dev_t *ezdev,
	struct _ez_peer_security_info *ez_peer)
{
	struct _ez_security *ez_sec_info = &ezdev->ez_security;
	unsigned char tmp_key[EZ_RAW_KEY_LEN];
	unsigned int dh_len;
	unsigned int DiffCnt;

	dh_len = EZ_RAW_KEY_LEN;
	
	NdisZeroMemory(&ez_sec_info->self_pkr[0], EZ_RAW_KEY_LEN);
	ez_peer->ezdev->driver_ops->RT_DH_SecretKey_Generate(ezdev,
		&ez_peer->peer_pke[0], EZ_RAW_KEY_LEN,
		__DH_P_VALUE, sizeof(__DH_P_VALUE),
		&ez_sec_info->self_dh_random_seed[0],  sizeof(ez_sec_info->self_dh_random_seed),
		&ez_sec_info->self_pkr[0], (UINT *) &dh_len);
	
	/* Need to prefix zero padding */
	if (dh_len < EZ_RAW_KEY_LEN)
	{
		DiffCnt = EZ_RAW_KEY_LEN - dh_len;

		NdisZeroMemory(&tmp_key[0], EZ_RAW_KEY_LEN);
		NdisCopyMemory(&tmp_key[DiffCnt], &ez_sec_info->self_pkr[0], dh_len);
		NdisCopyMemory(&ez_sec_info->self_pkr[0], &tmp_key[0], EZ_RAW_KEY_LEN);
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("%s: Do zero padding!\n", __FUNCTION__));
	}
	else if (dh_len > EZ_RAW_KEY_LEN) {
		
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("%s: dh_len(=%d) exceeds EZ_RAW_KEY_LEN(=%d)!\n", 
			__FUNCTION__,
			dh_len,
			EZ_RAW_KEY_LEN));

		return FALSE;
	}
	return TRUE;
}


 void ez_compute_dh_key(
	ez_dev_t *ezdev,
	struct _ez_peer_security_info *ez_peer)
{
	struct _ez_security *ez_sec = &ezdev->ez_security;
	
	NdisZeroMemory(&ez_peer->dh_key[0], EZ_DH_KEY_LEN);
	/* Compute the DHKey based on the DH secret */
	ez_peer->ezdev->driver_ops->RT_SHA256(ezdev, &ez_sec->self_pkr[0], EZ_RAW_KEY_LEN, &ez_peer->dh_key[0]);
	//ez_hex_dump("DH Key", &ez_peer->dh_key[0], EZ_DH_KEY_LEN);
}


 void ez_get_sw_encrypted_key(
	struct _ez_security *ez_sec_info,
	struct _ez_peer_security_info *ez_peer,
	unsigned char *a_addr,
	unsigned char *s_addr)
{
	
	
	unsigned char addr1[MAC_ADDR_LEN];
	unsigned char addr2[MAC_ADDR_LEN];

	NdisZeroMemory(&ez_peer->sw_key[0], EZ_PTK_LEN);
	NdisCopyMemory(&addr1[0], &mtk_oui[0], MTK_OUI_LEN);
	NdisCopyMemory(&addr1[MTK_OUI_LEN], &a_addr[MTK_OUI_LEN], MTK_OUI_LEN);
	NdisCopyMemory(&addr2[0], &ralink_oui[0], RALINK_OUI_LEN);
	NdisCopyMemory(&addr2[RALINK_OUI_LEN], &s_addr[RALINK_OUI_LEN], RALINK_OUI_LEN);
	ez_peer->ezdev->driver_ops->WpaDerivePTK(ez_peer->ezdev,
			&ez_peer->dh_key[0], 
			&ez_peer->dh_key[0], 
			&addr1[0], 
			&ez_peer->dh_key[16], 
			&addr2[0], 
			&ez_peer->sw_key[0], 
			LEN_PTK);;
	//ez_hex_dump("dh_key", &ez_peer->dh_key[0], EZ_DH_KEY_LEN);
	//ez_hex_dump("addr1", &addr1[0], 6);
	//ez_hex_dump("addr2", &addr2[0], 6);
	//ez_hex_dump("sw_key", &ez_peer->sw_key[0], LEN_PTK);
}


 void ez_calculate_mic(
 	ez_dev_t *ezdev,
	unsigned char *sw_key,
	unsigned char *msg,
	unsigned int msg_len,
	unsigned char *mic)
{
	unsigned char	digest[80];

	NdisZeroMemory(mic, sizeof(mic));
	NdisZeroMemory(digest, sizeof(digest));

	ezdev->driver_ops->RT_HMAC_SHA1(ezdev, sw_key, LEN_PTK_KCK, msg,  msg_len, digest, SHA1_DIGEST_SIZE);
	NdisCopyMemory(mic, digest, EZ_MIC_LEN);
}

 unsigned short ez_probe_request_sanity(
	void *msg,
	unsigned long msg_len,
	struct _ez_peer_security_info *ez_peer)
{	
	unsigned char tag_count;
	FRAME_802_11 *Fr;
	unsigned char *Ptr;
	EID_STRUCT *pEid;
	unsigned long Length;
	unsigned char tag;
	unsigned char data_len;
	unsigned char capability0;

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
						if (tag == EZ_TAG_SDH_PUBLIC_KEY) {
							if (data_len == EZ_RAW_KEY_LEN) {
								NdisCopyMemory(&ez_peer->peer_pke[0], 
									&pEid->Octet[EZ_TAG_DATA_OFFSET], 
									data_len);
								tag_count++;
							} else {
								return EZ_STATUS_CODE_INVALID_DATA;
							}
						}
#ifdef EZ_NETWORK_MERGE_SUPPORT
						else if (tag == EZ_TAG_CAPABILITY_INFO) {
						//! aditional capability tag is added in probe request so that AP is aware of CLIs group merge capability
							if (data_len == EZ_CAPABILITY_LEN) {
								NdisCopyMemory(&ez_peer->capability, 
									&pEid->Octet[EZ_TAG_DATA_OFFSET], 
									EZ_CAPABILITY_LEN);
								
								ez_peer->capability = be2cpu32(ez_peer->capability);
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

	if (tag_count == EZ_PROB_REQ_TAG_COUNT) {
		return EZ_STATUS_CODE_SUCCESS;
	}
	else
		return EZ_STATUS_CODE_INVALID_DATA;
}


 unsigned short ez_probe_beacon_response_sanity(
	void *msg,
	unsigned long msg_len,
	struct _ez_peer_security_info *ez_peer)
{	
	unsigned char tag_count;
	unsigned char target_tag_count;
	FRAME_802_11 *Fr;
	unsigned char *Ptr;
	EID_STRUCT *pEid;
	unsigned long Length;
	unsigned char tag;
	unsigned char data_len;
	unsigned char capability0;

	tag_count = 0;
	Length = 0;
	Fr = (PFRAME_802_11)msg;

	if (Fr->Hdr.FC.SubType == SUBTYPE_PROBE_RSP) {
		target_tag_count = EZ_PROB_RSP_TAG_COUNT;
	}
	else
		target_tag_count = EZ_BEACON_TAG_COUNT;
	
	Ptr = Fr->Octet;
	Length += LENGTH_802_11;
	
	Ptr += (TIMESTAMP_LEN + 4);
	Length += (TIMESTAMP_LEN + 4);
	
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
						if (tag == EZ_TAG_ADH_PUBLIC_KEY) {
							if (data_len == EZ_RAW_KEY_LEN) {
								NdisCopyMemory(&ez_peer->peer_pke[0], 
									&pEid->Octet[EZ_TAG_DATA_OFFSET], 
									data_len);
								tag_count++;
							}
							else
								return EZ_STATUS_CODE_INVALID_DATA;
						}							
						if (tag == EZ_TAG_CAPABILITY_INFO) {
							if (data_len == EZ_CAPABILITY_LEN) {
								NdisCopyMemory(&ez_peer->capability, 
									&pEid->Octet[EZ_TAG_DATA_OFFSET], 
									EZ_CAPABILITY_LEN);
								ez_peer->capability = be2cpu32(ez_peer->capability);
								tag_count++;
							}
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

	if (tag_count == target_tag_count) {
		return EZ_STATUS_CODE_SUCCESS;
	}
	else
		return EZ_STATUS_CODE_INVALID_DATA;
}

 unsigned short ez_auth_request_sanity(
	void *msg,
	unsigned long msg_len,
	struct _ez_peer_security_info *ez_peer)
{	
	unsigned char tag_count;
	FRAME_802_11 *Fr;
	unsigned char *Ptr;
	EID_STRUCT *pEid;
	unsigned long Length;
	unsigned char tag;
	unsigned char data_len;
	unsigned char capability0;
	unsigned char *data_buf;
	unsigned int data_buf_len;
	unsigned char mic[EZ_MIC_LEN];

	data_buf_len = EZ_AUTH_REQ_TAG_COUNT*255;
	EZ_MEM_ALLOC(NULL, &data_buf, data_buf_len);

	if (data_buf == NULL) {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
						("%s() - malloc fail.\n", __FUNCTION__));
		return EZ_STATUS_CODE_NO_RESOURCE;
	}
	
	NdisZeroMemory(data_buf, data_buf_len);
	NdisZeroMemory(&mic[0], EZ_MIC_LEN);
	data_buf_len = 0;
	
	tag_count = 0;
	Length = 0;
	Fr = (PFRAME_802_11)msg;		
	Ptr = Fr->Octet;
	Length += LENGTH_802_11;
	
	Ptr += 6;
	Length += 6;
	
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
						if (tag == EZ_TAG_SNONCE) {
							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							if (data_len == EZ_NONCE_LEN){
								NdisCopyMemory(&ez_peer->peer_nonce[0], 
									&pEid->Octet[EZ_TAG_DATA_OFFSET], 
									data_len);
								tag_count++;
							}
							else {
								if (data_buf)
									EZ_MEM_FREE(data_buf);
								return EZ_STATUS_CODE_INVALID_DATA;
							}
						}
						if (tag == EZ_TAG_GROUP_ID) {
							unsigned char *encrypted_data;
							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							if (ez_peer->group_id_len != 0) {
								EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO, 
									("%s() - free previous group id\n", __FUNCTION__));
								ez_peer->group_id_len = 0;
								EZ_MEM_FREE(ez_peer->group_id);
							}
							EZ_MEM_ALLOC(NULL, &ez_peer->group_id, data_len);
							if (ez_peer->group_id) {
								NdisZeroMemory(ez_peer->group_id, data_len);
								encrypted_data = &pEid->Octet[EZ_TAG_DATA_OFFSET];			
								/* AES */
								ez_peer->ezdev->driver_ops->AES_Key_Unwrap(ez_peer->ezdev,
												encrypted_data, data_len,
											   &ez_peer->sw_key[0], LEN_PTK_KEK, 
											   ez_peer->group_id, &ez_peer->group_id_len);
								tag_count++;
							}
							else {
								EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
									("%s() - malloc failed\n", __FUNCTION__));
								if (data_buf)
									EZ_MEM_FREE(data_buf);
								return EZ_STATUS_CODE_NO_RESOURCE;
							}
						}

						if (tag == EZ_TAG_OPEN_GROUP_ID) {
							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							ez_peer->open_group_id_len = pEid->Octet[EZ_TAG_LEN_OFFSET];
							NdisCopyMemory(ez_peer->open_group_id, &pEid->Octet[EZ_TAG_DATA_OFFSET], pEid->Octet[EZ_TAG_LEN_OFFSET]);
							tag_count++;
						}

						if (tag == EZ_TAG_MIC) {
							ez_calculate_mic(ez_peer->ezdev, ez_peer->sw_key, data_buf, data_buf_len, &mic[0]);
							if (NdisEqualMemory(&mic[0], &pEid->Octet[EZ_TAG_DATA_OFFSET], EZ_MIC_LEN)) {
								tag_count++;
							}
							else {
								EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
									("%s() - MIC different......\n", __FUNCTION__));
							}
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

	if (data_buf)
		EZ_MEM_FREE( data_buf);
	if (tag_count == EZ_AUTH_REQ_TAG_COUNT) {
		return EZ_STATUS_CODE_SUCCESS;
	}
	else
		return EZ_STATUS_CODE_INVALID_DATA;
}

 unsigned short ez_auth_response_sanity(
	void *msg,
	unsigned long msg_len,
	struct _ez_peer_security_info *ez_peer)
{	
	unsigned char tag_count;
	FRAME_802_11 *Fr;
	unsigned char *Ptr;
	EID_STRUCT *pEid;
	unsigned long Length;
	unsigned char tag;
	unsigned char data_len;
	unsigned char capability0;
	unsigned char *data_buf;
	unsigned int data_buf_len;
	unsigned char mic[EZ_MIC_LEN];
	unsigned char ez_this_band_psk_len;
	
	data_buf_len = EZ_AUTH_RSP_TAG_COUNT*255;
	EZ_MEM_ALLOC(NULL, &data_buf, data_buf_len);

	if (data_buf == NULL) {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
						("%s() - malloc fail.\n", __FUNCTION__));
		return EZ_STATUS_CODE_NO_RESOURCE;
	}

	NdisZeroMemory(data_buf, data_buf_len);
	NdisZeroMemory(&mic[0], EZ_MIC_LEN);
	data_buf_len = 0;

	Length = 0;
	Fr = (PFRAME_802_11)msg;
	Ptr = Fr->Octet;
	Length += LENGTH_802_11;
	
	Ptr += 6;
	Length += 6;
	
	pEid = (PEID_STRUCT) Ptr;

	tag_count = 0;
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
						if (tag == EZ_TAG_PMK) {
							unsigned char *encrypted_data;
							unsigned char *pmk;
							unsigned int pmk_len;
							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							encrypted_data = &pEid->Octet[EZ_TAG_DATA_OFFSET];
							EZ_MEM_ALLOC(NULL, &pmk, data_len+EZ_AES_KEY_ENCRYPTION_EXTEND);
							if (pmk) {
								/* AES */
								ez_peer->ezdev->driver_ops->AES_Key_Unwrap(ez_peer->ezdev,
												encrypted_data, data_len,
											   &ez_peer->sw_key[0], LEN_PTK_KEK, 
											   pmk, &pmk_len);
								NdisZeroMemory(&ez_peer->this_band_info.pmk[0], EZ_PMK_LEN);
								NdisCopyMemory(&ez_peer->this_band_info.pmk[0], pmk, EZ_PMK_LEN);
								tag_count++;
								EZ_MEM_FREE( pmk);
							}
						}

						if (tag == EZ_TAG_PSK_LEN) {
							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							ez_this_band_psk_len = pEid->Octet[EZ_TAG_DATA_OFFSET];
							tag_count++;
						}

						if (tag == EZ_TAG_PSK) {
							unsigned char *encrypted_data;
							unsigned char *psk;
							unsigned int psk_len;
							NdisCopyMemory(data_buf + data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							encrypted_data = &pEid->Octet[EZ_TAG_DATA_OFFSET];
							EZ_MEM_ALLOC(NULL, &psk, data_len + EZ_AES_KEY_ENCRYPTION_EXTEND);
							if (psk) {
								/* AES */
								ez_peer->ezdev->driver_ops->AES_Key_Unwrap(ez_peer->ezdev,
									encrypted_data, data_len,
									&ez_peer->sw_key[0], LEN_PTK_KEK, 
									psk, &psk_len);
								NdisZeroMemory(&ez_peer->this_band_info.psk[0], EZ_LEN_PSK);
								NdisCopyMemory(&ez_peer->this_band_info.psk[0], psk, ez_this_band_psk_len);
								tag_count++;
								EZ_MEM_FREE( psk);
							}
						}

						if (tag == EZ_TAG_ANONCE) {
							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							if (data_len == EZ_NONCE_LEN) {
								NdisCopyMemory(&ez_peer->peer_nonce[0], 
									&pEid->Octet[EZ_TAG_DATA_OFFSET], 
									data_len);
								tag_count++;
							}
							else {
								if (data_buf)
									EZ_MEM_FREE( data_buf);
								return EZ_STATUS_CODE_INVALID_DATA;
							}
						}
						if (tag == EZ_TAG_GROUP_ID) {
							unsigned char *encrypted_data;
							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							EZ_MEM_ALLOC(NULL, &ez_peer->group_id, data_len);
							if (ez_peer->group_id) {
								NdisZeroMemory(ez_peer->group_id, data_len);
								encrypted_data = &pEid->Octet[EZ_TAG_DATA_OFFSET];			
								/* AES */
								
								ez_peer->ezdev->driver_ops->AES_Key_Unwrap(ez_peer->ezdev,
												encrypted_data, data_len,
											   &ez_peer->sw_key[0], LEN_PTK_KEK, 
											   ez_peer->group_id, &ez_peer->group_id_len);								
								tag_count++;
							}
						}

						if (tag == EZ_TAG_OPEN_GROUP_ID) {
							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							ez_peer->open_group_id_len = pEid->Octet[EZ_TAG_LEN_OFFSET];
							NdisCopyMemory(ez_peer->open_group_id, &pEid->Octet[EZ_TAG_DATA_OFFSET], pEid->Octet[EZ_TAG_LEN_OFFSET]);
							tag_count++;
						}

						if (tag == EZ_TAG_MIC) {
							ez_calculate_mic(ez_peer->ezdev, ez_peer->sw_key, data_buf, data_buf_len, &mic[0]);
							if (NdisEqualMemory(&mic[0], &pEid->Octet[EZ_TAG_DATA_OFFSET], EZ_MIC_LEN)) {
								tag_count++;
							}
							else {
								EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
									("%s() - MIC different......\n", __FUNCTION__));
							}
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

	if (data_buf)
		EZ_MEM_FREE( data_buf);
	
	if (tag_count >= EZ_AUTH_RSP_TAG_COUNT) {
		return EZ_STATUS_CODE_SUCCESS;
	}
	else
		return EZ_STATUS_CODE_INVALID_DATA;
}


 unsigned short ez_assoc_request_sanity(
	unsigned char isReassoc,
	void *msg,
	unsigned long msg_len,
	struct _ez_peer_security_info *ez_peer)
{
	unsigned char tag_count;
	FRAME_802_11 *Fr;
	unsigned char *Ptr;
	EID_STRUCT *pEid;
	unsigned long Length;
	unsigned char tag;
	unsigned char data_len;
	unsigned char capability0;
	unsigned char *data_buf;
	unsigned int data_buf_len;
	unsigned char mic[EZ_MIC_LEN];
	unsigned char expected_tag_count = EZ_ASSOC_REQ_TAG_COUNT;
	struct _ez_security *ez_security = &((ez_dev_t *)(ez_peer->ezdev))->ez_security;
	
	NdisZeroMemory(&mic[0], EZ_MIC_LEN);
	data_buf = msg;
	data_buf_len = 0;
	
	Length = 0;
	Fr = (PFRAME_802_11)msg;
	Ptr = Fr->Octet;
	Length += LENGTH_802_11;

	if (isReassoc)
	{
		Ptr += 10;
		Length += 10;
	}
	else
	{
		Ptr += 4;
		Length += 4;
	}	
	
	pEid = (PEID_STRUCT) Ptr;

	tag_count = 0;
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
						Ptr = (unsigned char *)pEid;
						data_buf_len = (unsigned int)(Ptr-(unsigned char *)msg);
						tag = pEid->Octet[EZ_TAG_OFFSET];
						data_len = pEid->Octet[EZ_TAG_LEN_OFFSET];
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO, 
							("%s() - tag = %d, data_len = %d\n", 
							__FUNCTION__, tag, data_len));
						if (tag == EZ_TAG_MIC) {
							/*
								Exclude 802.11 header to calculate mic data.
							*/
							ez_calculate_mic(ez_peer->ezdev, ez_peer->sw_key, data_buf+LENGTH_802_11, data_buf_len-LENGTH_802_11, &mic[0]);
							if (NdisEqualMemory(&mic[0], &pEid->Octet[EZ_TAG_DATA_OFFSET], EZ_MIC_LEN)) {
								tag_count++;
							}
							else {
								EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
									("%s() - MIC different......\n", __FUNCTION__));
							}
						}
						if (tag == EZ_TAG_CAPABILITY_INFO) {
							EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO,("ez_peer->capability, Before--->%x\n", ez_peer->capability));
							if (data_len == EZ_CAPABILITY_LEN) {
								NdisCopyMemory(&ez_peer->capability,
									&pEid->Octet[EZ_TAG_DATA_OFFSET], 
									EZ_CAPABILITY_LEN);
								EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO,("ez_peer->capability, received--->%x\n", ez_peer->capability));
							
								ez_peer->capability = be2cpu32(ez_peer->capability);
								tag_count++;
								//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("ez_peer->capability, After--->%x\n", ez_peer->capability));
							
							}
							else
								return EZ_STATUS_CODE_INVALID_DATA;
						}
						//! send netwrok weight in assoc response, this is stored in ez_peer info
						//! will be copied to ez_security if configuration adapt is required
						if(tag == EZ_TAG_NETWORK_WEIGHT)
						{
							NdisCopyMemory(ez_peer->device_info.network_weight,&pEid->Octet[EZ_TAG_DATA_OFFSET],NETWORK_WEIGHT_LEN);
							tag_count++;
						}
#ifndef EZ_DUAL_BAND_SUPPORT
						if (tag == EZ_TAG_AP_MAC) {
							if (data_len == MAC_ADDR_LEN) {
								NdisCopyMemory(&ez_peer->this_band_info.shared_info.ap_mac_addr, 
									&pEid->Octet[EZ_TAG_DATA_OFFSET], 
									MAC_ADDR_LEN);
								//ez_peer->capability = be2cpu32(ez_peer->capability);
								tag_count++;
							}
							else
								return EZ_STATUS_CODE_INVALID_DATA;
						}
#else
						if (tag == EZ_TAG_INTERFACE_INFO) {
							interface_info_tag_t *shared_info = (interface_info_tag_t *)&pEid->Octet[EZ_TAG_DATA_OFFSET];
							
							interface_info_t other_band_config;
							NdisCopyMemory(&ez_peer->this_band_info.shared_info,&shared_info[0], sizeof(interface_info_tag_t));
							NdisCopyMemory(&ez_peer->other_band_info.shared_info,&shared_info[1], sizeof(interface_info_tag_t));
							if (!ez_get_other_band_info(ez_peer->ezdev, &other_band_config) 
								&& !ez_security->other_band_info_backup.interface_activated)
							{
								ez_dev_t  *cli_ezdev = EZ_GET_EZBAND_CLIDEV(ez_peer->ad, ez_peer->ez_band_idx);
								
								ez_security->other_band_info_backup.shared_info.ssid_len = shared_info[1].ssid_len;
								NdisCopyMemory(ez_security->other_band_info_backup.shared_info.ssid,shared_info[1].ssid,  shared_info[1].ssid_len);
								NdisCopyMemory(&ez_security->other_band_info_backup.shared_info.channel_info, &shared_info[1].channel_info, sizeof(channel_info_t));
#ifdef DOT11R_FT_SUPPORT
								FT_SET_MDID(ez_security->other_band_info_backup.shared_info.FtMdId, shared_info[1].FtMdId);
#endif

								ez_security->other_band_info_backup.interface_activated = TRUE;
								NdisCopyMemory(&cli_ezdev->ez_security.other_band_info_backup,&ez_security->other_band_info_backup,sizeof(interface_info_t));
							}
							tag_count++;
							
						}

#endif

					}
				}
				break;
				
			default:
				break;
		}
		Length = Length + 2 + pEid->Len;  /* Eid[1] + Len[1]+ content[Len]*/
		pEid = (PEID_STRUCT)((unsigned char *)pEid + 2 + pEid->Len);
	}
#ifdef EZ_API_SUPPORT
	{
		ez_dev_t *ezdev = ez_peer->ezdev;
		if (ezdev->ez_security.ez_api_mode == CONNECTION_OFFLOAD)
		{
			expected_tag_count -= 2;
		}
	}
#endif

	if (tag_count == expected_tag_count) {
		return EZ_STATUS_CODE_SUCCESS;
	}
	else
		return EZ_STATUS_CODE_INVALID_DATA;
}

 unsigned short ez_assoc_response_sanity(
	void *msg,
	unsigned long msg_len,
	struct _ez_peer_security_info *ez_peer)
{
	unsigned char tag_count;
	FRAME_802_11 *Fr;
	unsigned char *Ptr;
	EID_STRUCT *pEid;
	unsigned long Length;
	unsigned char tag;
	unsigned char data_len;
	unsigned char capability0;
	unsigned char *data_buf;
	unsigned int data_buf_len;
	unsigned char mic[EZ_MIC_LEN];
	unsigned char expected_tag_count = EZ_ASSOC_RSP_TAG_COUNT;
	unsigned char ez_other_band_psk_len;

	data_buf_len = EZ_ASSOC_RSP_TAG_COUNT*255;
	EZ_MEM_ALLOC(NULL, &data_buf, data_buf_len);

	if (data_buf == NULL) {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
						("%s() - malloc fail.\n", __FUNCTION__));
		return EZ_STATUS_CODE_NO_RESOURCE;
	}

	NdisZeroMemory(data_buf, data_buf_len);
	NdisZeroMemory(&mic[0], EZ_MIC_LEN);
	data_buf_len = 0;

	Length = 0;
	Fr = (PFRAME_802_11)msg;
	Ptr = Fr->Octet;
	Length += LENGTH_802_11;
	
	Ptr += 6;
	Length += 6;
	
	pEid = (PEID_STRUCT) Ptr;

	tag_count = 0;
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
						if (tag == EZ_TAG_GTK) {
							if (data_len != 0) {
								EZ_MEM_ALLOC(NULL, &ez_peer->gtk, data_len+EZ_AES_KEY_ENCRYPTION_EXTEND);
								if (ez_peer->gtk) {
									unsigned char *encrypted_data;
									encrypted_data = &pEid->Octet[EZ_TAG_DATA_OFFSET];
									/* AES */
									ez_peer->ezdev->driver_ops->AES_Key_Unwrap(ez_peer->ezdev,
													encrypted_data, data_len,
												   &ez_peer->sw_key[0], LEN_PTK_KEK, 
												   ez_peer->gtk, &ez_peer->gtk_len);
									tag_count++;
									NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
									data_buf_len += (pEid->Len + 2);
								}
								else {
									ez_peer->gtk = NULL;
									if (data_buf)
										EZ_MEM_FREE( data_buf);
									return EZ_STATUS_CODE_NO_RESOURCE;
								}
							}
							else {
								if (data_buf)
									EZ_MEM_FREE( data_buf);
								return EZ_STATUS_CODE_INVALID_DATA;
							}
						}
#ifdef EZ_NETWORK_MERGE_SUPPORT	
						//! other band and weight parameters are not send in assoc request because 
						//! weight comparision after connection is performed by CLI, 		
						//! send oter band PMK in ecrypted form also in the assoc response			
						if (tag == EZ_TAG_OTHER_BAND_PMK)
						{
							unsigned int other_band_pmk_len;
							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							ez_peer->ezdev->driver_ops->AES_Key_Unwrap(ez_peer->ezdev,
									&pEid->Octet[EZ_TAG_DATA_OFFSET], data_len,
								   &ez_peer->sw_key[0], LEN_PTK_KEK, 
						    		ez_peer->other_band_info.pmk, &other_band_pmk_len);
							tag_count++;
						}

						if (tag == EZ_TAG_OTHER_BAND_PSK_LEN)
						{
							NdisCopyMemory(data_buf + data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							ez_other_band_psk_len = pEid->Octet[EZ_TAG_DATA_OFFSET];
							tag_count++;
						}

						if (tag == EZ_TAG_OTHER_BAND_PSK)
						{
							unsigned char *psk;
							unsigned int other_band_psk_len;
							NdisCopyMemory(data_buf + data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							EZ_MEM_ALLOC(NULL, &psk, data_len + EZ_AES_KEY_ENCRYPTION_EXTEND);
							if (psk) {
								ez_peer->ezdev->driver_ops->AES_Key_Unwrap(ez_peer->ezdev,
									&pEid->Octet[EZ_TAG_DATA_OFFSET], data_len,
									&ez_peer->sw_key[0], LEN_PTK_KEK, 
							    	psk, &other_band_psk_len);
								NdisZeroMemory(&ez_peer->other_band_info.psk[0], EZ_LEN_PSK);
								NdisCopyMemory(&ez_peer->other_band_info.psk[0], psk, ez_other_band_psk_len);
								tag_count++;
								EZ_MEM_FREE(psk);
							}
						}

						if (tag == EZ_TAG_GROUPID_SEED)
						{
							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							if(data_len != 0)
							{
								if(ez_peer->gen_group_id)
									EZ_MEM_FREE(ez_peer->gen_group_id);
								
								EZ_MEM_ALLOC(NULL, &ez_peer->gen_group_id, data_len+EZ_AES_KEY_ENCRYPTION_EXTEND);
								ez_peer->ezdev->driver_ops->AES_Key_Unwrap(ez_peer->ezdev,
										&pEid->Octet[EZ_TAG_DATA_OFFSET], data_len,
									   &ez_peer->sw_key[0], LEN_PTK_KEK, 
							    		ez_peer->gen_group_id, &ez_peer->gen_group_id_len);
							}
							tag_count++;
						}

						if (tag == EZ_TAG_NON_EZ_CONFIG)
						{
							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							NdisCopyMemory(&ez_peer->non_ez_band_info[0], &pEid->Octet[EZ_TAG_DATA_OFFSET],data_len);
							tag_count++;
						}

						
						if (tag == EZ_TAG_NON_EZ_PSK)
						{
							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							NdisCopyMemory(&ez_peer->non_ez_psk_info[0], &pEid->Octet[EZ_TAG_DATA_OFFSET],data_len);
							tag_count++;
						}
//! Levarage from MP1.0 CL#170037
						if (tag == EZ_TAG_NON_MAN_CONFIG)
						{
							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							NdisCopyMemory(&ez_peer->non_man_info, &pEid->Octet[EZ_TAG_DATA_OFFSET],data_len);
							tag_count++;
						}
						
#ifndef EZ_DUAL_BAND_SUPPORT		
						//! send netwrok weight in assoc response, this is stored in ez_peer info
						//! will be copied to ez_security if configuration adapt is required
						if(tag == EZ_TAG_NETWORK_WEIGHT)
						{

							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							NdisCopyMemory(ez_peer->device_info.network_weight,&pEid->Octet[EZ_TAG_DATA_OFFSET],NETWORK_WEIGHT_LEN);
							tag_count++;
						}
#else
						//! send netwrok weight in assoc response, this is stored in ez_peer info
						//! will be copied to ez_security if configuration adapt is required
						if(tag == EZ_TAG_DEVICE_INFO)
						{

							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							NdisCopyMemory(&ez_peer->device_info,&pEid->Octet[EZ_TAG_DATA_OFFSET], sizeof(device_info_t));
							tag_count++;
						}
#endif
#if 0
						//! send other band SSID in assoc response
						if(tag == EZ_TAG_OTHER_BAND_SSID)
						{

							NdisCopyMemory(data_buf+data_buf_len, pEid, (pEid->Len + 2));
							data_buf_len += (pEid->Len + 2);
							ez_peer->other_band_info.shared_info.ssid_len = pEid->Octet[EZ_TAG_LEN_OFFSET];
							NdisZeroMemory(ez_peer->other_band_info.shared_info.ssid,32);
							NdisCopyMemory(ez_peer->other_band_info.shared_info.ssid,&pEid->Octet[EZ_TAG_DATA_OFFSET],ez_peer->other_band_info.shared_info.ssid_len);
							tag_count++;
						}

#else
						if (tag == EZ_TAG_INTERFACE_INFO) {
							interface_info_tag_t *shared_info = (interface_info_tag_t *)&pEid->Octet[EZ_TAG_DATA_OFFSET];
							NdisCopyMemory(&ez_peer->this_band_info.shared_info,&shared_info[0], sizeof(interface_info_tag_t));
							NdisCopyMemory(&ez_peer->other_band_info.shared_info,&shared_info[1], sizeof(interface_info_tag_t));
							tag_count++;
							
						}

#endif

#endif
						if (tag == EZ_TAG_MIC) {
							ez_calculate_mic(ez_peer->ezdev, ez_peer->sw_key, data_buf, data_buf_len, &mic[0]);
							if (NdisEqualMemory(&mic[0], &pEid->Octet[EZ_TAG_DATA_OFFSET], EZ_MIC_LEN)) {
								tag_count++;
							}
							else {
								EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
									("%s() - MIC different......\n", __FUNCTION__));
							}
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

	if (data_buf)
		EZ_MEM_FREE( data_buf);

#ifdef EZ_API_SUPPORT
	{
		ez_dev_t *ezdev = ez_peer->ezdev;
		if (ezdev->ez_security.ez_api_mode == CONNECTION_OFFLOAD)
		{
			expected_tag_count -= 3;
		}
	}
#endif	
	if (ez_is_triband_hook())
		expected_tag_count += 2;
	else if(((EZ_ADAPTER *)(ez_peer->ad))->is_man_nonman)//! Levarage from MP1.0 CL 170037
		expected_tag_count += 1;
	if (tag_count == expected_tag_count) {
		return EZ_STATUS_CODE_SUCCESS;
	}
	else
		return EZ_STATUS_CODE_INVALID_DATA;
}


struct _ez_peer_security_info *ez_peer_table_insert(
	ez_dev_t *ezdev,
	unsigned char *addr
	)
{
	int i;
	int irq_flags;
	struct _ez_peer_security_info *ez_peer;
	
	ez_peer = NULL;
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
						("------> %s()\n", __FUNCTION__));
	ez_hex_dump("addr", addr, 6);
	ez_peer = ez_peer_table_search_by_addr_hook(ezdev, addr);
	if (ez_peer)
		return ez_peer;
	EZ_IRQ_LOCK(ezdev->ez_peer_table_lock, irq_flags);
	for (i = 0; i < EZ_MAX_STA_NUM; i++) {		
		if (EZ_GET_EZBAND_BAND(ezdev->ez_ad, ezdev->ez_band_idx)->ez_peer_table[i].valid == FALSE) {
			ez_peer = &EZ_GET_EZBAND_BAND(ezdev->ez_ad, ezdev->ez_band_idx)->ez_peer_table[i];
			NdisZeroMemory(ez_peer, sizeof(struct _ez_peer_security_info));

			ez_peer->valid = TRUE;
			
			ez_peer->ez_peer_table_index = i;
			ez_peer->ezdev = ezdev;
			ez_peer->ad = ezdev->ez_ad;
			ez_peer->ez_band_idx = ezdev->ez_band_idx;
			NdisGetSystemUpTime(&ez_peer->creation_time);
			NdisCopyMemory(&ez_peer->mac_addr[0], addr, MAC_ADDR_LEN);
			break;
		}
	}
	EZ_IRQ_UNLOCK(ezdev->ez_peer_table_lock, irq_flags);
	return ez_peer;
}

void ez_peer_table_delete(
	ez_dev_t *ezdev,
	unsigned char *addr)
{
	struct _ez_peer_security_info *ez_peer;
	int irq_flags;
	int irq_flags1;
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
						("------> %s()\n", __FUNCTION__));
	ez_hex_dump("addr", addr, 6);

	ez_peer = ez_peer_table_search_by_addr_hook(ezdev, addr);

		

	EZ_IRQ_LOCK(ezdev->ez_peer_table_lock, irq_flags);		

	if (ez_peer) {

		if(!IS_SINGLE_CHIP_DBDC((EZ_ADAPTER *)ezdev->ez_ad)) 
			EZ_IRQ_LOCK(&((EZ_ADAPTER *)(ezdev->ez_ad))->ez_set_peer_lock, irq_flags1);
		if (ez_peer->gtk) {
			EZ_MEM_FREE( ez_peer->gtk);
			ez_peer->gtk = NULL;
			ez_peer->gtk_len = 0;
		}
		if (ez_peer->group_id) {
			EZ_MEM_FREE( ez_peer->group_id);
			ez_peer->group_id = NULL;
			ez_peer->group_id_len= 0;
		}
		if (ez_peer->gen_group_id) {
			EZ_MEM_FREE( ez_peer->gen_group_id);
			ez_peer->gen_group_id = NULL;
			ez_peer->gen_group_id_len= 0;
		}
		NdisZeroMemory(ez_peer, sizeof(struct _ez_peer_security_info));
			if(!IS_SINGLE_CHIP_DBDC((EZ_ADAPTER *)ezdev->ez_ad))
				EZ_IRQ_UNLOCK(&((EZ_ADAPTER *)(ezdev->ez_ad))->ez_set_peer_lock, irq_flags1);
	}else{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
							("------> %s()ez_peer_table Entry not found\n", __FUNCTION__));
	}
	EZ_IRQ_UNLOCK(ezdev->ez_peer_table_lock, irq_flags);
}

void ez_show_peer_table_info(
	ez_dev_t *ezdev)
{
	int i;
	int j;
	struct _ez_peer_security_info *ez_peer;

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Show peer information - \n"));
	for (i = 0; i < EZ_MAX_STA_NUM; i++) {
			ez_peer = &EZ_GET_EZBAND_BAND(ezdev->ez_ad,ezdev->ez_band_idx)->ez_peer_table[i];

			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\t[%d] valid = %d, %02x:%02x:%02x:%02x:%02x:%02x\n", 
					i,
					ez_peer->valid,
					ez_peer->mac_addr[0],
					ez_peer->mac_addr[1],
					ez_peer->mac_addr[2],
					ez_peer->mac_addr[3],
					ez_peer->mac_addr[4],
					ez_peer->mac_addr[5]));
			if (ez_peer->valid) {
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\tCapability   = 0x%04x\n", ez_peer->capability));
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\tGroup ID     = "));
				for (j=0; j<ez_peer->group_id_len; j++) {
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%02x", ez_peer->group_id[j]));
				}
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
				ez_hex_dump("\tNodeNumber    = ", (PUCHAR)&ez_peer->device_info.ez_node_number, sizeof(EZ_NODE_NUMBER));
				ez_hex_dump("\tWeight    = ", (PUCHAR)&ez_peer->device_info.network_weight, NETWORK_WEIGHT_LEN);

				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\tPKE         = "));
			
				for (j=0; j<EZ_RAW_KEY_LEN; j++) {
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%02x", ez_peer->peer_pke[j]));
				}
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\tNonce         = "));
				for (j=0; j<EZ_NONCE_LEN; j++) {
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%02x", ez_peer->peer_nonce[j]));
				}
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\tSHA256 DH KEY= "));
				for (j=0; j<EZ_PMK_LEN; j++) {
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%02x", ez_peer->dh_key[j]));
				}
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\tPMK          = "));
				for (j=0; j<EZ_PMK_LEN; j++) {
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%02x", ez_peer->this_band_info.pmk[j]));
				}
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("PSK is: %s\n",
					ez_peer->this_band_info.psk));
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\tSW Key       = "));
				for (j=0; j<EZ_PTK_LEN; j++) {
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%02x", ez_peer->sw_key[j]));
				}
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
			}
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
	}
}

void ez_insert_tlv(
	unsigned char tag,
	unsigned char *data,
	unsigned char data_len,
	unsigned char *buffer,
	unsigned long *msg_len)
{
	struct _mediatek_ie mtk_ie;
	unsigned char mtk_tag;
	unsigned char mtk_data_len;
	
	NdisZeroMemory(&mtk_ie, sizeof(struct _mediatek_ie));
	mtk_ie.ie_hdr.eid = IE_VENDOR_SPECIFIC;
	mtk_ie.ie_hdr.len = MTK_OUI_LEN + MTK_VENDOR_CAPABILITY_SIZE;
	mtk_ie.ie_hdr.len = MTK_OUI_LEN + MTK_VENDOR_CAPABILITY_SIZE + EZ_TLV_TAG_SIZE + EZ_TLV_LEN_SIZE + data_len;
	NdisCopyMemory(&mtk_ie.oui, &mtk_oui[0], MTK_OUI_LEN);
	mtk_ie.cap0 |= MEDIATEK_EASY_SETUP;
	mtk_tag = tag;
	mtk_data_len = data_len;
	if (data && data_len) {
		EzMakeOutgoingFrame(buffer, msg_len, 
			sizeof(struct _mediatek_ie), &mtk_ie,
			1, &mtk_tag,
			1, &mtk_data_len,
			data_len, data,
			END_OF_ARGS);
	}
	else {
		EzMakeOutgoingFrame(buffer, msg_len, 
				sizeof(struct _mediatek_ie), &mtk_ie,
				1, &mtk_tag,
				1, &mtk_data_len,
				END_OF_ARGS);
	}
}
BOOLEAN ez_apcli_is_link_duplicate(ez_dev_t *ezdev,unsigned char * peer_addr)
{
		//EZ_ADAPTER *ez_ad = ez_ad;
		struct _ez_peer_security_info * ez_other_cli_peer_ap;
		interface_info_t other_band_info;
		ez_dev_t  *other_band_ezdev = ez_get_otherband_cli_ezdev(ezdev);
#if 0		
#ifdef DUAL_CHIP
		PRTMP_ADAPTER adOthBand = (PRTMP_ADAPTER)ez_ad->ez_adOthBand;
		if (!IS_SINGLE_CHIP_DBDC(ezdev->ez_ad)){
			if(adOthBand == NULL) {
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			("------> %s(): Error: adOthBand address is null\n", __FUNCTION__));
				ASSERT(FALSE);
				return FALSE;
			}
		}
#endif
#endif
		if (other_band_ezdev == NULL)
		{		
			return FALSE;
		}
		if (ez_get_other_band_info(ezdev, &other_band_info) == FALSE)
			return FALSE;

		ez_other_cli_peer_ap = ez_peer_table_search_by_addr_hook(other_band_ezdev, other_band_info.cli_peer_ap_mac);

		if (ez_other_cli_peer_ap 
			&& NdisEqualMemory(peer_addr,ez_other_cli_peer_ap->other_band_info.shared_info.ap_mac_addr,MAC_ADDR_LEN))
		{
			//! since internal_force_connect_bssid is set, we are attempting  duplicate link, AP will simply store it in its ez_peer
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("TRUE\n"));
			return TRUE;
		}
	
		return FALSE;
}


void ez_prepare_non_ez_tag(NON_EZ_BAND_INFO_TAG * non_ez_tag, NON_EZ_BAND_PSK_INFO_TAG * non_ez_psk_tag,struct _ez_peer_security_info *ez_peer)
{
	int band_count = 0;
	UINT entrypted_pmk_len;
	EZ_ADAPTER *ez_ad = ez_peer->ad;
	
	for (band_count = 0; band_count < MAX_NON_EZ_BANDS; band_count++)
	{
		NdisCopyMemory(non_ez_tag[band_count].ssid , ez_ad->non_ez_band_info[band_count].ssid,ez_ad->non_ez_band_info[band_count].ssid_len);
		non_ez_tag[band_count].ssid_len = ez_ad->non_ez_band_info[band_count].ssid_len;
		
		non_ez_tag[band_count].triband_sec.AKMMap = ez_ad->non_ez_band_info[band_count].triband_sec.AKMMap;
		non_ez_tag[band_count].triband_sec.PairwiseCipher = ez_ad->non_ez_band_info[band_count].triband_sec.PairwiseCipher;
		non_ez_tag[band_count].triband_sec.GroupCipher = ez_ad->non_ez_band_info[band_count].triband_sec.GroupCipher;
#ifdef DOT11R_FT_SUPPORT
		FT_SET_MDID(&non_ez_tag[band_count].FtMdId, &ez_ad->non_ez_band_info[band_count].FtMdId);
#endif

	NdisZeroMemory(non_ez_tag[band_count].encrypted_pmk, EZ_PMK_LEN + EZ_AES_KEY_ENCRYPTION_EXTEND);
	NdisZeroMemory(non_ez_psk_tag[band_count].encrypted_psk, LEN_PSK + EZ_AES_KEY_ENCRYPTION_EXTEND);
	
	/* encrypt */
	ez_peer->ezdev->driver_ops->AES_Key_Wrap(ez_peer->ezdev, ez_ad->non_ez_band_info[band_count].pmk, EZ_PMK_LEN, ez_peer->sw_key, LEN_PTK_KEK, 
				 non_ez_tag[band_count].encrypted_pmk, &entrypted_pmk_len);
	ez_peer->ezdev->driver_ops->AES_Key_Wrap(ez_peer->ezdev, ez_ad->non_ez_band_info[band_count].psk, LEN_PSK, ez_peer->sw_key, LEN_PTK_KEK, 
				 non_ez_psk_tag[band_count].encrypted_psk, &entrypted_pmk_len);
	

	}
}
//! Levarage from MP1.0 CL #170037
void ez_prepare_non_man_tag(NON_MAN_INFO_TAG * non_man_tag, struct _ez_peer_security_info *ez_peer)
{
	UINT entrypted_pmk_len;
	EZ_ADAPTER *ez_ad = ez_peer->ad;

	NdisZeroMemory(non_man_tag, sizeof(NON_MAN_INFO_TAG));
	
	non_man_tag->ssid_len = ez_ad->non_man_info.ssid_len;
	NdisCopyMemory(non_man_tag->ssid , ez_ad->non_man_info.ssid, ez_ad->non_man_info.ssid_len);
	NdisCopyMemory(non_man_tag->authmode, ez_ad->non_man_info.authmode, strlen(ez_ad->non_man_info.authmode));
	NdisCopyMemory(non_man_tag->encryptype, ez_ad->non_man_info.encryptype, strlen(ez_ad->non_man_info.encryptype));

#ifdef DOT11R_FT_SUPPORT
	FT_SET_MDID(&non_man_tag->FtMdId, &ez_ad->non_man_info.FtMdId);
#endif

	NdisZeroMemory(non_man_tag->encrypted_psk, LEN_PSK + EZ_AES_KEY_ENCRYPTION_EXTEND);
	
	/* encrypt */
	ez_peer->ezdev->driver_ops->AES_Key_Wrap(ez_peer->ezdev, ez_ad->non_man_info.psk, LEN_PSK, ez_peer->sw_key, LEN_PTK_KEK, 
				 non_man_tag->encrypted_psk, &entrypted_pmk_len);
	

}


unsigned char ez_install_ptk(
	struct _ez_peer_security_info *ez_peer,
	unsigned char authenticator)
{
	ez_dev_t *ezdev;
	struct _ez_security *ez_sec_info;
	unsigned char ptk[EZ_PTK_LEN];
	
	ezdev = ez_peer->ezdev;
	ez_sec_info = &ezdev->ez_security;

	if (ez_peer) {
		if (authenticator) {
#ifdef RELEASE_EXCLUDE
			ez_hex_dump("A_Nonce", ez_sec_info->self_nonce, EZ_NONCE_LEN);
			ez_hex_dump("A_Addr", ezdev->if_addr, 6);
			ez_hex_dump("S_Nonce", ez_peer->peer_nonce, EZ_NONCE_LEN);
			ez_hex_dump("S_Addr", entry->Addr, 6);
#endif /* RELEASE_EXCLUDE */
			ez_peer->ezdev->driver_ops->WpaDerivePTK(ez_peer->ezdev,
				&ez_peer->this_band_info.pmk[0], 
				ez_sec_info->self_nonce, 
				ezdev->if_addr, 
				ez_peer->peer_nonce, 
				ez_peer->mac_addr, 
				&ptk[0], 
				LEN_PTK);		
		}
		else {
#ifdef RELEASE_EXCLUDE
			ez_hex_dump("A_Nonce", ez_peer->peer_nonce, EZ_NONCE_LEN);
			ez_hex_dump("A_Addr", entry->Addr, 6);
			ez_hex_dump("S_Nonce", ez_sec_info->self_nonce, EZ_NONCE_LEN);
			ez_hex_dump("S_Addr", ezdev->if_addr, 6);
#endif /* RELEASE_EXCLUDE */
			ez_peer->ezdev->driver_ops->WpaDerivePTK(ez_peer->ezdev,
				&ez_peer->this_band_info.pmk[0], 
				ez_peer->peer_nonce,
				ez_peer->mac_addr, 
				ez_sec_info->self_nonce, 
				ezdev->if_addr,
				&ptk[0], 
				LEN_PTK);
		}
		//ez_hex_dump("PTK", &ptk[0], LEN_PTK);
		ez_peer->ezdev->driver_ops->ez_install_pairwise_key(ezdev, ez_peer->mac_addr, &ez_peer->this_band_info.pmk[0], &ptk[0], authenticator);
		return TRUE;
	}
	else {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("%s - Install PTK failed?? This case shalln't happen....\n", 
			__FUNCTION__));
		return FALSE;
	}
}

unsigned char ez_apcli_install_gtk(
	struct _ez_peer_security_info *ez_peer)
{

	if (ez_peer) {
		ez_peer->ezdev->driver_ops->ez_apcli_install_group_key(ez_peer->ezdev, &ez_peer->mac_addr[0], ez_peer->gtk, ez_peer->gtk_len);
		return TRUE;
	}
	else {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, 
			("%s - Install PTK failed?? This case shalln't happen....\n", 
			__FUNCTION__));
		return FALSE;
	}
}

BOOLEAN ez_is_loop_formed(struct _ez_peer_security_info *ez_peer)
{
	int i;
	EZ_ADAPTER *ez_ad = ez_peer->ad;
	for (i=0; i< EZDEV_NUM_MAX;i++)
	{
		ez_dev_t *ezdev = ez_ad->ezdev_list[i];
		if(ezdev && ez_is_ap_apcli(ez_ad->ezdev_list[i]))
		{
			if (MAC_ADDR_EQUAL(ezdev->if_addr,&ez_peer->device_info.network_weight[1])){
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("Loop Formed !!!\n"));
				return TRUE;
			}
		}
	}
	return FALSE;
}
void ez_show_interface_info(ez_dev_t *ezdev)
{
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Interface Activated = %d\n\n",
								ezdev->ez_security.this_band_info.interface_activated));
	//if(ezdev->ez_security.this_band_info.interface_activated)
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("SSID: %s\n",
							ezdev->ez_security.this_band_info.shared_info.ssid));
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("SSID Len = %d\n",
							ezdev->ez_security.this_band_info.shared_info.ssid_len));
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Channel: %d\n", 
							ezdev->ez_security.this_band_info.shared_info.channel_info.channel));
#ifdef EZ_PUSH_BW_SUPPORT
		//if( ((PRTMP_ADAPTER)(ezdev->ez_ad))->push_bw_config )
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\tHT-BW: %d, CFG:%d OPER:%d\n", 
								ezdev->ez_security.this_band_info.shared_info.channel_info.ht_bw,
								ezdev->driver_ops->wlan_config_get_ht_bw(ezdev),
								ezdev->driver_ops->wlan_operate_get_ht_bw(ezdev)));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\tVHT-BW: %d CFG:%d OPER:%d\n", 
								ezdev->ez_security.this_band_info.shared_info.channel_info.vht_bw,
								ezdev->driver_ops->wlan_config_get_vht_bw(ezdev),
								ezdev->driver_ops->wlan_operate_get_vht_bw(ezdev)));
		}
#else
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\tHT-BW: CFG:%d OPER:%d\n", 
								ezdev->driver_ops->wlan_config_get_ht_bw(ezdev),
								ezdev->driver_ops->wlan_operate_get_ht_bw(ezdev)));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\tVHT-BW: CFG:%d OPER:%d\n", 
								ezdev->driver_ops->wlan_config_get_vht_bw(ezdev),
								ezdev->driver_ops->wlan_operate_get_vht_bw(ezdev)));
}
#endif
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\tEXTCHA: %d CFG:%d OPER:%d\n", 
							ezdev->ez_security.this_band_info.shared_info.channel_info.extcha,
							ezdev->driver_ops->wlan_config_get_ext_cha(ezdev),
							ezdev->driver_ops->wlan_operate_get_ext_cha(ezdev)));
		ez_hex_dump("ap_mac_addr", ezdev->ez_security.this_band_info.shared_info.ap_mac_addr, MAC_ADDR_LEN);
		ez_hex_dump("cli_mac_addr", ezdev->ez_security.this_band_info.shared_info.cli_mac_addr, MAC_ADDR_LEN);
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Cli Link Duplicate = %d\n",
							ezdev->ez_security.this_band_info.shared_info.link_duplicate));
	
		ez_hex_dump("cli_peer_ap_addr", ezdev->ez_security.this_band_info.cli_peer_ap_mac, MAC_ADDR_LEN);
		ez_hex_dump("PMK", ezdev->ez_security.this_band_info.pmk, EZ_PMK_LEN);
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("PSK: %s\n",
			ezdev->ez_security.this_band_info.psk));

#ifdef DOT11R_FT_SUPPORT
		if((ezdev->ezdev_type == EZDEV_TYPE_AP) && (ezdev->FtCfg.FtCapFlag.Dot11rFtEnable)){
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("%s::FtMdid =%c%c\n", __FUNCTION__,
									ezdev->ez_security.this_band_info.shared_info.FtMdId[0],
									ezdev->ez_security.this_band_info.shared_info.FtMdId[1]));
		}
#endif

	}
	
}
void ez_show_device_info(device_info_t ez_device_info)
{
	ez_hex_dump("Network Wt", (PUCHAR)ez_device_info.network_weight, NETWORK_WEIGHT_LEN);
	ez_hex_dump("WDL MAC", ez_device_info.weight_defining_link.peer_mac, MAC_ADDR_LEN);
	ez_hex_dump("NodeNumber", (void *)&ez_device_info.ez_node_number,sizeof(EZ_NODE_NUMBER));
}

/*
	If addr1 > addr2, return TRUE
	else return FALSE.
*/
 unsigned char ez_mac_addr_compare(
	unsigned char *addr1,
	unsigned char *addr2)
{
	int i;
	unsigned char higher_mac;

	higher_mac = FALSE;
	for (i = 0; i < 6; i++)
	{
		if (addr1[i] > addr2[i])
		{
			higher_mac = TRUE;
			break;
		}
		else if (addr1[i] < addr2[i])
			break;
	}
	return higher_mac;
}

EZ_ADAPTER *ez_get_adapter_hook(void);

void ez_allocate_node_number(
	EZ_NODE_NUMBER *node_number,
	ez_dev_t *ezdev)
{
	ez_dev_t *ezdev_2p4 = ezdev;

	EZ_ADAPTER *ez_ad = ez_get_adapter_hook();

	node_number->path[0] = 0;
	NdisZeroMemory(node_number,sizeof(EZ_NODE_NUMBER));
	node_number->path_len = MAC_ADDR_LEN;

	if (ez_ad->band_count == 1) {
		NdisCopyMemory(node_number->root_mac, ezdev->if_addr,MAC_ADDR_LEN);
	} else {
		ez_dev_t *other_band_ezdev = ez_get_otherband_ezdev(ezdev);

		if (ez_get_band(ezdev)){
			if (other_band_ezdev != NULL){
				ezdev_2p4 = ez_get_otherband_ezdev(ezdev);
			}
		}

		/*Node number of the device will be the mac address of the 2.4GHz AP*/
			NdisCopyMemory(node_number->root_mac, EZ_GET_EZBAND_APDEV(ezdev_2p4->ez_ad,ezdev_2p4->ez_band_idx)->if_addr,MAC_ADDR_LEN);
	}
	ez_hex_dump("OwnNodeNumber", (PUCHAR)node_number, sizeof(EZ_NODE_NUMBER));
}
////////////////////////////////////////////////////////////////////////////Done till here
void ez_apcli_allocate_self_node_number(
	EZ_NODE_NUMBER *node_number, 
	ez_dev_t *ezdev, char *mac_addr)
{

// TODO: Raghav: check if node number on other band is already given. if true then don't allocate another node number
	NdisZeroMemory(node_number,sizeof(EZ_NODE_NUMBER));
	NdisCopyMemory(node_number->root_mac,mac_addr,MAC_ADDR_LEN);
	if (ez_get_band(ezdev) == BAND0)
		node_number->path[0] = ezdev->driver_ops->get_cli_aid(ezdev, ezdev->bssid);
	else if (ez_get_band(ezdev) == BAND1)
		node_number->path[0] = (1<< 7) | (ezdev->driver_ops->get_cli_aid(ezdev, mac_addr));

	node_number->path_len = MAC_ADDR_LEN + 1;
	
	ez_hex_dump("OwnSelfNodeNumber", (PUCHAR)node_number, sizeof(EZ_NODE_NUMBER));
}

 void ez_restore_node_number(EZ_NODE_NUMBER *ez_node_number)
{
	// delete the last entry in the node number to get the parent's node number
	ez_node_number->path[ez_node_number->path_len -6 -1] = 0;
	ez_node_number->path_len-=1;
	
}

void ez_allocate_node_number_sta(
	struct _ez_peer_security_info *ez_peer,
	BOOLEAN is_forced)
{
	EZ_ADAPTER *ez_ad = ez_peer->ad;
	EZ_NODE_NUMBER *own_node_number = &ez_ad->device_info.ez_node_number;
	ez_dev_t *ezdev = ez_peer->ezdev;
	int irq_flags;
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,(" %s -->\n", __FUNCTION__));

	if (ez_peer == NULL) {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%s: ez_peer is NULL", __FUNCTION__)); 
		return;
	}

	
	ez_hex_dump("PeerMac",ez_peer->mac_addr,6);

 #if 1
#if 0
 	if (is_forced == FALSE && ez_peer->this_band_info.shared_info.link_duplicate)
#else
	if (is_forced == FALSE && ez_is_link_duplicate(ez_peer))
#endif
	{
		struct _ez_peer_security_info *first_ez_peer = ez_get_other_band_ez_peer(ezdev, ez_peer);
		if (first_ez_peer) {
			NdisCopyMemory(&ez_peer->device_info.ez_node_number,&first_ez_peer->device_info.ez_node_number,sizeof(EZ_NODE_NUMBER));
			ez_hex_dump("PeerNodeNum:", (PUCHAR)&ez_peer->device_info.ez_node_number, sizeof(EZ_NODE_NUMBER));
			return;
		}
		else {
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("ez_allocate_node_number_sta: BUG:is duplicate link but first peer missing. "));
		}
	}
#endif
	EZ_IRQ_LOCK(ezdev->ez_peer_table_lock, irq_flags);

	NdisZeroMemory(&ez_peer->device_info.ez_node_number,sizeof(EZ_NODE_NUMBER));
	NdisCopyMemory(ez_peer->device_info.ez_node_number.root_mac,own_node_number->root_mac,own_node_number->path_len);
	if (ezdev->ezdev_type == EZDEV_TYPE_APCLI)  // if allocating for the AP : for AP node number , 6th bit will be set, for 5Ghz device, 7th bit will be set
	{
		if (ez_get_band(ezdev) == BAND0)
		{
			ez_peer->device_info.ez_node_number.path[own_node_number->path_len -MAC_ADDR_LEN] = (1<<6) | (ez_peer->ez_peer_table_index+1);
		}
		else if (ez_get_band(ezdev) == BAND1)
		{
			ez_peer->device_info.ez_node_number.path[own_node_number->path_len -MAC_ADDR_LEN] = (3<<6) | (ez_peer->ez_peer_table_index+1);
		}
	}
	else {  // allocating node number for the connected clients (APCLI entries)
		if (ez_get_band(ezdev) == BAND0)
		{
			ez_peer->device_info.ez_node_number.path[own_node_number->path_len -MAC_ADDR_LEN] = (ez_peer->ez_peer_table_index+1);
		}
		else if (ez_get_band(ezdev) == BAND1)
		{
			ez_peer->device_info.ez_node_number.path[own_node_number->path_len -MAC_ADDR_LEN] = (1<<7) | (ez_peer->ez_peer_table_index+1);
		}
	}
	ez_peer->device_info.ez_node_number.path_len = own_node_number->path_len + 1;

	EZ_IRQ_UNLOCK(ezdev->ez_peer_table_lock, irq_flags);

	ez_hex_dump("PeerNodeNum:", (PUCHAR)&ez_peer->device_info.ez_node_number, sizeof(EZ_NODE_NUMBER));
}

/*check whether peer node is child node of the own node*/
BOOLEAN ez_is_child_node(
	EZ_NODE_NUMBER own_node_number, 
	EZ_NODE_NUMBER peer_node_number)
{
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s -->\n", __FUNCTION__));

	ez_hex_dump("PeerNodeNum", (PUCHAR)&peer_node_number, sizeof(EZ_NODE_NUMBER));
	if ((MAC_ADDR_EQUAL(own_node_number.root_mac,peer_node_number.root_mac))
		&& NdisEqualMemory(own_node_number.path,peer_node_number.path ,own_node_number.path_len - MAC_ADDR_LEN))
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("TRUE\n"));
		return TRUE;
	}
	else
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("FALSE\n"));
		return FALSE;
	}
}
BOOLEAN ez_is_same_open_group_id(ez_dev_t *ezdev, char *open_group_id, char open_group_id_len)
{
	/*connection to third party AP*/
	if (ezdev->ez_security.open_group_id_len == open_group_id_len
		&& NdisEqualMemory(ezdev->ez_security.open_group_id,open_group_id,open_group_id_len)) {
	        ez_hex_dump("OwnOpenGroupID",ezdev->ez_security.open_group_id, ezdev->ez_security.open_group_id_len);
	        ez_hex_dump("PeerOpenGroupID",open_group_id, open_group_id_len);
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,(" TRUE\n"));
		return TRUE;
	}
	else
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("ez_is_same_open_group_id: FALSE\n"));
		return FALSE;
}

BOOLEAN ez_is_other_band_connection_to_same_bss(ez_dev_t *ezdev, beacon_info_tag_t *beacon_info)
{
	//EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	ez_dev_t *other_band_ezdev = ez_get_otherband_cli_ezdev(ezdev);

	if (other_band_ezdev)
	{
	} else {
		return FALSE;
	}

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,(" %s ez_band_idx=%d -->\n", __FUNCTION__, ezdev->ez_band_idx));
	if(MAC_ADDR_EQUAL(other_band_ezdev->bssid, beacon_info->other_ap_mac) )
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("TRUE\n"));
		return TRUE;
	}
	else
	{	
		ez_hex_dump("OtherbandBssid",other_band_ezdev->bssid,6);
		ez_hex_dump("Bss",beacon_info->other_ap_mac,6);
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("FALSE"));
		return FALSE;
	}
}


void ez_apcli_force_bssid(
	ez_dev_t *ezdev,
	unsigned char *bssid)
{
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,(" %s -->\n", __FUNCTION__));
	COPY_MAC_ADDR(ezdev->ez_security.ez_apcli_force_bssid,bssid);
}

#if 0
void ez_apcli_force_channel(
	ez_dev_t *ezdev,
	unsigned char channel)
{
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,(" %s -->\n", __FUNCTION__));

//	if (IS_SINGLE_CHIP_DBDC(ez_ad))
	ez_ad->ApCfg.ApCliTab[if_index].ezdev->ez_security.ez_apcli_force_channel = channel;
}
#endif

BOOLEAN ez_is_link_duplicate(struct _ez_peer_security_info *ez_peer)
{
	ez_dev_t *ezdev = ez_peer->ezdev;

	if(ezdev->ezdev_type == EZDEV_TYPE_APCLI)
	{
		return ezdev->ez_security.this_band_info.shared_info.link_duplicate;
	} else {
		return ez_peer->this_band_info.shared_info.link_duplicate;
	}
	return FALSE;
}

void ez_wait_for_connection_allow(
	unsigned long time,
	EZ_ADAPTER *ez_ad)
{
	unsigned long now;
	ez_dev_t *connection_wait_timer_ezdev = ez_ad->ez_connect_wait_ezdev;
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,(" %s --> %d\n", __FUNCTION__, (int)time));
	NdisGetSystemUpTime(&now);

	if (connection_wait_timer_ezdev->driver_ops->ez_is_timer_running(ez_ad->ez_connect_wait_ezdev
				, ez_ad->ez_connect_wait_timer))
	{
		if (!RTMP_TIME_AFTER(ez_ad->ez_connect_wait_timer_value + ez_ad->ez_connect_wait_timer_timestamp
			,now + time))
		{
			
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,(" %s --> Cancel previous timer\n", __FUNCTION__));
			connection_wait_timer_ezdev->driver_ops->ez_cancel_timer(connection_wait_timer_ezdev,
				ez_ad->ez_connect_wait_timer);
		} else {
		
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,(" %s --> rely on previous timer\n", __FUNCTION__));
			return;
		}

	}
	connection_wait_timer_ezdev->driver_ops->ez_set_timer
		(connection_wait_timer_ezdev, ez_ad->ez_connect_wait_timer, time);
	ez_ad->ez_connect_wait_timer_value = time;
	ez_ad->ez_connect_wait_timer_timestamp = now;
	ez_ad->ez_connect_wait_ezdev->ez_security.weight_update_going_on = TRUE;
	
}

#ifdef EZ_ROAM_SUPPORT
void ez_apcli_check_roaming_status(EZ_ADAPTER *ez_ad)
{
	ULONG now;
	int i;
	NdisGetSystemUpTime(&now);
	
	//if (IS_SINGLE_CHIP_DBDC(ez_ad))
	for (i=0; i < MAX_EZ_BANDS; i++) {
		ez_dev_t *ezdev = &ez_ad->ez_band_info[i].cli_ezdev;
		ez_dev_t *ap_ezdev = &ez_ad->ez_band_info[i].ap_ezdev;

		if (ezdev->wdev && ap_ezdev->wdev) {
			int apcli_enable = ezdev->driver_ops->get_apcli_enable(ezdev);
			if(apcli_enable == 0)
				continue;
			if (!MAC_ADDR_EQUAL(ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid,ZERO_MAC_ADDR)
				&& (RTMP_TIME_AFTER(now,ezdev->ez_security.ez_roam_info.timestamp + ez_ad->ez_roam_time*ezdev->os_hz))){
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,(" Roaming for Idx=%d failed within 1 min\n",i));
				NdisZeroMemory(ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid,MAC_ADDR_LEN);
				COPY_MAC_ADDR(ap_ezdev->ez_security.ez_ap_roam_blocked_mac,ZERO_MAC_ADDR);
				ez_update_connection_permission_hook(ezdev,EZ_DEQUEUE_PERMISSION);
				ez_update_connection_permission_hook(ezdev,EZ_ALLOW_ALL);
			}
		}
	}
}
#endif


#ifdef EZ_ROAM_SUPPORT



struct _ez_peer_security_info * ez_peer_table_search_by_node_number(ez_dev_t *ezdev, EZ_NODE_NUMBER ez_node_number)
{
	int i;
	struct _ez_peer_security_info *ez_peer;
	int irq_flags;
	ez_peer = NULL;
	EZ_IRQ_LOCK(ezdev->ez_peer_table_lock, irq_flags);
	for (i = 0; i < EZ_MAX_STA_NUM; i++) {		
		if (EZ_GET_EZBAND_BAND(ezdev->ez_ad,ezdev->ez_band_idx)->ez_peer_table[i].valid &&
			NdisEqualMemory(&ez_node_number, &EZ_GET_EZBAND_BAND(ezdev->ez_ad,ezdev->ez_band_idx)->ez_peer_table[i].device_info.ez_node_number,ez_node_number.path_len + 1)) {
			ez_peer = &EZ_GET_EZBAND_BAND(ezdev->ez_ad,ezdev->ez_band_idx)->ez_peer_table[i];
			break;
		}
	}
	EZ_IRQ_UNLOCK(ezdev->ez_peer_table_lock, irq_flags);
	return ez_peer;
}

struct _ez_peer_security_info *ez_find_link_to_roam_candidate(EZ_ADAPTER *ez_ad,
	ez_dev_t *ezdev, 
	EZ_NODE_NUMBER target_node_number)
{
	EZ_NODE_NUMBER *own_node_number = &ez_ad->device_info.ez_node_number;
	EZ_NODE_NUMBER link_node_number;
	NdisZeroMemory(&link_node_number,sizeof(EZ_NODE_NUMBER));
	if (ez_is_child_node(*own_node_number, target_node_number) == TRUE)
	{
		NdisCopyMemory(link_node_number.root_mac,target_node_number.root_mac,own_node_number->path_len+1);
		link_node_number.path_len = own_node_number->path_len+1;
	}
	else
	{
		NdisCopyMemory(link_node_number.root_mac,own_node_number->root_mac,own_node_number->path_len-1);
		link_node_number.path_len = own_node_number->path_len-1;
	}
	ez_hex_dump("link Node Number ", (PUCHAR)&link_node_number,sizeof(EZ_NODE_NUMBER));
	//if (IS_SINGLE_CHIP_DBDC(ez_ad))
	{
		//find the ez_peer for this link node number
		int i;
		struct _ez_peer_security_info *ez_peer = NULL, *tmp_ez_peer=NULL;
		for (i = 0; i < MAX_EZ_BANDS; i++)
		{
			ez_dev_t  * ap_ezdev = &ez_ad->ez_band_info[i].ap_ezdev;
			ez_dev_t  * cli_ezdev = &ez_ad->ez_band_info[i].cli_ezdev;

			if (ap_ezdev->wdev)
			{
				ez_peer = ez_peer_table_search_by_node_number(ap_ezdev,link_node_number);
				if (ez_peer)
				{
					if (ez_peer->ezdev == ap_ezdev)
					{
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("own APCLI interface is the link.\n"));
					ez_hex_dump("Mac",ez_peer->mac_addr, MAC_ADDR_LEN);
					break;
					}
					else
					{
						tmp_ez_peer = ez_peer;
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_peer found\n"));
						ez_hex_dump("MAC", ez_peer->mac_addr,6);
						ez_peer = NULL;
						continue;
					}
				}
			}
			if (cli_ezdev->wdev)
			{
				ez_peer = ez_peer_table_search_by_node_number(cli_ezdev,link_node_number);
				if (ez_peer)
				{
					if (ez_peer->ezdev == cli_ezdev)
					{
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("own APCLI interface is the link.\n"));
					ez_hex_dump("Mac",ez_peer->mac_addr, MAC_ADDR_LEN);
					break;
					}
					else
					{
						tmp_ez_peer = ez_peer;
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_peer found\n"));
						ez_hex_dump("MAC", ez_peer->mac_addr,6);
						ez_peer = NULL;
						continue;
					}
				}
			}
		}
		return ez_peer != NULL? ez_peer: tmp_ez_peer;
	}
}


PUCHAR ez_get_other_band_bssid(beacon_info_tag_t *beacon_info)
{
	return beacon_info->other_ap_mac;
}
UCHAR ez_get_other_band_channel(beacon_info_tag_t *beacon_info)
{	
	return beacon_info->other_ap_channel;
}

void ez_initiate_roam(ez_dev_t *ezdev, PUCHAR roam_bssid, UCHAR roam_channel)
{
	struct _ez_peer_security_info *ez_peer = NULL;
	if (roam_bssid) {
		NdisCopyMemory(ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid,
						roam_bssid,MAC_ADDR_LEN);
		ez_hex_dump("RoamBssid",roam_bssid,6);
		ezdev->ez_security.ez_roam_info.roam_channel = roam_channel;
		NdisGetSystemUpTime(&ezdev->ez_security.ez_roam_info.timestamp);
	}
	else
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_initiate_roam: RoamBssid is NULL!!! \n"));

	ez_peer = ez_peer_table_search_by_addr_hook(ezdev,ezdev->bssid);
	if (ez_peer) {
		ez_hex_dump("address :", ez_peer->mac_addr, 6);
		
		ez_peer->delete_in_differred_context = TRUE;
		ez_peer->ez_disconnect_due_roam = TRUE;
		ezdev->driver_ops->ez_send_unicast_deauth(ezdev, ezdev->bssid);
	}
}

BOOLEAN ez_is_bss_user_configured(beacon_info_tag_t *beacon_info)
{
	return (beacon_info->network_weight[0] == 0x0f);
}

#endif

#ifdef EZ_PUSH_BW_SUPPORT
 void update_ap_peer_record(EZ_ADAPTER *ez_ad, void * ezdev, BOOLEAN this_band_changed, BOOLEAN other_band_changed)
{

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO,("%s >>>\n",__FUNCTION__));

	if(this_band_changed)
		ez_update_this_band_ap_peer_record(ez_ad, ezdev);

	if(other_band_changed)	
		ez_update_other_band_ap_peer_record(ez_ad, ezdev);

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO,("%s <<<\n",__FUNCTION__));

}

 void update_cli_peer_record(EZ_ADAPTER *ez_ad, void * ezdev, BOOLEAN this_band_changed, BOOLEAN other_band_changed)
{
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO,("%s >>>\n",__FUNCTION__));

	if(this_band_changed)
		ez_update_this_band_cli_peer_record(ez_ad, ezdev);

	if(other_band_changed)
		ez_update_other_band_cli_peer_record(ez_ad, ezdev);
	
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO,("%s <<<\n",__FUNCTION__));

}

 void update_peer_record(EZ_ADAPTER *ez_ad, void * ezdev, BOOLEAN this_band_changed, BOOLEAN other_band_changed)
{
	if(!ez_ad->push_bw_config)
		return;

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s >>>\n",__FUNCTION__));

	update_ap_peer_record(ez_ad, ezdev, this_band_changed, other_band_changed);

	update_cli_peer_record(ez_ad, ezdev, this_band_changed, other_band_changed);

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s <<<\n",__FUNCTION__));

}
#endif

//! Levarage from MP1.0 CL#170037
void ez_prepare_man_plus_nonman_nonez_device_info_to_app(EZ_ADAPTER *ez_ad, man_plus_nonman_nonez_device_info_to_app_t *dev_info)
{

	NdisZeroMemory(dev_info, sizeof(man_plus_nonman_nonez_device_info_to_app_t));
	
	NdisCopyMemory(dev_info->non_ez_ssid, ez_ad->non_man_info.ssid, ez_ad->non_man_info.ssid_len);		
//! Leverage form MP.1.0 CL 170364
	dev_info->non_ez_ssid_len = ez_ad->non_man_info.ssid_len;
	NdisCopyMemory(dev_info->non_ez_psk, ez_ad->non_man_info.psk, strlen(ez_ad->non_man_info.psk));
	NdisCopyMemory(dev_info->non_ez_auth_mode, ez_ad->non_man_info.authmode, strlen(ez_ad->non_man_info.authmode));
	NdisCopyMemory(dev_info->non_ez_encryptype, ez_ad->non_man_info.encryptype, strlen(ez_ad->non_man_info.encryptype));	
#ifdef DOT11R_FT_SUPPORT
	NdisCopyMemory(dev_info->ftmdid, ez_ad->non_man_info.FtMdId, FT_MDID_LEN);	
#endif

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("\n %s send ...\n", __FUNCTION__));
}

void ez_prepare_man_plus_nonman_ez_device_info_to_app(EZ_ADAPTER *ez_ad, man_plus_nonman_ez_device_info_to_app_t *dev_info)
{
	if (ez_ad->ez_band_info[0].ap_ezdev.wdev == NULL || ez_ad->ez_band_info[0].cli_ezdev.wdev == NULL)
	{
		return;
	}
	NdisZeroMemory(dev_info, sizeof(man_plus_nonman_ez_device_info_to_app_t));

	NdisCopyMemory(dev_info->network_weight, ez_ad->device_info.network_weight, NETWORK_WEIGHT_LEN);
	NdisCopyMemory(&dev_info->node_number, &ez_ad->device_info.ez_node_number, 	sizeof(EZ_NODE_NUMBER));

	dev_info->internet_access = ez_ad->ez_band_info[0].ap_ezdev.ez_security.go_internet;
	dev_info->ssid_len = ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.shared_info.ssid_len;
	
	NdisCopyMemory(dev_info->ssid, ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.shared_info.ssid, dev_info->ssid_len);
	NdisCopyMemory(dev_info->pmk, ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.pmk, LEN_PMK);
	NdisCopyMemory(dev_info->peer_mac, ez_ad->ez_band_info[0].cli_ezdev.bssid, MAC_ADDR_LEN);	

	if (ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.non_easy_connection)
		dev_info->is_non_ez_connection = TRUE;
	else 
		dev_info->is_non_ez_connection = FALSE;
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("\n %s send ...\n", __FUNCTION__));
}

void ez_prepare_triband_nonez_device_info_to_app(EZ_ADAPTER *ez_ad, triband_nonez_device_info_to_app_t *dev_info)
{

	UCHAR authModeString[32] = {0};
	UCHAR encrypTypeString[32] = {0};
	NdisZeroMemory(dev_info,sizeof(triband_nonez_device_info_to_app_t));		
	if((*ez_ad->non_ez_band_info[0].channel <= 14) 
			||((*ez_ad->non_ez_band_info[0].channel > 14)
				&& (*ez_ad->non_ez_band_info[1].channel > 14))){
		NdisCopyMemory(dev_info->non_ez_psk1, ez_ad->non_ez_band_info[0].psk, LEN_PSK);
		NdisCopyMemory(dev_info->non_ez_psk2, ez_ad->non_ez_band_info[1].psk, LEN_PSK);

		NdisCopyMemory(authModeString, EzGetAuthModeStr(ez_ad->non_ez_band_info[0].triband_sec.AKMMap), 
			strlen(EzGetAuthModeStr(ez_ad->non_ez_band_info[0].triband_sec.AKMMap)));
		NdisCopyMemory(dev_info->non_ez_auth_mode1,authModeString,strlen(authModeString));

		
		NdisZeroMemory(authModeString,sizeof(authModeString));
		NdisCopyMemory(authModeString, 
			EzGetAuthModeStr(ez_ad->non_ez_band_info[1].triband_sec.AKMMap), 
			strlen(EzGetAuthModeStr(ez_ad->non_ez_band_info[1].triband_sec.AKMMap)));

		NdisCopyMemory(dev_info->non_ez_auth_mode2,authModeString,strlen(authModeString));

		
		NdisCopyMemory(encrypTypeString, 
			EzGetEncryModeStr(ez_ad->non_ez_band_info[0].triband_sec.PairwiseCipher), 
			strlen(EzGetEncryModeStr(ez_ad->non_ez_band_info[0].triband_sec.PairwiseCipher)));
		NdisCopyMemory(dev_info->non_ez_encryptype1,encrypTypeString,strlen(encrypTypeString));

		NdisZeroMemory(encrypTypeString,sizeof(encrypTypeString));
		NdisCopyMemory(encrypTypeString, 
				EzGetEncryModeStr(ez_ad->non_ez_band_info[1].triband_sec.PairwiseCipher), 
				strlen(EzGetEncryModeStr(ez_ad->non_ez_band_info[1].triband_sec.PairwiseCipher)));

		NdisCopyMemory(dev_info->non_ez_encryptype2,encrypTypeString,strlen(encrypTypeString));
	} else {
		NdisCopyMemory(dev_info->non_ez_psk1, ez_ad->non_ez_band_info[1].psk, LEN_PSK);
		NdisCopyMemory(dev_info->non_ez_psk2, ez_ad->non_ez_band_info[0].psk, LEN_PSK);

		NdisCopyMemory(authModeString, EzGetAuthModeStr(ez_ad->non_ez_band_info[1].triband_sec.AKMMap), 
			strlen(EzGetAuthModeStr(ez_ad->non_ez_band_info[1].triband_sec.AKMMap)));
		NdisCopyMemory(dev_info->non_ez_auth_mode1,authModeString,strlen(authModeString));

		
		NdisZeroMemory(authModeString,sizeof(authModeString));
		NdisCopyMemory(authModeString, 
			EzGetAuthModeStr(ez_ad->non_ez_band_info[0].triband_sec.AKMMap), 
			strlen(EzGetAuthModeStr(ez_ad->non_ez_band_info[0].triband_sec.AKMMap)));

		NdisCopyMemory(dev_info->non_ez_auth_mode2,authModeString,strlen(authModeString));

		
		NdisCopyMemory(encrypTypeString, 
			EzGetEncryModeStr(ez_ad->non_ez_band_info[1].triband_sec.PairwiseCipher), 
			strlen(EzGetEncryModeStr(ez_ad->non_ez_band_info[1].triband_sec.PairwiseCipher)));
		NdisCopyMemory(dev_info->non_ez_encryptype1,encrypTypeString,strlen(encrypTypeString));

		NdisZeroMemory(encrypTypeString,sizeof(encrypTypeString));
		NdisCopyMemory(encrypTypeString, 
				EzGetEncryModeStr(ez_ad->non_ez_band_info[0].triband_sec.PairwiseCipher), 
				strlen(EzGetEncryModeStr(ez_ad->non_ez_band_info[0].triband_sec.PairwiseCipher)));

		NdisCopyMemory(dev_info->non_ez_encryptype2,encrypTypeString,strlen(encrypTypeString));
	}
}

void ez_prepare_triband_ez_device_info_to_app(EZ_ADAPTER *ez_ad, triband_ez_device_info_to_app_t *dev_info)
{
	if (ez_ad->ez_band_info[0].ap_ezdev.wdev == NULL || ez_ad->ez_band_info[0].cli_ezdev.wdev == NULL)
	{
		return;
	}
	NdisZeroMemory(dev_info,sizeof(triband_ez_device_info_to_app_t));
	NdisCopyMemory(dev_info->network_weight, ez_ad->device_info.network_weight, NETWORK_WEIGHT_LEN);
	NdisCopyMemory(&dev_info->node_number, &ez_ad->device_info.ez_node_number, 
		sizeof(EZ_NODE_NUMBER));
	dev_info->internet_access = ez_ad->ez_band_info[0].ap_ezdev.ez_security.go_internet;

	dev_info->ssid_len = ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.shared_info.ssid_len;
	
	NdisCopyMemory(dev_info->ssid, ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.shared_info.ssid, dev_info->ssid_len);
	NdisCopyMemory(dev_info->pmk, ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.pmk, LEN_PMK);

	NdisCopyMemory(dev_info->peer_mac, ez_ad->ez_band_info[0].cli_ezdev.bssid, MAC_ADDR_LEN);	

	if((*ez_ad->non_ez_band_info[0].channel <= 14) 
			||((*ez_ad->non_ez_band_info[0].channel > 14)
					&& (*ez_ad->non_ez_band_info[1].channel > 14))){
		dev_info->non_ez_ssid1_len = ez_ad->non_ez_band_info[0].ssid_len;
		dev_info->non_ez_ssid2_len = ez_ad->non_ez_band_info[1].ssid_len;
		
		NdisCopyMemory(dev_info->non_ez_ssid1, ez_ad->non_ez_band_info[0].ssid, ez_ad->non_ez_band_info[0].ssid_len);
		NdisCopyMemory(dev_info->non_ez_ssid2, ez_ad->non_ez_band_info[1].ssid, ez_ad->non_ez_band_info[1].ssid_len);
	} else {
		dev_info->non_ez_ssid1_len = ez_ad->non_ez_band_info[1].ssid_len;
		dev_info->non_ez_ssid2_len = ez_ad->non_ez_band_info[0].ssid_len;
		
		NdisCopyMemory(dev_info->non_ez_ssid1, ez_ad->non_ez_band_info[1].ssid, ez_ad->non_ez_band_info[1].ssid_len);
		NdisCopyMemory(dev_info->non_ez_ssid2, ez_ad->non_ez_band_info[0].ssid, ez_ad->non_ez_band_info[0].ssid_len);
	}
	if (ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.non_easy_connection)
		dev_info->is_non_ez_connection = TRUE;
	else 
		dev_info->is_non_ez_connection = FALSE;
}


void ez_prepare_device_info_to_app(EZ_ADAPTER *ez_ad, device_info_to_app_t *dev_info)
{
	int i;
	if (ez_ad->ez_band_info[0].ap_ezdev.wdev == NULL || ez_ad->ez_band_info[0].cli_ezdev.wdev  == NULL)
	{
		return;
	}
	if(ez_ad->band_count ==  2)
	{
		if (ez_ad->ez_band_info[1].ap_ezdev.wdev == NULL || ez_ad->ez_band_info[1].cli_ezdev.wdev == NULL)
		{
			return;
		}
	}

	NdisZeroMemory(dev_info,sizeof(device_info_to_app_t));	
	NdisCopyMemory(dev_info->network_weight, ez_ad->device_info.network_weight, NETWORK_WEIGHT_LEN);
	NdisCopyMemory(&dev_info->node_number, &ez_ad->device_info.ez_node_number, 	sizeof(EZ_NODE_NUMBER));
	dev_info->internet_access = ez_ad->ez_band_info[0].ap_ezdev.ez_security.go_internet;

	dev_info->device_connected[0] = !MAC_ADDR_EQUAL(ez_ad->ez_band_info[0].cli_ezdev.bssid, ZERO_MAC_ADDR);
	dev_info->ssid_len1 = ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.shared_info.ssid_len;	
	NdisCopyMemory(dev_info->ssid1, ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.shared_info.ssid, dev_info->ssid_len1);
	NdisCopyMemory(dev_info->pmk1, ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.pmk, LEN_PMK);
	dev_info->dual_chip_dbdc = IS_DUAL_CHIP_DBDC(ez_ad);

	if (ez_ad->band_count == 2)
	{
		dev_info->device_connected[1] = !MAC_ADDR_EQUAL(ez_ad->ez_band_info[1].cli_ezdev.bssid, ZERO_MAC_ADDR);	
		dev_info->ssid_len2 = ez_ad->ez_band_info[1].cli_ezdev.ez_security.this_band_info.shared_info.ssid_len;
		NdisCopyMemory(dev_info->ssid2, ez_ad->ez_band_info[1].cli_ezdev.ez_security.this_band_info.shared_info.ssid, dev_info->ssid_len2);
		NdisCopyMemory(dev_info->pmk2, ez_ad->ez_band_info[1].cli_ezdev.ez_security.this_band_info.pmk, LEN_PMK);
	}
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("dev_info->ssid1 %s, dev_info->ssid1 %s\n", dev_info->ssid1, dev_info->ssid2));
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("dev_info->device_connected[0] %d, dev_info->device_connected[1] %d\n", dev_info->device_connected[0], dev_info->device_connected[1]));

	if(ez_ad->band_count == 2) {
		if(dev_info->device_connected[0]) {
			if(*(ez_ad->ez_band_info[0].cli_ezdev.channel) <= 14) {
				NdisCopyMemory(dev_info->peer2p4mac, ez_ad->ez_band_info[0].cli_ezdev.bssid, MAC_ADDR_LEN);	
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("\n Line : %d\n", __LINE__));
			} else {
				struct _ez_peer_security_info *ez_peer = NULL;
				ez_peer = ez_peer_table_search_by_addr_hook(&ez_ad->ez_band_info[0].cli_ezdev,
					ez_ad->ez_band_info[0].cli_ezdev.bssid);
				if(ez_peer)
				{
					NdisCopyMemory(dev_info->peer2p4mac, 
						ez_peer->other_band_info.shared_info.ap_mac_addr, MAC_ADDR_LEN);
					
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("\n Line : %d\n", __LINE__));
				}
			}
		} else if(dev_info->device_connected[1]){

			if(*(ez_ad->ez_band_info[1].cli_ezdev.channel) <= 14) {
				NdisCopyMemory(dev_info->peer2p4mac, ez_ad->ez_band_info[1].cli_ezdev.bssid, MAC_ADDR_LEN); 
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("\n Line : %d\n", __LINE__));
			} else {
				struct _ez_peer_security_info *ez_peer = NULL;
				ez_peer = ez_peer_table_search_by_addr_hook(&ez_ad->ez_band_info[1].cli_ezdev,
					ez_ad->ez_band_info[1].cli_ezdev.bssid);
				if(ez_peer)
				{
					NdisCopyMemory(dev_info->peer2p4mac, 
						ez_peer->other_band_info.shared_info.ap_mac_addr, MAC_ADDR_LEN);
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("\n Line : %d\n", __LINE__));
				}
			}
		}
	}else {
		NdisCopyMemory(dev_info->peer2p4mac, ez_ad->ez_band_info[0].cli_ezdev.bssid, MAC_ADDR_LEN); 
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("\n Line : %d\n", __LINE__));
	}	
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("\n dev_info->peer2p4mac : "));
	
	for (i = 0; i < 6; ++i)
	  EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, (" %02x", dev_info->peer2p4mac[i]));
	
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("\n\n"));

	
	if (ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.non_easy_connection)
		dev_info->non_ez_connection = TRUE;
	else 
		dev_info->non_ez_connection = FALSE;

	if (ez_ad->band_count == 2)
	{
		if (ez_ad->ez_band_info[1].cli_ezdev.ez_security.this_band_info.non_easy_connection)
				dev_info->non_ez_connection = TRUE;
	}

	//printk("\n band_0 : %d, band_1 : %d \n", *(ez_ad->ez_band_info[0].cli_ezdev.channel), 
	//	*(ez_ad->ez_band_info[1].cli_ezdev.channel) );
	if (ez_ad->band_count == 2)
	{
		if((*(ez_ad->ez_band_info[0].cli_ezdev.channel) <= 14)
				|| ((*(ez_ad->ez_band_info[0].cli_ezdev.channel) > 14) && (*(ez_ad->ez_band_info[1].cli_ezdev.channel) > 14))){

			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, (" Do nothing this is 1st 2G band and 2nd 5G band\n"));
		} else {

			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, (" Reverst ssid and pmk this is 1st 5G band and 2nd 2G band\n"));
			{
				dev_info->ssid_len1 = ez_ad->ez_band_info[1].cli_ezdev.ez_security.this_band_info.shared_info.ssid_len;
				NdisCopyMemory(dev_info->ssid1, ez_ad->ez_band_info[1].cli_ezdev.ez_security.this_band_info.shared_info.ssid, dev_info->ssid_len1);
				NdisCopyMemory(dev_info->pmk1, ez_ad->ez_band_info[1].cli_ezdev.ez_security.this_band_info.pmk, LEN_PMK);
				dev_info->ssid_len2 = ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.shared_info.ssid_len;
				NdisCopyMemory(dev_info->ssid2, ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.shared_info.ssid, dev_info->ssid_len2);
				NdisCopyMemory(dev_info->pmk2, ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.pmk, LEN_PMK);
			}
		}
	}


	
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("dev_info->non_ez_connection %d\n", dev_info->non_ez_connection));	
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("\n ez_prepare_device_info_to_app send ...\n"));
}

BOOLEAN push_and_update_ap_config(EZ_ADAPTER *ez_ad, void * ezdev, updated_configs_t *updated_configs, BOOLEAN group_id_diff)
{

	BOOLEAN action_sent = FALSE;	
	
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s >>>\n",__FUNCTION__));
#ifdef RT_CFG80211_SUPPORT
		ez_dev_t  * ap_ezdev = (ez_dev_t *)ezdev;
		interface_info_t other_band_info;
		if (ez_get_other_band_info(ezdev, &other_band_info)){
			if (!SSID_EQUAL(updated_configs->other_band_info.shared_info.ssid, updated_configs->other_band_info.shared_info.ssid_len,
				other_band_info.shared_info.ssid, other_band_info.shared_info.ssid_len)) {
				ez_ad->u4ConfigPushTriggered = TRUE;
			}
			if (!NdisEqualMemory(updated_configs->this_band_info.psk, other_band_info.psk, EZ_LEN_PSK)) {
				ez_ad->u4ConfigPushTriggered = TRUE;
			}
		}
		if (!SSID_EQUAL(updated_configs->this_band_info.shared_info.ssid, updated_configs->this_band_info.shared_info.ssid_len,
			ap_ezdev->ez_security.this_band_info.shared_info.ssid, ap_ezdev->ez_security.this_band_info.shared_info.ssid_len)) {
			ez_ad->u4ConfigPushTriggered = TRUE;
		}
		if (!NdisEqualMemory(updated_configs->this_band_info.psk, ap_ezdev->ez_security.this_band_info.psk, EZ_LEN_PSK)) {
			ez_ad->u4ConfigPushTriggered = TRUE;
		}
#endif
	if (ez_update_this_band_ap(ez_ad, ezdev, updated_configs, group_id_diff))
	{
		action_sent = TRUE;
	}
	
	if (ez_update_other_band_ap(ez_ad, ezdev, updated_configs, group_id_diff))
	{
		action_sent = TRUE;
	}

	return action_sent;
}

void ez_update_non_ez_ap(EZ_ADAPTER *ez_ad,
					NON_EZ_BAND_INFO_TAG *non_ez_and_info_tag, 
					NON_EZ_BAND_INFO *non_ez_band_info,
					updated_configs_t *updated_configs,
					int band_count)
{
	EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s >>>\n", __FUNCTION__));
	NdisCopyMemory(non_ez_band_info->ssid, non_ez_and_info_tag->ssid, non_ez_and_info_tag->ssid_len);
	NdisCopyMemory(non_ez_band_info->pmk, non_ez_and_info_tag->encrypted_pmk, EZ_PMK_LEN);

	non_ez_band_info->ssid_len = non_ez_and_info_tag->ssid_len;
	non_ez_band_info->triband_sec.AKMMap = non_ez_and_info_tag->triband_sec.AKMMap;
	non_ez_band_info->triband_sec.PairwiseCipher = non_ez_and_info_tag->triband_sec.PairwiseCipher;
	non_ez_band_info->triband_sec.GroupCipher= non_ez_and_info_tag->triband_sec.GroupCipher;
	
		
#ifdef DOT11R_FT_SUPPORT
	FT_SET_MDID(non_ez_band_info->FtMdId,non_ez_and_info_tag->FtMdId);
#endif

	//printk("ez_update_non_ez_ap pointer === %p\n", ez_ad->non_ez_band_info[band_count].lut_driver_ops.ez_update_non_ez_ap);
	ez_ad->non_ez_band_info[band_count].lut_driver_ops.ez_update_non_ez_ap(ez_ad->non_ez_band_info[band_count].pAd,
		non_ez_and_info_tag, non_ez_band_info, updated_configs, band_count);
	
		
					
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s <<<\n", __FUNCTION__));

}


BOOLEAN push_and_update_cli_config(EZ_ADAPTER *ez_ad, void * ezdev, updated_configs_t *updated_configs, BOOLEAN group_id_diff)
{
	BOOLEAN action_sent = FALSE, is_event_req = TRUE;
	
	device_info_to_app_t dev_info;
	triband_ez_device_info_to_app_t triband_ez_dev_info;
	triband_nonez_device_info_to_app_t triband_nonez_dev_info;
//! Levarage from MP1.0 CL#170037
	man_plus_nonman_ez_device_info_to_app_t man_plus_nonman_ez_dev_info;
	man_plus_nonman_nonez_device_info_to_app_t man_plus_nonman_nonez_dev_info;
	
	int band_count = 0;
	if (((ez_dev_t *)(ezdev))->wdev == NULL)
	{
		return FALSE;
	}
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s >>>\n",__FUNCTION__));

	if (ez_update_this_band_cli(ez_ad, ezdev, updated_configs, group_id_diff))
	{
		action_sent = TRUE;
	}
	
	if (ez_update_other_band_cli(ez_ad, ezdev, updated_configs, group_id_diff))
	{
		action_sent = TRUE;
	}
	
	if(group_id_diff)
	{
		unsigned int group_id_len,count,backupcount;
		unsigned char *group_id_ptr;
		ez_group_id_t group_id;
		NdisZeroMemory(&group_id,sizeof(ez_group_id_t));
		if(updated_configs->gen_group_id)
		{
			group_id.ucFlags = BIT(7);
			group_id_ptr = updated_configs->gen_group_id;
			group_id_len = updated_configs->gen_group_id_len;
		}
		else
		{
			group_id_ptr = updated_configs->group_id;
			group_id_len = updated_configs->group_id_len;
		}

		group_id.open_group_id_len = updated_configs->open_group_id_len;
		NdisCopyMemory(&group_id.open_group_id,updated_configs->open_group_id,updated_configs->open_group_id_len);

		if(updated_configs->group_id_len < GROUPID_LEN_BUF)
		{
			group_id.ez_group_id_len = group_id_len;
			group_id.ucFlags |= 1;

			NdisCopyMemory(&group_id.ez_group_id,group_id_ptr,group_id_len);

			EzRtmpOSWrielessEventSend(ezdev, RT_WLAN_EVENT_CUSTOM, OID_WH_EZ_GROUP_ID_UPDATE,
					NULL, (void *)&group_id, sizeof(ez_group_id_t));
		}
		else
		{
			if(group_id_len % GROUPID_LEN_BUF)
			{
				group_id.ucFlags |= (group_id_len/GROUPID_LEN_BUF) + 1;
			}
			else
			{
				group_id.ucFlags |= group_id_len/GROUPID_LEN_BUF;
			}
			backupcount = group_id.ucFlags & 0x7f ;
			for (count = 0 ; count < backupcount; count++)
			{
					NdisZeroMemory(group_id.ez_group_id,GROUPID_LEN_BUF+1);
					if(group_id_len ==0 )
					{
						break;
					}
					if(group_id_len < GROUPID_LEN_BUF)
					{
						group_id.ez_group_id_len = group_id_len;
						group_id_len = 0;
					}
					else
					{
						group_id.ez_group_id_len = GROUPID_LEN_BUF;
						group_id_len -= GROUPID_LEN_BUF; 
					}
					NdisCopyMemory(group_id.ez_group_id,&group_id_ptr[count * GROUPID_LEN_BUF],group_id.ez_group_id_len );
					EzRtmpOSWrielessEventSend(ezdev, RT_WLAN_EVENT_CUSTOM, OID_WH_EZ_GROUP_ID_UPDATE,
							NULL, (void *)&group_id, sizeof(ez_group_id_t));
					group_id.ucFlags--;
				}
			}
	}
		if(ez_is_triband_hook())
		{
			
			for (band_count = 0; band_count < MAX_NON_EZ_BANDS; band_count++)
			{
#if 0
				ez_ad->non_ez_band_info[band_count].lut_driver_ops.ez_update_non_ez_ap(ez_ad->non_ez_band_info[band_count].ez_ad,
					&updated_configs->non_ez_info[band_count], &ez_ad->non_ez_band_info[band_count], updated_configs);
#else
				ez_update_non_ez_ap(ez_ad,
					&updated_configs->non_ez_info[band_count], &ez_ad->non_ez_band_info[band_count], updated_configs, band_count);
#endif
			}
			
			if ((updated_configs->context_linkdown == FALSE)){
				ez_ad->non_ez_band_info[0].lut_driver_ops.HwCtrlWifiSysRestart(ez_ad->non_ez_band_info[0].pAd);
			}
			//HW_WIFISYS_RESTART((RTMP_ADAPTER *)ez_ad->non_ez_band_info[0].ez_ad);
			//RTCMDUp(&ez_ad->restartNonEzApTask);
			//RTMP_OS_TASKLET_SCHE(&ez_ad->restartNonEzApTasklet);

			ez_prepare_triband_ez_device_info_to_app(ez_ad, &triband_ez_dev_info);
			ez_prepare_triband_nonez_device_info_to_app(ez_ad, &triband_nonez_dev_info);
			triband_ez_dev_info.is_forced = updated_configs->device_info.network_weight[0] & BIT(7);			
			triband_ez_dev_info.update_parameters = updated_configs->need_ez_update;
			triband_ez_dev_info.third_party_present = updated_configs->device_info.network_weight[0] & BIT(6);
			triband_ez_dev_info.new_updated_received =  updated_configs->device_info.network_weight[0] & BIT(5);
			triband_ez_dev_info.need_non_ez_update_ssid[0] = updated_configs->need_non_ez_update_ssid[0];
			triband_ez_dev_info.need_non_ez_update_ssid[1] = updated_configs->need_non_ez_update_ssid[1];
			triband_nonez_dev_info.need_non_ez_update_psk[0] = updated_configs->need_non_ez_update_psk[0];
			triband_nonez_dev_info.need_non_ez_update_psk[1] = updated_configs->need_non_ez_update_psk[1];
			triband_nonez_dev_info.need_non_ez_update_secconfig[0] = updated_configs->need_non_ez_update_secconfig[0];
			triband_nonez_dev_info.need_non_ez_update_secconfig[1] = updated_configs->need_non_ez_update_secconfig[1];
	
			EzRtmpOSWrielessEventSend(ezdev, RT_WLAN_EVENT_CUSTOM, OID_WH_EZ_MAN_TRIBAND_EZ_DEVINFO_EVENT,
						NULL, (void *)&triband_ez_dev_info, sizeof(triband_ez_dev_info));
	
			if (triband_nonez_dev_info.need_non_ez_update_secconfig[0] || triband_nonez_dev_info.need_non_ez_update_secconfig[1] 
				|| triband_nonez_dev_info.need_non_ez_update_psk[0] || triband_nonez_dev_info.need_non_ez_update_psk[1] ) {
				NonEzRtmpOSWrielessEventSend(ez_ad, 0,RT_WLAN_EVENT_CUSTOM, OID_WH_EZ_MAN_TRIBAND_NONEZ_DEVINFO_EVENT,
					NULL, (void *)&triband_nonez_dev_info, sizeof(triband_nonez_dev_info));
			}
		
		} else if(ez_ad->is_man_nonman) {
//! Levarage from MP1.0 CL#170037
			
			ez_prepare_man_plus_nonman_ez_device_info_to_app(ez_ad, &man_plus_nonman_ez_dev_info);
			ez_prepare_man_plus_nonman_nonez_device_info_to_app(ez_ad, &man_plus_nonman_nonez_dev_info);

//! Leverage form MP.1.0 CL 170364
			man_plus_nonman_ez_dev_info.update_parameters = updated_configs->need_ez_update;
			man_plus_nonman_nonez_dev_info.need_non_ez_update_psk = updated_configs->need_non_ez_update_psk[0];
			man_plus_nonman_nonez_dev_info.need_non_ez_update_secconfig = updated_configs->need_non_ez_update_secconfig[0];
			man_plus_nonman_nonez_dev_info.need_non_ez_update_ssid= updated_configs->need_non_ez_update_ssid[0];
			man_plus_nonman_ez_dev_info.third_party_present = updated_configs->device_info.network_weight[0] & BIT(6);
			man_plus_nonman_ez_dev_info.new_updated_received =  updated_configs->device_info.network_weight[0] & BIT(5);

#ifdef SYSTEM_LOG_SUPPORT
			RTMPSendWirelessEvent(ez_ad, OID_WH_EZ_MAN_PLUS_NONMAN_NONEZ_DEVINFO_EVENT, NULL, ezdev->ez_band_idx, 0);
			RTMPSendWirelessEvent(ez_ad, OID_WH_EZ_MAN_PLUS_NONMAN_EZ_DEVINFO_EVENT, NULL, ezdev->ez_band_idx, 0);
#else /* SYSTEM_LOG_SUPPORT */
			NonEzRtmpOSWrielessEventSend(ez_ad, 0, RT_WLAN_EVENT_CUSTOM, OID_WH_EZ_MAN_PLUS_NONMAN_EZ_DEVINFO_EVENT,
						NULL, (void *)&man_plus_nonman_ez_dev_info, sizeof(man_plus_nonman_ez_dev_info));
			NonEzRtmpOSWrielessEventSend(ez_ad, 0, RT_WLAN_EVENT_CUSTOM, OID_WH_EZ_MAN_PLUS_NONMAN_NONEZ_DEVINFO_EVENT,
						NULL, (void *)&man_plus_nonman_nonez_dev_info, sizeof(man_plus_nonman_nonez_dev_info));
#endif /* !SYSTEM_LOG_SUPPORT */
		
		} else {
			ez_prepare_device_info_to_app(ez_ad, &dev_info);
			dev_info.is_push = 0;
			dev_info.is_forced = updated_configs->device_info.network_weight[0] & BIT(7);	
			dev_info.third_party_present = updated_configs->device_info.network_weight[0] & BIT(6);
			dev_info.new_updated_received =	updated_configs->device_info.network_weight[0] & BIT(5);
#ifdef SYSTEM_LOG_SUPPORT
			RTMPSendWirelessEvent(ez_ad, OID_WH_EZ_MAN_DEAMON_EVENT, NULL, ezdev->ez_band_idx, 0);
#else /* SYSTEM_LOG_SUPPORT */
			if (ez_ad->ez_band_info[0].ap_ezdev.wdev == NULL || ez_ad->ez_band_info[0].cli_ezdev.wdev  == NULL)
			{
				is_event_req = FALSE;
			}	
			
			if(ez_ad->band_count ==  2)
			{
				if (ez_ad->ez_band_info[1].ap_ezdev.wdev == NULL || ez_ad->ez_band_info[1].cli_ezdev.wdev == NULL)
				{
					is_event_req = FALSE;
				}
			}
			if(is_event_req)
			{
					EzRtmpOSWrielessEventSend(ezdev, RT_WLAN_EVENT_CUSTOM, OID_WH_EZ_MAN_DEAMON_EVENT,
									NULL, (void *)&dev_info, sizeof(device_info_to_app_t));
#ifdef RT_CFG80211_SUPPORT
					ez_ad->u4ConfigPushTriggered = FALSE;
#endif
			} else {
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("\n %s : %d don't send OID_WH_EZ_MAN_DEAMON_EVENT \n", __FUNCTION__, __LINE__));

			}
			
#endif /* !SYSTEM_LOG_SUPPORT */
		
		}

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s <<<\n",__FUNCTION__));
	return action_sent;

}

void ez_init_updated_configs_for_adapt(updated_configs_t *updated_config, struct _ez_peer_security_info *ez_peer, ez_dev_t * ezdev)
{
	EZ_ADAPTER *ez_ad = ez_peer->ad;
	channel_info_t null_channel_info;
	ez_dev_t *this_band_ap_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev;
	ez_dev_t *other_band_ap_ezdev = ez_get_otherband_ap_ezdev(ezdev);
	NdisZeroMemory(&null_channel_info,sizeof(null_channel_info));
	//! copy MAC adrss of the peer from where the config are received so that we do not send them back to the same peer
	COPY_MAC_ADDR(updated_config->mac_addr, ez_peer->mac_addr);

	//! take peer device configurations like weight and node number, node number will be update din the end while sending fram to specific peer
	NdisCopyMemory(&updated_config->device_info, &ez_peer->device_info,sizeof(device_info_t));

	//! TODO: RAGHAV update peer node number here, peer previously sent the weight that we should adapt, now we need to restore it for future refference.
	//! take current band info
	NdisCopyMemory(&updated_config->this_band_info, &ez_peer->this_band_info,sizeof(interface_info_t));
	if (ez_peer->this_band_info.shared_info.channel_info.ht_bw == 0)
	{
		updated_config->this_band_info.shared_info.channel_info.extcha = this_band_ap_ezdev->ez_security.this_band_info.shared_info.channel_info.extcha;
	}

	if (other_band_ap_ezdev && ez_peer->other_band_info.shared_info.channel_info.ht_bw == 0)
	{
		updated_config->other_band_info.shared_info.channel_info.extcha = other_band_ap_ezdev->ez_security.this_band_info.shared_info.channel_info.extcha;
	}

	//! take other band info
	NdisCopyMemory(&updated_config->other_band_info, &ez_peer->other_band_info,sizeof(interface_info_t));
	if(NdisEqualMemory(&null_channel_info,&updated_config->other_band_info.shared_info.channel_info,sizeof(channel_info_t)))
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("peer other band channel info is NULL\n"));
		updated_config->other_band_info.interface_activated = FALSE;
	} else {
			updated_config->other_band_info.interface_activated = TRUE;
	}
	updated_config->this_band_info.interface_activated = TRUE;
	
	//! init pointer to group ID

	updated_config->group_id = ez_peer->group_id;
	updated_config->group_id_len = ez_peer->group_id_len;
	if(ez_peer->gen_group_id)
	{
		updated_config->gen_group_id = ez_peer->gen_group_id;
		updated_config->gen_group_id_len = ez_peer->gen_group_id_len;
	}
	//! init open group ID
	updated_config->open_group_id_len = ez_peer->open_group_id_len;
	NdisCopyMemory(updated_config->open_group_id, ez_peer->open_group_id, ez_peer->open_group_id_len);
	
	//! I am taking configuration from this peer so this becomes my weight providing link.
	NdisCopyMemory(ez_ad->device_info.network_weight,ez_peer->device_info.network_weight, NETWORK_WEIGHT_LEN);
	NdisCopyMemory(&ez_ad->device_info.ez_node_number,&ez_peer->device_info.ez_node_number, sizeof(EZ_NODE_NUMBER));
	// ez_peer->device_info.ez_node_number is overloaded to the node number of the own device that is to be allocated.
	//restore it once the allocation is done.
	ez_restore_node_number(&ez_peer->device_info.ez_node_number);
	
	NdisCopyMemory(ez_ad->device_info.weight_defining_link.peer_mac,ez_peer->mac_addr,MAC_ADDR_LEN);
	NdisCopyMemory(ez_ad->device_info.weight_defining_link.peer_ap_mac,ez_peer->this_band_info.shared_info.ap_mac_addr,MAC_ADDR_LEN);
	NdisGetSystemUpTime(&ez_ad->device_info.weight_defining_link.ap_time_stamp);
	ez_hex_dump("peer_ap_mac", ez_peer->this_band_info.shared_info.ap_mac_addr, 6);
	NdisGetSystemUpTime(&ez_ad->device_info.weight_defining_link.time_stamp);
	ez_ad->device_info.weight_defining_link.ezdev = ezdev;
											
	ez_inform_all_interfaces(ezdev->ez_ad,ezdev,ACTION_UPDATE_DEVICE_INFO);

 		
	if (ez_is_triband_hook())
	{
		UINT encrypted_pmk_len;
		NdisCopyMemory(&updated_config->non_ez_info[0], &ez_peer->non_ez_band_info[0], sizeof(ez_peer->non_ez_band_info));

		NdisZeroMemory(updated_config->non_ez_info[0].encrypted_pmk, sizeof(updated_config->non_ez_info[0].encrypted_pmk));
		NdisZeroMemory(updated_config->non_ez_info[1].encrypted_pmk, sizeof(updated_config->non_ez_info[1].encrypted_pmk));
		
		ez_peer->ezdev->driver_ops->AES_Key_Unwrap(ez_peer->ezdev, ez_peer->non_ez_band_info[0].encrypted_pmk, EZ_PMK_LEN + EZ_AES_KEY_ENCRYPTION_EXTEND,
			   &ez_peer->sw_key[0], LEN_PTK_KEK, 
		updated_config->non_ez_info[0].encrypted_pmk, &encrypted_pmk_len);
		ez_peer->ezdev->driver_ops->AES_Key_Unwrap(ez_peer->ezdev, ez_peer->non_ez_band_info[1].encrypted_pmk, EZ_PMK_LEN + EZ_AES_KEY_ENCRYPTION_EXTEND,
			   &ez_peer->sw_key[0], LEN_PTK_KEK, 
		updated_config->non_ez_info[1].encrypted_pmk, &encrypted_pmk_len);

		NdisZeroMemory(updated_config->non_ez_psk_info[0].encrypted_psk, sizeof(updated_config->non_ez_psk_info[0].encrypted_psk));
		NdisZeroMemory(updated_config->non_ez_psk_info[1].encrypted_psk, sizeof(updated_config->non_ez_psk_info[1].encrypted_psk));
		
		ez_peer->ezdev->driver_ops->AES_Key_Unwrap(ez_peer->ezdev, ez_peer->non_ez_psk_info[0].encrypted_psk, LEN_PSK + EZ_AES_KEY_ENCRYPTION_EXTEND,
			   &ez_peer->sw_key[0], LEN_PTK_KEK, 
		updated_config->non_ez_psk_info[0].encrypted_psk, &encrypted_pmk_len);
		ez_peer->ezdev->driver_ops->AES_Key_Unwrap(ez_peer->ezdev, ez_peer->non_ez_psk_info[1].encrypted_psk, LEN_PSK + EZ_AES_KEY_ENCRYPTION_EXTEND,
			   &ez_peer->sw_key[0], LEN_PTK_KEK, 
		updated_config->non_ez_psk_info[1].encrypted_psk, &encrypted_pmk_len);



		
		if (!SSID_EQUAL(ez_ad->non_ez_band_info[0].ssid, ez_ad->non_ez_band_info[0].ssid_len,updated_config->non_ez_info[0].ssid, updated_config->non_ez_info[0].ssid_len))
		{
			updated_config->need_non_ez_update_ssid[0] = TRUE;
		}

		if (!SSID_EQUAL(ez_ad->non_ez_band_info[1].ssid, ez_ad->non_ez_band_info[1].ssid_len,updated_config->non_ez_info[1].ssid, updated_config->non_ez_info[1].ssid_len))
		{
			updated_config->need_non_ez_update_ssid[1] = TRUE;
		}

		if (!SSID_EQUAL(ez_ad->non_ez_band_info[0].psk, strlen(ez_ad->non_ez_band_info[0].psk),updated_config->non_ez_psk_info[0].encrypted_psk, strlen(updated_config->non_ez_psk_info[0].encrypted_psk)))
		{
			updated_config->need_non_ez_update_psk[0] = TRUE;
		}


		if (!SSID_EQUAL(ez_ad->non_ez_band_info[1].psk, strlen(ez_ad->non_ez_band_info[1].psk),updated_config->non_ez_psk_info[1].encrypted_psk, strlen(updated_config->non_ez_psk_info[1].encrypted_psk)))
		{
		
			updated_config->need_non_ez_update_psk[1] = TRUE;
		}

		if (!SSID_EQUAL(ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.shared_info.ssid, ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.shared_info.ssid_len,
			updated_config->this_band_info.shared_info.ssid, updated_config->this_band_info.shared_info.ssid_len))
		{
			updated_config->need_ez_update = TRUE;
		}

		if (!NdisEqualMemory(updated_config->this_band_info.pmk,ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.pmk,EZ_PMK_LEN))
		{
			updated_config->need_ez_update = TRUE;
		}

		ez_ad->non_ez_band_info[0].ssid_len = updated_config->non_ez_info[0].ssid_len;
		ez_ad->non_ez_band_info[1].ssid_len = updated_config->non_ez_info[1].ssid_len ;

		
		if (NdisEqualMemory(&ez_ad->non_ez_band_info[0].triband_sec, &updated_config->non_ez_info[0].triband_sec,sizeof(EZ_TRIBAND_SEC_CONFIG)))
		{
			
		} else {
			ez_ad->non_ez_band_info[0].need_restart = TRUE;
			updated_config->need_non_ez_update_secconfig[0] = TRUE;
		}

		if (NdisEqualMemory(&ez_ad->non_ez_band_info[1].triband_sec, &updated_config->non_ez_info[1].triband_sec,sizeof(EZ_TRIBAND_SEC_CONFIG)))
		{
			
		} else {
			ez_ad->non_ez_band_info[1].need_restart = TRUE;
			updated_config->need_non_ez_update_secconfig[1] = TRUE;
		}
		

		NdisCopyMemory(ez_ad->non_ez_band_info[0].ssid, updated_config->non_ez_info[0].ssid, ez_ad->non_ez_band_info[0].ssid_len);
		NdisCopyMemory(&ez_ad->non_ez_band_info[0].triband_sec, &updated_config->non_ez_info[0].triband_sec, sizeof(ez_ad->non_ez_band_info[0].triband_sec));

		NdisCopyMemory(ez_ad->non_ez_band_info[1].ssid, updated_config->non_ez_info[1].ssid, ez_ad->non_ez_band_info[1].ssid_len);
		NdisCopyMemory(&ez_ad->non_ez_band_info[1].triband_sec, &updated_config->non_ez_info[1].triband_sec, sizeof(ez_ad->non_ez_band_info[1].triband_sec));


		NdisZeroMemory(ez_ad->non_ez_band_info[0].pmk, EZ_PMK_LEN);
		NdisZeroMemory(ez_ad->non_ez_band_info[1].pmk, EZ_PMK_LEN);
		
		NdisCopyMemory(ez_ad->non_ez_band_info[0].pmk, updated_config->non_ez_info[0].encrypted_pmk, EZ_PMK_LEN);
		NdisCopyMemory(ez_ad->non_ez_band_info[1].pmk, updated_config->non_ez_info[1].encrypted_pmk, EZ_PMK_LEN);

		NdisZeroMemory(ez_ad->non_ez_band_info[0].psk, LEN_PSK);
		NdisZeroMemory(ez_ad->non_ez_band_info[1].psk, LEN_PSK);
	
		NdisCopyMemory(ez_ad->non_ez_band_info[0].psk, updated_config->non_ez_psk_info[0].encrypted_psk, LEN_PSK);
		NdisCopyMemory(ez_ad->non_ez_band_info[1].psk, updated_config->non_ez_psk_info[1].encrypted_psk, LEN_PSK);
	
	} else if(ez_ad->is_man_nonman) {

		UINT encrypted_pmk_len;
		NdisCopyMemory(&updated_config->non_man_info, &ez_peer->non_man_info, sizeof(ez_peer->non_man_info));

		NdisZeroMemory(updated_config->non_man_info.encrypted_psk, sizeof(updated_config->non_man_info.encrypted_psk));
		ez_peer->ezdev->driver_ops->AES_Key_Unwrap(ez_peer->ezdev,
				ez_peer->non_man_info.encrypted_psk, LEN_PSK + EZ_AES_KEY_ENCRYPTION_EXTEND,
				&ez_peer->sw_key[0], LEN_PTK_KEK, 
				updated_config->non_man_info.encrypted_psk, &encrypted_pmk_len);

		NdisZeroMemory(ez_ad->non_man_info.ssid, MAX_LEN_OF_SSID);
//! Leverage form MP.1.0 CL 170364
		NdisZeroMemory(ez_ad->non_man_info.encryptype, sizeof(ez_ad->non_man_info.encryptype));
		NdisZeroMemory(ez_ad->non_man_info.authmode, sizeof(ez_ad->non_man_info.authmode));
#ifdef DOT11R_FT_SUPPORT		
		NdisZeroMemory(ez_ad->non_man_info.FtMdId, FT_MDID_LEN);
#endif		
//! Leverage form MP.1.0 CL 170364
		if (!SSID_EQUAL(ez_ad->non_man_info.ssid, ez_ad->non_man_info.ssid_len,updated_config->non_man_info.ssid, updated_config->non_man_info.ssid_len))
		{
			updated_config->need_non_ez_update_ssid[0] = TRUE;
		}
		
//! Leverage form MP.1.0 CL 170364
		if (!SSID_EQUAL(ez_ad->non_man_info.psk, strlen(ez_ad->non_man_info.psk),updated_config->non_man_info.encrypted_psk, strlen(updated_config->non_man_info.encrypted_psk)))
		{
			updated_config->need_non_ez_update_psk[0] = TRUE;
		}

//! Leverage form MP.1.0 CL 170364
		if (!SSID_EQUAL(ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.shared_info.ssid, ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.shared_info.ssid_len,
			updated_config->this_band_info.shared_info.ssid, updated_config->this_band_info.shared_info.ssid_len))
		{
			updated_config->need_ez_update = TRUE;
		}

//! Leverage form MP.1.0 CL 170364
		if (!NdisEqualMemory(updated_config->this_band_info.pmk,ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.pmk,EZ_PMK_LEN))
		{
			updated_config->need_ez_update = TRUE;
		}

//! Leverage form MP.1.0 CL 170364
		if (!SSID_EQUAL(ez_ad->non_man_info.encryptype, strlen(ez_ad->non_man_info.encryptype), updated_config->non_man_info.encryptype, strlen(updated_config->non_man_info.encryptype))
			|| !SSID_EQUAL(ez_ad->non_man_info.authmode, strlen(ez_ad->non_man_info.authmode), updated_config->non_man_info.authmode, strlen(updated_config->non_man_info.authmode)))
		{
			updated_config->need_non_ez_update_secconfig[0] = TRUE;
		}

		ez_ad->non_man_info.ssid_len = updated_config->non_man_info.ssid_len;
		NdisCopyMemory(ez_ad->non_man_info.ssid, updated_config->non_man_info.ssid, ez_ad->non_man_info.ssid_len);
		NdisCopyMemory(ez_ad->non_man_info.encryptype, updated_config->non_man_info.encryptype, strlen(updated_config->non_man_info.encryptype));
		NdisCopyMemory(ez_ad->non_man_info.authmode, updated_config->non_man_info.authmode, strlen(updated_config->non_man_info.authmode));
#ifdef DOT11R_FT_SUPPORT
		NdisCopyMemory(ez_ad->non_man_info.FtMdId, updated_config->non_man_info.FtMdId, FT_MDID_LEN);
#endif
		NdisZeroMemory(ez_ad->non_man_info.psk, LEN_PSK);
		NdisCopyMemory(ez_ad->non_man_info.psk, updated_config->non_man_info.encrypted_psk, LEN_PSK);



	}	
	
 }

void ez_init_updated_configs_for_push(updated_configs_t *updated_config, ez_dev_t * ezdev)
{

	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	ez_dev_t *ap_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev;
	
	NdisCopyMemory(&updated_config->this_band_info, &ezdev->ez_security.this_band_info , sizeof(interface_info_t));
	if (ez_get_other_band_info(ezdev, &updated_config->other_band_info)){
	} else {
		NdisZeroMemory(&updated_config->other_band_info.shared_info, sizeof(interface_info_tag_t));
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("otherband interface not activated\n"));
		if (ezdev->ez_security.other_band_info_backup.interface_activated)
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("ez_init_updated_configs_for_push --->will use backup\n"));
			updated_config->other_band_info.shared_info.ssid_len = ezdev->ez_security.other_band_info_backup.shared_info.ssid_len;
			NdisCopyMemory(updated_config->other_band_info.shared_info.ssid, ezdev->ez_security.other_band_info_backup.shared_info.ssid, ezdev->ez_security.other_band_info_backup.shared_info.ssid_len);
			NdisCopyMemory(&updated_config->other_band_info.shared_info.channel_info, &ezdev->ez_security.other_band_info_backup.shared_info.channel_info, sizeof(channel_info_t));
#ifdef DOT11R_FT_SUPPORT
			FT_SET_MDID(updated_config->other_band_info.shared_info.FtMdId,ezdev->ez_security.other_band_info_backup.shared_info.FtMdId);
#endif
		}

	}
	COPY_MAC_ADDR(updated_config->mac_addr, ez_ad->device_info.weight_defining_link.peer_mac);
	NdisCopyMemory(&updated_config->device_info, &ez_ad->device_info,sizeof(device_info_t));
	updated_config->group_id = ezdev->ez_security.group_id;
	updated_config->group_id_len= ezdev->ez_security.group_id_len;
	updated_config->open_group_id_len= ezdev->ez_security.open_group_id_len;
	NdisCopyMemory(updated_config->open_group_id,ezdev->ez_security.open_group_id,ezdev->ez_security.open_group_id_len);
	if (ez_is_triband_hook())
	{
		updated_config->non_ez_info[0].ssid_len = ez_ad->non_ez_band_info[0].ssid_len;
		updated_config->non_ez_info[1].ssid_len = ez_ad->non_ez_band_info[1].ssid_len;

		NdisCopyMemory(updated_config->non_ez_info[0].ssid, ez_ad->non_ez_band_info[0].ssid, ez_ad->non_ez_band_info[0].ssid_len);
		NdisCopyMemory(&updated_config->non_ez_info[0].triband_sec, &ez_ad->non_ez_band_info[0].triband_sec, sizeof(ez_ad->non_ez_band_info[0].triband_sec));

		NdisCopyMemory(updated_config->non_ez_info[1].ssid, ez_ad->non_ez_band_info[1].ssid, ez_ad->non_ez_band_info[1].ssid_len);
		NdisCopyMemory(&updated_config->non_ez_info[1].triband_sec, &ez_ad->non_ez_band_info[1].triband_sec, sizeof(ez_ad->non_ez_band_info[1].triband_sec));


		NdisZeroMemory(updated_config->non_ez_info[0].encrypted_pmk, sizeof(updated_config->non_ez_info[0].encrypted_pmk));
		NdisZeroMemory(updated_config->non_ez_info[1].encrypted_pmk, sizeof(updated_config->non_ez_info[1].encrypted_pmk));
		
		NdisCopyMemory(updated_config->non_ez_info[0].encrypted_pmk, ez_ad->non_ez_band_info[0].pmk, EZ_PMK_LEN);
		NdisCopyMemory(updated_config->non_ez_info[1].encrypted_pmk, ez_ad->non_ez_band_info[1].pmk, EZ_PMK_LEN);

		NdisZeroMemory(updated_config->non_ez_psk_info[0].encrypted_psk, sizeof(updated_config->non_ez_psk_info[0].encrypted_psk));
		NdisZeroMemory(updated_config->non_ez_psk_info[1].encrypted_psk, sizeof(updated_config->non_ez_psk_info[1].encrypted_psk));
	
		NdisCopyMemory(updated_config->non_ez_psk_info[0].encrypted_psk, ez_ad->non_ez_band_info[0].psk, LEN_PSK);
		NdisCopyMemory(updated_config->non_ez_psk_info[1].encrypted_psk, ez_ad->non_ez_band_info[1].psk, LEN_PSK);

	}else if(ez_ad->is_man_nonman) {
//! Levarage from MP1.0 CL#170037

		updated_config->non_man_info.ssid_len = ez_ad->non_man_info.ssid_len;
		
		NdisCopyMemory(updated_config->non_man_info.ssid, ez_ad->non_man_info.ssid, ez_ad->non_man_info.ssid_len);
		NdisCopyMemory(updated_config->non_man_info.authmode, ez_ad->non_man_info.authmode, strlen(ez_ad->non_man_info.authmode));
		NdisCopyMemory(updated_config->non_man_info.encryptype, ez_ad->non_man_info.encryptype, strlen(ez_ad->non_man_info.encryptype));

		NdisZeroMemory(updated_config->non_man_info.encrypted_psk, strlen(updated_config->non_man_info.encrypted_psk));
		NdisCopyMemory(updated_config->non_man_info.encrypted_psk, ez_ad->non_man_info.psk, LEN_PSK);
#ifdef DOT11R_FT_SUPPORT
		NdisCopyMemory(updated_config->non_man_info.FtMdId, ez_ad->non_man_info.FtMdId, FT_MDID_LEN);
#endif

	}

	if (!SSID_EQUAL(ezdev->ez_security.this_band_info.shared_info.ssid, ezdev->ez_security.this_band_info.shared_info.ssid_len,
		ap_ezdev->ez_security.this_band_info.shared_info.ssid, ap_ezdev->ez_security.this_band_info.shared_info.ssid_len))
	{
		updated_config->need_ez_update = TRUE;
	}

	if (!NdisEqualMemory(ezdev->ez_security.this_band_info.pmk, ap_ezdev->ez_security.this_band_info.pmk, EZ_PMK_LEN))
	{
		updated_config->need_ez_update = TRUE;
	}


}

#ifdef EZ_PUSH_BW_SUPPORT
void ez_chk_bw_config_different(ez_dev_t * ezdev, struct _ez_peer_security_info *ez_peer, BOOLEAN *pthis_band_changed, BOOLEAN *pOther_band_changed)
{
	interface_info_t *peer_this_band_info = NULL;
	interface_info_t *peer_other_band_info = NULL;
	interface_info_t *own_this_band_info = NULL;

	interface_info_t own_other_band_info;
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	struct _ez_security *ez_sec_info = &ezdev->ez_security;

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO,("ez_bw_config_different ----> \n"));

	*pthis_band_changed = FALSE;
	*pOther_band_changed = FALSE;

	if(!ez_sec_info || !ez_peer){
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("ez_bw_config_different: invalid params \n"));
	}

	peer_this_band_info = &ez_peer->this_band_info;
	peer_other_band_info = &ez_peer->this_band_info;

	own_this_band_info = &ez_sec_info->this_band_info;

	// check for interface_activated ??
	if( (own_this_band_info->shared_info.channel_info.channel != peer_this_band_info->shared_info.channel_info.channel) ||
		(ez_ad->push_bw_config && 
		  ((own_this_band_info->shared_info.channel_info.ht_bw != peer_this_band_info->shared_info.channel_info.ht_bw ) ||
		  (own_this_band_info->shared_info.channel_info.vht_bw != peer_this_band_info->shared_info.channel_info.vht_bw ))) ||
		(own_this_band_info->shared_info.channel_info.extcha != peer_this_band_info->shared_info.channel_info.extcha) )
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO,("ez_chk_bw_config_different: This Band Info different \n"));
		*pthis_band_changed = TRUE;
	}

	NdisZeroMemory(&own_other_band_info,sizeof(interface_info_t));
	// check for interface_activated ??
	if(ez_get_other_band_info(ezdev,&own_other_band_info)){

		if( (own_other_band_info.shared_info.channel_info.channel != peer_other_band_info->shared_info.channel_info.channel) ||
			( ez_ad->push_bw_config && 
			  ((own_other_band_info.shared_info.channel_info.ht_bw != peer_other_band_info->shared_info.channel_info.ht_bw ) ||
			  (own_other_band_info.shared_info.channel_info.vht_bw != peer_other_band_info->shared_info.channel_info.vht_bw ))) ||
			(own_other_band_info.shared_info.channel_info.extcha != peer_other_band_info->shared_info.channel_info.extcha) )
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO,("ez_chk_bw_config_different: Other band Info different \n"));
			*pOther_band_changed = TRUE;
		}
	}
	else{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("ez_chk_bw_config_different: Other band Info not obtained \n"));
	}

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("ez_bw_config_different: ThisBandChng:%x, OtherBandChng:%x \n", *pthis_band_changed, *pOther_band_changed));

}
#endif

enum_config_update_action_t push_and_update_config(EZ_ADAPTER *ez_ad , ez_dev_t *ezdev,
	struct _ez_peer_security_info *ez_peer, 
	BOOLEAN check_for_weight, 
	BOOLEAN from_port_secured,
	BOOLEAN surely_a_group_merge
	)
{
	ez_dev_t  *ap_ezdev;
	struct _ez_security *ez_sec_info;
	//UINT32 dbg_bkp = ez_ad->debug;
	unsigned int peer_groupid_len = ez_peer->group_id_len;
	unsigned char *peer_groupid = ez_peer->group_id;
	UINT32 peer_open_groupid_len = ez_peer->open_group_id_len;
	UINT8 *peer_open_groupid = ez_peer->open_group_id;
	int group_id_different = FALSE;
	BOOLEAN is_apcli  = (ezdev->ezdev_type == EZDEV_TYPE_APCLI);
	int i;
	enum_config_update_action_t config_update_action = ACTION_ADAPT;
	/*
		Update my apclient SSID/AuthMode/EncrypType setting
	*/
	ap_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev;

	ez_sec_info = &ezdev->ez_security;
	//ez_ad->debug = 3;
	//! we need not check for weight if, 
	//! function is called from port ecured when same interface AP was not in a configured state, or
	//! function is called while processing a group ID update action frame
	if (check_for_weight)
	{
//! Levarage from MP1.0 CL 170210
#ifdef CONFIG_PUSH_VER_SUPPORT	
		for (i = 0; i < (NETWORK_WEIGHT_LEN - 1); i++)
#else
		for (i = 0; i < NETWORK_WEIGHT_LEN; i++)
#endif
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%x:%x\t", ez_peer->device_info.network_weight[i], ez_ad->device_info.network_weight[i]));
//! Levarage from MP1.0 CL 170210
#ifdef CONFIG_PUSH_VER_SUPPORT
			if(i == 1) {
				if (ez_peer->device_info.network_weight[NETWORK_WEIGHT_LEN - 1] > ez_ad->device_info.network_weight[NETWORK_WEIGHT_LEN - 1]){

					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("\n ---> ACTION_ADAPT peer version is grater \n"));
					config_update_action = ACTION_ADAPT;
					break;
				} else if (ez_peer->device_info.network_weight[NETWORK_WEIGHT_LEN - 1] < ez_ad->device_info.network_weight[NETWORK_WEIGHT_LEN -1]){

					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("\n ---> ACTION_PUSH peer version is smaller \n"));
					config_update_action = ACTION_PUSH;
					break;
				} else {
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("\n ---> peer version is same \n"));
				}		
			}
#endif

			if (ez_peer->device_info.network_weight[i] > ez_ad->device_info.network_weight[i])
			{
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("---> ACTION_ADAPT\n"));
				config_update_action = ACTION_ADAPT;
				break;
			} else if (ez_peer->device_info.network_weight[i] < ez_ad->device_info.network_weight[i]){
				config_update_action = ACTION_PUSH;
				break;
			} else if (ez_peer->device_info.network_weight[i] == ez_ad->device_info.network_weight[i]){
				config_update_action = ACTION_NOTHING;
			}
			}
		}
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("\n"));
	if (config_update_action == ACTION_NOTHING)
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Assert!!! ACTION_NOTHING\n"));

	}
	//! group ID comparision
	if (((ezdev->ez_security.group_id_len != peer_groupid_len) ||
		!NdisEqualMemory(ezdev->ez_security.group_id, peer_groupid, peer_groupid_len)) || ((ezdev->ez_security.open_group_id_len != peer_open_groupid_len) ||
		!NdisEqualMemory(ezdev->ez_security.open_group_id, peer_open_groupid, peer_open_groupid_len)))
	{
		group_id_different = TRUE;
		ez_hex_dump("ezdev->ez_security.group_id",ezdev->ez_security.group_id,ezdev->ez_security.group_id_len);
		ez_hex_dump("peer_groupid", peer_groupid, peer_groupid_len);
		//! group ID is different, yet we need not update configuration if:
		//! function is called from port secured of AP interface, or
		//! function is called from action frame processing but group ID update bit is not set
		if ((from_port_secured && !is_apcli) || (!from_port_secured && !surely_a_group_merge))
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("---> ACTION_NOTHING\n"));
			config_update_action = ACTION_NOTHING;
		} else if ((from_port_secured && is_apcli) || (!from_port_secured && surely_a_group_merge)) {
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("---> ACTION_ADAPT\n"));
			config_update_action = ACTION_ADAPT;
		} else {
#ifdef DBG		
			ASSERT(FALSE);
#endif
		}
	}

	//! if we need to update the configuration	
	if (config_update_action == ACTION_ADAPT)
	{
//! Levarage from MP1.0 CL 170192
		updated_configs_t *updated_configs = NULL;
		NDIS_STATUS NStatus;
		BOOLEAN action_sent = FALSE;
#ifdef EZ_PUSH_BW_SUPPORT
		BOOLEAN this_band_changed = FALSE;
		BOOLEAN other_band_changed = FALSE;

		//if( ((PRTMP_ADAPTER)(ezdev->ez_ad))->push_bw_config )
			ez_chk_bw_config_different(ezdev,ez_peer,&this_band_changed,&other_band_changed);
#endif
//! Levarage from MP1.0 CL 170192
		NStatus = EZ_MEM_ALLOC(NULL, (UCHAR **)&updated_configs, sizeof(updated_configs_t));
        	if(NStatus != NDIS_STATUS_SUCCESS)
        	{
                	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%s() allocate memory failed \n", __FUNCTION__));
			ASSERT(FALSE);
        	}
		if(is_apcli){
			// no need to restore channel in case of AP as it is already on the same channel.
			ap_ezdev->driver_ops->ez_restore_channel_config(ap_ezdev);
		// 08/2016 : Rakesh: when channel switch happens on A band, currently many events are seen which delay other Rx
			// causing Link loss. Workaround to delay linkloss.
			ezdev->ez_security.delay_disconnect_count = ez_ad->ez_delay_disconnect_count;
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Delay linkloss detection during config adapt, delay_disconnect_count=%d\n", 
																	ez_ad->ez_delay_disconnect_count));

		}
		
		//! copy configurations to be adapted from the ez_peer and send update configuration to connected peers
		NdisZeroMemory(updated_configs, sizeof(updated_configs_t));
		ez_init_updated_configs_for_adapt(updated_configs, ez_peer, ezdev);
		
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("updated_configs->ssid --- > %s\n", updated_configs->this_band_info.shared_info.ssid));
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("updated_configs->other_band ssid --- > %s\n", updated_configs->other_band_info.shared_info.ssid));
		
		if ( push_and_update_ap_config(ez_ad, ezdev, updated_configs, group_id_different))
		{
			action_sent = TRUE;
		}
		if (push_and_update_cli_config(ez_ad, ezdev, updated_configs, group_id_different))
		{
			action_sent = TRUE;
		}
		//! run loop for all CLI interfaces
		if (action_sent == FALSE)
		{
			
			//ez_update_connection_permission(ez_ad,NULL,EZ_ALLOW_ALL);
			ez_update_connection_permission_hook(ezdev,EZ_ALLOW_ALL);
		}

		//! switch back AP channel to target channel

		ap_ezdev->driver_ops->ez_restore_channel_config(ap_ezdev);
#if (defined(DOT11_N_SUPPORT) && defined(DOT11N_DRAFT3))
		if(ap_ezdev->ez_security.ap_did_fallback){
			if(ap_ezdev->ez_security.fallback_channel != ap_ezdev->ez_security.this_band_info.shared_info.channel_info.channel){
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("\npush_and_update_config:Last channel(%d) and current channel(%d) different, reset fallback context\n",
					ap_ezdev->ez_security.fallback_channel, ap_ezdev->ez_security.this_band_info.shared_info.channel_info.channel));
				ez_set_ap_fallback_context(ap_ezdev,FALSE,0);
			}
		}
#endif

		ap_ezdev->driver_ops->UpdateBeaconHandler(ap_ezdev, IE_CHANGE);

#ifdef EZ_PUSH_BW_SUPPORT
		//update peer info to new config
		//Rakesh: Optimization can be added of using from_port_secured to avoid updating for connected AP
		if(ez_ad->push_bw_config )
			update_peer_record(ez_ad, ezdev, this_band_changed, other_band_changed);
#endif
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Initiate New scan after adapt\n"));
		ez_initiate_new_scan(ez_ad);
//! Levarage from MP1.0 CL 170192
		EZ_MEM_FREE(updated_configs);

	}

	//ez_ad->debug = dbg_bkp;
	return config_update_action;
	
}

 char * ez_CheckAuthMode(UINT32 _AKMMap)
{

	if(IS_AKM_OPEN(_AKMMap))
		return "OPEN";
 	else if(IS_AKM_SHARED(_AKMMap))
		return "SHARED";
	else if(IS_AKM_AUTOSWITCH(_AKMMap))
		return "WEPAUTO";
	else if(IS_AKM_WPA1(_AKMMap)) 
		return "WPA";
  	else if(IS_AKM_WPA1PSK(_AKMMap))
		return "WPAPSK";
 	else if(IS_AKM_WPANONE(_AKMMap))  
		return "WPANONE";
 	else if(IS_AKM_WPA2(_AKMMap))
		return "WPA2";
	else if(IS_AKM_WPA2PSK(_AKMMap))
		return "WPA2PSK";

	return NULL ;
}

 char * ez_CheckEncrypType(UINT32 Cipher)
{
	if (IS_CIPHER_NONE(Cipher))
		return "NONE";
	else if (IS_CIPHER_WEP(Cipher))
		return "WEP";
	else if (IS_CIPHER_TKIP(Cipher))
		return "TKIP";
	else if (IS_CIPHER_CCMP128(Cipher))
		return "AES";
	else if (IS_CIPHER_CCMP256(Cipher))
		return "CCMP256";
	else if (IS_CIPHER_GCMP128(Cipher))
		return "GCMP128";
	else if (IS_CIPHER_GCMP256(Cipher))
		return "GCMP256";
	else if (IS_CIPHER_TKIP(Cipher)||IS_CIPHER_CCMP128(Cipher))
		return "TKIPAES";
	else if (IS_CIPHER_TKIP(Cipher)|| IS_CIPHER_CCMP128(Cipher))
		return "WPA_AES_WPA2_TKIPAES";
#ifdef WAPI_SUPPORT
	else if (IS_CIPHER_WPI_SMS4(Cipher))
		return "SMS4";
	
#endif /* WAPI_SUPPORT */
	else
	{
		return NULL;
	}

}
#if 0
#ifdef EZ_API_SUPPORT
BOOLEAN ez_port_secured_for_connection_offload(
	EZ_ADAPTER *ez_ad,
	void *entry_obj,
	unsigned char if_idx,
	unsigned char ap_mode)
{
	MAC_TABLE_ENTRY *entry;
	ez_dev_t *ezdev;
	struct _ez_security *ez_sec_info;
	APCLI_STRUCT *apcli_entry;
	unsigned char ori_apcli_enable;
	struct _ez_peer_security_info *ez_peer;
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("------> %s()\n", __FUNCTION__));
	ad = (RTMP_ADAPTER *)ez_ad;
	entry = (MAC_TABLE_ENTRY *)entry_obj;
	ezdev = entry->ezdev;
	ez_sec_info = &ezdev->ez_security;
	entry->easy_setup_enabled = TRUE;
	apcli_entry = &ad->ApCfg.ApCliTab[if_idx];
	ori_apcli_enable = apcli_entry->Enable;
	apcli_entry->Enable = FALSE;
#if defined (NEW_CONNECTION_ALGO) || defined (EZ_NETWORK_MERGE_SUPPORT)
		ez_peer = ez_peer_table_search_by_addr_hook(ezdev, entry->Addr);
		ez_peer->port_secured = TRUE;
		
#endif

	if (ap_mode) {
		entry->is_apcli = TRUE;
#ifdef EZ_NETWORK_MERGE_SUPPORT
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("connected peer capabilities --->%x\n", ez_peer->capability));
		if (EZ_GET_CAP_ALLOW_MERGE(ez_peer->capability)) {
			NdisZeroMemory(ez_sec_info->merge_peer_addr, MAC_ADDR_LEN);
			EZ_CLEAR_CAP_ALLOW_MERGE(ez_sec_info->capability);
			EZ_UPDATE_CAPABILITY_INFO(ad, EZ_CLEAR_ACTION, ALLOW_MERGE, if_idx);
			if (ez_sec_info->ez_group_merge_timer_running)
			{
				ezdev->driver_ops->ez_cancel_timer(ezdev, &ez_sec_info->ez_group_merge_timer, 
							ez_sec_info->ez_group_merge_timer_running);

			}
		}
#endif		
		ez_install_ptk(ad, entry, TRUE);
	} else {
		/* Install Key */
		ez_install_ptk(ad, entry, FALSE);
		ez_apcli_install_gtk((RTMP_ADAPTER *)ezdev->ez_security.ad, entry);
		entry->SecConfig.Handshake.WpaState = AS_PTKINITDONE;
		
		//! clear off group merge related params
		NdisZeroMemory(ez_sec_info->merge_peer_addr, MAC_ADDR_LEN);
		EZ_CLEAR_CAP_ALLOW_MERGE(ez_sec_info->capability);
		EZ_UPDATE_APCLI_CAPABILITY_INFO(ad, EZ_CLEAR_ACTION, ALLOW_MERGE, if_idx);
		if (ez_sec_info->ez_group_merge_timer_running)
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Group Merge completed, cancel timer\n"));
			ezdev->driver_ops->ez_cancel_timer(ezdev, &ez_sec_info->ez_group_merge_timer, 
						ez_sec_info->ez_group_merge_timer_running);

		}

	}

	if (ap_mode) {
#ifdef SYSTEM_LOG_SUPPORT
		RTMPSendWirelessEvent(ad, IW_WH_EZ_MY_AP_HAS_APCLI, entry->Addr, ezdev->ez_band_idx, 0);
#else /* SYSTEM_LOG_SUPPORT */
		RtmpOSWrielessEventSend(ezdev->if_dev, RT_WLAN_EVENT_CUSTOM, IW_WH_EZ_MY_AP_HAS_APCLI,
						    NULL, NULL, 0);
#endif /* !SYSTEM_LOG_SUPPORT */
	}
	else {
#ifdef SYSTEM_LOG_SUPPORT
		RTMPSendWirelessEvent(ad, IW_WH_EZ_MY_APCLI_CONNECTED, NULL, ezdev->ez_band_idx, 0);
#else /* SYSTEM_LOG_SUPPORT */
		RtmpOSWrielessEventSend(ezdev->if_dev, RT_WLAN_EVENT_CUSTOM, IW_WH_EZ_MY_APCLI_CONNECTED,
						    NULL, (PUCHAR)ez_peer, sizeof(struct _ez_peer_security_info));
#endif /* !SYSTEM_LOG_SUPPORT */
	}
	
		apcli_entry->Enable = ori_apcli_enable;
	
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
				("<------ %s()\n", __FUNCTION__));
	return TRUE;
}
#endif
#endif
#ifdef EZ_NETWORK_MERGE_SUPPORT
BOOLEAN ez_get_other_band_info(ez_dev_t * ezdev, void *other_band_config)
{
	struct _ez_security* other_band_sec_info;
	ez_dev_t *other_band_ezdev = ez_get_otherband_ezdev(ezdev);
			
	if (other_band_ezdev != NULL) {
		other_band_sec_info =  &other_band_ezdev->ez_security;
	} else {
		return FALSE;
	}

	if (other_band_sec_info->this_band_info.interface_activated) {
		NdisCopyMemory(other_band_config, &other_band_sec_info->this_band_info, sizeof(interface_info_t));
		return TRUE;
	} else {
		return FALSE;
	}
	return FALSE;		
}
struct _ez_peer_security_info *ez_get_other_band_ez_peer(ez_dev_t * ezdev, struct _ez_peer_security_info *ez_peer)
{
	//PRTMP_ADAPTER ad = ezdev->ez_ad;
	unsigned char *mac_addr;
	//PRTMP_ADAPTER adOthBand = ez_ad->ez_band_info[ezdev->ez_band_idx ^ 1].ez_ad;

	ez_dev_t * other_band_ezdev = ez_get_otherband_ezdev(ezdev);
	if (ezdev->ezdev_type == EZDEV_TYPE_AP){
		mac_addr = ez_peer->other_band_info.shared_info.cli_mac_addr;
	} else {
		mac_addr = ez_peer->other_band_info.shared_info.ap_mac_addr;
	}
	if (other_band_ezdev) {
		return ez_peer_table_search_by_addr_hook(other_band_ezdev, mac_addr); 
	} else {
		return NULL;
	}
	return NULL;
}

void ez_inform_all_interfaces(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev, inform_other_band_action_t action)
{
	//struct _ez_security* security_info;
	ez_dev_t  *target_ezdev;
	switch (action)
	{
		case ACTION_UPDATE_DUPLICATE_LINK_ENTRY: // since only ezdev of other CLI interface updated here, separate funtion to update other band cli can be used instead
			if((ezdev->ezdev_type == EZDEV_TYPE_APCLI)){
				ez_dev_t  *other_band_cli_ezdev = ez_get_otherband_cli_ezdev(ezdev);
				if (other_band_cli_ezdev) {
				other_band_cli_ezdev->ez_security.this_band_info.shared_info.link_duplicate = ezdev->ez_security.this_band_info.shared_info.link_duplicate;
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("ez_band_idx=0x%x, ezdev_type=0x%x, : Change Duplicate Link status to: %x\n",
					other_band_cli_ezdev->ez_band_idx,other_band_cli_ezdev->ezdev_type,other_band_cli_ezdev->ez_security.this_band_info.shared_info.link_duplicate));
				}
			}
			break;
			
		case ACTION_UPDATE_INTERNET_STATUS:
		{
			ez_dev_t  *other_band_ap_ezdev = ez_get_otherband_ap_ezdev(ezdev);
										
			if (other_band_ap_ezdev) {	
				if(ezdev->ez_security.go_internet)
				{
					EZ_UPDATE_CAPABILITY_INFO(other_band_ap_ezdev, EZ_SET_ACTION, INTERNET);
				}
				else
				{
					EZ_UPDATE_CAPABILITY_INFO(other_band_ap_ezdev, EZ_CLEAR_ACTION, INTERNET);
				}
			}				
			break;
		}
		case ACTION_UPDATE_DEVICE_INFO:
			break;
		case ACTION_UPDATE_CONFIG_STATUS:
			if (ez_ad->configured_status == EZ_UNCONFIGURED) {
				int i =0;
				ez_ad->configured_status = EZ_CONFIGURED;
				for (i = 0; i< MAX_EZ_BANDS; i++)
				{
					target_ezdev = &ez_ad->ez_band_info[i].ap_ezdev;
					
					if (target_ezdev == NULL)
					{
						continue;
					}
					
					target_ezdev->driver_ops->ez_update_security_setting
						(target_ezdev, target_ezdev->ez_security.this_band_info.pmk);
					target_ezdev->driver_ops->ez_update_ap_wsc_profile
						(target_ezdev);

					if (target_ezdev->ezdev_type == EZDEV_TYPE_AP)
					{
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Move AP into configured state\n"));
						EZ_UPDATE_CAPABILITY_INFO(target_ezdev, EZ_SET_ACTION, CONFIGRED);
					}

					EZ_SET_CAP_CONFIGRED(target_ezdev->ez_security.capability);
					
					target_ezdev->driver_ops->UpdateBeaconHandler
						(target_ezdev, IE_CHANGE);
					
					target_ezdev = &ez_ad->ez_band_info[i].cli_ezdev;
					
					if (target_ezdev == NULL)
					{
						continue;
					}
					target_ezdev->driver_ops->ez_update_security_setting
						(target_ezdev, target_ezdev->ez_security.this_band_info.pmk);
					target_ezdev->driver_ops->ez_update_ap_wsc_profile
						(target_ezdev);
					{
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Move CLI into configured state\n"));
						EZ_UPDATE_APCLI_CAPABILITY_INFO(ez_ad, EZ_SET_ACTION, CONFIGRED, target_ezdev->ez_band_idx);
					}
					EZ_SET_CAP_CONFIGRED(target_ezdev->ez_security.capability);

				}
			}
			break;
			
		default:
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("INVALID ACTION = 0x%x\n", action));
			break;
	}
}

 BOOLEAN ez_ap_basic_config_changed(ez_dev_t * ezdev, updated_configs_t *updated_configs)
{
	EZ_ADAPTER * ez_ad = ezdev->ez_ad;
	interface_info_t *curr_band_info = NULL;
	interface_info_t *update_band_info = NULL;

	if(!ezdev || !updated_configs){
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("ez_ap_basic_config_changed: Invalid params\n"));
	}

	curr_band_info = &ezdev->ez_security.this_band_info;
	update_band_info = &updated_configs->this_band_info;

	if(curr_band_info->shared_info.ssid_len != update_band_info->shared_info.ssid_len)
		return TRUE;

	if( NdisCmpMemory(curr_band_info->shared_info.ssid,
		    update_band_info->shared_info.ssid,
		    update_band_info->shared_info.ssid_len) != 0 ){
		return TRUE;
	}

	if( (curr_band_info->shared_info.channel_info.channel != update_band_info->shared_info.channel_info.channel) ||
#ifdef EZ_PUSH_BW_SUPPORT
		(ez_ad->push_bw_config && 
		  ((curr_band_info->shared_info.channel_info.ht_bw != update_band_info->shared_info.channel_info.ht_bw ) ||
		  (curr_band_info->shared_info.channel_info.vht_bw != update_band_info->shared_info.channel_info.vht_bw ))) ||
#endif
		(curr_band_info->shared_info.channel_info.extcha != update_band_info->shared_info.channel_info.extcha) )
	{
		return TRUE;
	}

	if( NdisCmpMemory(curr_band_info->pmk,
			update_band_info->pmk,
			EZ_PMK_LEN) != 0 ){
		return TRUE;
	}
	
	if (NdisCmpMemory(curr_band_info->psk,
		update_band_info->psk,
		EZ_LEN_PSK) != 0 ){
		return TRUE;
	}
#ifdef DOT11R_FT_SUPPORT
	if(ezdev->FtCfg.FtCapFlag.Dot11rFtEnable && ( NdisCmpMemory(curr_band_info->shared_info.FtMdId,
			update_band_info->shared_info.FtMdId,
			FT_MDID_LEN) != 0 ) )
	{
		return TRUE;
	}
#endif

	return FALSE;
}

static BOOLEAN ez_update_ap(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev, updated_configs_t *updated_configs, BOOLEAN group_id_diff, BOOLEAN band_switched)
{
	BOOLEAN ret_value =FALSE;
	BOOLEAN deauth_non_ez_sta = TRUE;
	int flags;	
	//PRTMP_ADAPTER adOthBand = ez_adapter.ez_band_info[wdev->ez_security.ez_band_idx ^ 1].pAd;

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s >>>\n", __FUNCTION__));

	if (ezdev->wdev == NULL)
	{
		return FALSE;
	}
	
	if (updated_configs->this_band_info.interface_activated == FALSE)
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("do not update as no configs provided\n"));
		return FALSE;
	}
	//if (IS_SINGLE_CHIP_DBDC(ad))
	{

		//NdisCopyMemory(&ez_adapter.device_info, &updated_configs->device_info, sizeof(device_info_t));
		ez_adapter->device_info.network_weight[0] &= ~(BIT(7));
		ez_adapter->device_info.network_weight[0] &= ~(BIT(6));
		ez_adapter->device_info.network_weight[0] &= ~(BIT(5));

		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s, wdev_idx:%x wdev->type: %x weight_defining_link, mac = %02x:%02x:%02x:%02x:%02x:%02x\n", 
		//	__FUNCTION__,ezdev->wdev_idx, ezdev->wdev_type, PRINT_MAC(updated_configs->mac_addr)));

		deauth_non_ez_sta = ez_ap_basic_config_changed(ezdev,updated_configs);

		ezdev->ez_security.this_band_info.shared_info.ssid_len = updated_configs->this_band_info.shared_info.ssid_len;
		NdisZeroMemory(ezdev->ez_security.this_band_info.shared_info.ssid,MAX_LEN_OF_SSID);
		NdisCopyMemory(ezdev->ez_security.this_band_info.shared_info.ssid, updated_configs->this_band_info.shared_info.ssid, updated_configs->this_band_info.shared_info.ssid_len);

		ezdev->ez_security.this_band_info.shared_info.channel_info.channel = updated_configs->this_band_info.shared_info.channel_info.channel;
		ezdev->ez_security.this_band_info.shared_info.channel_info.extcha = updated_configs->this_band_info.shared_info.channel_info.extcha;
#ifdef EZ_PUSH_BW_SUPPORT
		if( ez_adapter->push_bw_config ){
			if(updated_configs->this_band_info.shared_info.channel_info.ht_bw != 0xFF)
				ezdev->ez_security.this_band_info.shared_info.channel_info.ht_bw = updated_configs->this_band_info.shared_info.channel_info.ht_bw;
			if(updated_configs->this_band_info.shared_info.channel_info.vht_bw != 0xFF)
				ezdev->ez_security.this_band_info.shared_info.channel_info.vht_bw = updated_configs->this_band_info.shared_info.channel_info.vht_bw;
		}
#endif

		NdisCopyMemory(ezdev->ez_security.this_band_info.pmk, updated_configs->this_band_info.pmk, LEN_PMK);
		NdisZeroMemory(ezdev->ez_security.this_band_info.psk, EZ_LEN_PSK);
		NdisCopyMemory(ezdev->ez_security.this_band_info.psk, updated_configs->this_band_info.psk, strlen(updated_configs->this_band_info.psk));
#ifdef DOT11R_FT_SUPPORT
		FT_SET_MDID(ezdev->ez_security.this_band_info.shared_info.FtMdId, updated_configs->this_band_info.shared_info.FtMdId);
#endif

		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s, weight_defining_link, new channel will be %d\n", __FUNCTION__,updated_configs->this_band_info.shared_info.channel_info.channel));
#ifdef DUAL_CHIP
		if(((!IS_SINGLE_CHIP_DBDC((EZ_ADAPTER *)ez_ad)) && TRUE) && 
			band_switched)
			EZ_IRQ_LOCK(&ez_ad->ez_set_peer_lock, flags);
#endif
		ret_value = ez_send_action_update_config_for_this_band(ez_ad, ezdev,updated_configs, group_id_diff, band_switched, deauth_non_ez_sta);
#ifdef DUAL_CHIP
		if(((!IS_SINGLE_CHIP_DBDC((EZ_ADAPTER *)ez_ad)) && TRUE) && 
			band_switched)
			EZ_IRQ_UNLOCK(&ez_ad->ez_set_peer_lock, flags);
#endif

		//! if group ID is different, update group ID
		if (group_id_diff)
		{
			if (updated_configs->group_id == NULL)
			{
				ASSERT(FALSE);
			}
			EZ_MEM_FREE(ezdev->ez_security.group_id);
			EZ_MEM_ALLOC(NULL,&ezdev->ez_security.group_id,updated_configs->group_id_len);
			NdisCopyMemory(ezdev->ez_security.group_id,updated_configs->group_id,updated_configs->group_id_len);
			ezdev->ez_security.open_group_id_len = updated_configs->open_group_id_len;
			NdisCopyMemory(ezdev->ez_security.open_group_id,updated_configs->open_group_id,updated_configs->open_group_id_len);
			
			ezdev->ez_security.group_id_len = updated_configs->group_id_len;
		}
		//! update my own parameters
		{

			ezdev->driver_ops->ez_update_ap(ezdev, updated_configs);

			ezdev->driver_ops->ez_update_security_setting
					(ezdev, ezdev->ez_security.this_band_info.pmk);
			ezdev->driver_ops->ez_update_ap_wsc_profile
					(ezdev);
				
			//ez_update_security_setting(ad, wdev, updated_configs->this_band_info.pmk);
			//ez_update_ap_wsc_profile(ad, wdev, wdev->func_idx);

#ifdef DOT11R_FT_SUPPORT
			FT_SET_MDID(ezdev->FtCfg.FtMdId, updated_configs->this_band_info.shared_info.FtMdId);
			//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("%s::FtCfg FtMdid =%c%c\n", __FUNCTION__,
			//							wdev->FtCfg.FtMdId[0],
			//							wdev->FtCfg.FtMdId[1]));
#endif

#ifdef EZ_PUSH_BW_SUPPORT
			if(ez_adapter->push_bw_config )
			{
				if(updated_configs->this_band_info.shared_info.channel_info.ht_bw != 0xFF)
					ezdev->driver_ops->wlan_config_set_ht_bw(ezdev, updated_configs->this_band_info.shared_info.channel_info.ht_bw);
				if(updated_configs->this_band_info.shared_info.channel_info.vht_bw != 0xFF)
					ezdev->driver_ops->wlan_config_set_vht_bw(ezdev, updated_configs->this_band_info.shared_info.channel_info.vht_bw);
			}
#endif
			ezdev->driver_ops->wlan_config_set_ext_cha(ezdev, updated_configs->this_band_info.shared_info.channel_info.extcha);

#if (defined(DOT11_N_SUPPORT) && defined(DOT11N_DRAFT3))
			if(ezdev->ez_security.ap_did_fallback){
				if(ezdev->ez_security.fallback_channel == updated_configs->this_band_info.shared_info.channel_info.channel){
					EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("\nez_update_ap:Restoring ap to fallback mode\n"));
					ezdev->driver_ops->wlan_config_set_ht_bw(ezdev,HT_BW_20);
					ezdev->driver_ops->wlan_config_set_ext_cha(ezdev,EXTCHA_NONE);
				}
			}
#endif

			ezdev->driver_ops->SetCommonHtVht(ezdev);
			ezdev->driver_ops->UpdateBeaconHandler(ezdev, IE_CHANGE);
 			
 		}
		
	}
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s <<<\n", __FUNCTION__));

	return ret_value;
}



static BOOLEAN ez_update_cli(EZ_ADAPTER *ez_ad, ez_dev_t *ezdev, updated_configs_t *updated_configs, BOOLEAN group_id_diff, BOOLEAN band_switched)
{
	BOOLEAN ret_value = FALSE;
	int flags;
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s >>>\n", __FUNCTION__));
	
	//if (IS_SINGLE_CHIP_DBDC(ad))
//	{

	if (ezdev->wdev == NULL)
	{
		return FALSE;
	}
	
		if (updated_configs->this_band_info.interface_activated == FALSE)
		{
			
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("do not update as no configs provided\n"));
			return FALSE;
		}
		ezdev->ez_security.this_band_info.shared_info.ssid_len = updated_configs->this_band_info.shared_info.ssid_len;
		NdisZeroMemory(ezdev->ez_security.this_band_info.shared_info.ssid,MAX_LEN_OF_SSID);
		NdisCopyMemory(ezdev->ez_security.this_band_info.shared_info.ssid, updated_configs->this_band_info.shared_info.ssid, updated_configs->this_band_info.shared_info.ssid_len);

		ezdev->ez_security.this_band_info.shared_info.channel_info.channel = updated_configs->this_band_info.shared_info.channel_info.channel;
		ezdev->ez_security.this_band_info.shared_info.channel_info.extcha = updated_configs->this_band_info.shared_info.channel_info.extcha;
#ifdef EZ_PUSH_BW_SUPPORT
		if(ez_adapter->push_bw_config ){
			if(updated_configs->this_band_info.shared_info.channel_info.ht_bw != 0xFF)
				ezdev->ez_security.this_band_info.shared_info.channel_info.ht_bw = updated_configs->this_band_info.shared_info.channel_info.ht_bw;
			if(updated_configs->this_band_info.shared_info.channel_info.vht_bw != 0xFF)
				ezdev->ez_security.this_band_info.shared_info.channel_info.vht_bw = updated_configs->this_band_info.shared_info.channel_info.vht_bw;
		}
#endif

		NdisCopyMemory(ezdev->ez_security.this_band_info.pmk, updated_configs->this_band_info.pmk, LEN_PMK);
		NdisZeroMemory(ezdev->ez_security.this_band_info.psk, EZ_LEN_PSK);
		NdisCopyMemory(ezdev->ez_security.this_band_info.psk, updated_configs->this_band_info.psk, strlen(updated_configs->this_band_info.psk));

#ifdef DOT11R_FT_SUPPORT
		FT_SET_MDID(ezdev->ez_security.this_band_info.shared_info.FtMdId, updated_configs->this_band_info.shared_info.FtMdId);
#endif

		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s, weight_defining_link, new channel will be %d\n", __FUNCTION__,updated_configs->this_band_info.shared_info.channel_info.channel));

		//ret_value = ez_send_action_update_config_for_this_band(ad_obj, wdev,updated_configs, group_id_diff, band_switched);
		//! if group ID is different, update group ID
		if (group_id_diff)
		{
			if (updated_configs->group_id == NULL)
			{
				ASSERT(FALSE);
			}
			EZ_MEM_FREE(ezdev->ez_security.group_id);
			EZ_MEM_ALLOC(NULL,&ezdev->ez_security.group_id,updated_configs->group_id_len);
			NdisCopyMemory(ezdev->ez_security.group_id,updated_configs->group_id,updated_configs->group_id_len);
			ezdev->ez_security.open_group_id_len = updated_configs->open_group_id_len;
			NdisCopyMemory(ezdev->ez_security.open_group_id,updated_configs->open_group_id,updated_configs->open_group_id_len);
			
			ezdev->ez_security.group_id_len = updated_configs->group_id_len;
		}
		//! update my own parameters
		{

			ezdev->driver_ops->ez_update_cli(ezdev, updated_configs);
			//NdisZeroMemory(apcli_entry->Ssid,MAX_LEN_OF_SSID);
		//	apcli_entry->SsidLen = updated_configs->this_band_info.shared_info.ssid_len ;
		//	NdisCopyMemory(apcli_entry->Ssid, updated_configs->this_band_info.shared_info.ssid,updated_configs->this_band_info.shared_info.ssid_len );
		
		//	NdisZeroMemory(apcli_entry->CfgSsid,MAX_LEN_OF_SSID);
		//	apcli_entry->CfgSsidLen = updated_configs->this_band_info.shared_info.ssid_len ;
		//	NdisCopyMemory(apcli_entry->CfgSsid, updated_configs->this_band_info.shared_info.ssid,updated_configs->this_band_info.shared_info.ssid_len );
#if 0
			if (ez_adapter.band_count == 1 && ez_adapter.non_ez_band_count == 2)
			{
				apcli_entry->SsidLen = sizeof("BAKHAUL_AP");
				os_zero_mem(apcli_entry->Ssid, MAX_LEN_OF_SSID);
				NdisCopyMemory(apcli_entry->Ssid, "BAKHAUL_AP", sizeof("BAKHAUL_AP"));
				
				
				apcli_entry->CfgSsidLen = sizeof("BAKHAUL_AP");
				os_zero_mem(apcli_entry->CfgSsid, MAX_LEN_OF_SSID);
				NdisCopyMemory(apcli_entry->CfgSsid, "BAKHAUL_AP", sizeof("BAKHAUL_AP"));
			}
#endif			
			//NdisCopyMemory(&wdev->SecConfig.PMK[0], updated_configs->this_band_info.pmk, LEN_PMK);

			
			//if (ez_adapter.configured_status == EZ_UNCONFIGURED)
			{
				//ez_adapter.configured_status = EZ_CONFIGURED;
				//ez_update_security_setting(ad, wdev, updated_configs->this_band_info.pmk);
				//ez_update_ap_wsc_profile(ad, wdev, wdev->func_idx);
				ezdev->driver_ops->ez_update_security_setting
					(ezdev, ezdev->ez_security.this_band_info.pmk);
				ezdev->driver_ops->ez_update_ap_wsc_profile
						(ezdev);
				//EZ_SET_CAP_CONFIGRED(wdev->ez_security.capability);
			}
#if 0			
			if (!ez_is_triband_hook()){			

				CLEAR_SEC_AKM(wdev->SecConfig.AKMMap);
				SET_AKM_WPA2PSK(wdev->SecConfig.AKMMap);
				SET_CIPHER_CCMP128(wdev->SecConfig.PairwiseCipher);
				SET_CIPHER_CCMP128(wdev->SecConfig.GroupCipher);

			}
#endif
			
#ifdef EZ_PUSH_BW_SUPPORT
			if( ez_adapter->push_bw_config )
			{
				if(updated_configs->this_band_info.shared_info.channel_info.ht_bw != 0xFF)
					ezdev->driver_ops->wlan_config_set_ht_bw(ezdev, updated_configs->this_band_info.shared_info.channel_info.ht_bw);
				if(updated_configs->this_band_info.shared_info.channel_info.vht_bw != 0xFF)
					ezdev->driver_ops->wlan_config_set_vht_bw(ezdev, updated_configs->this_band_info.shared_info.channel_info.vht_bw);
			}
#endif
			ezdev->driver_ops->wlan_config_set_ext_cha(ezdev, updated_configs->this_band_info.shared_info.channel_info.extcha);

			ezdev->driver_ops->SetCommonHtVht(ezdev);
		}
				

#ifdef DUAL_CHIP
		if(((!IS_SINGLE_CHIP_DBDC((EZ_ADAPTER *)ez_ad)) && TRUE) && 
			band_switched) 
			EZ_IRQ_LOCK(&ez_ad->ez_set_peer_lock, flags);
#endif
		//! if action is from AP_CLI and same band, no need to send a de-auth and action frame
		ez_hex_dump("this_band cli_peer_ap_mac",ezdev->ez_security.this_band_info.cli_peer_ap_mac, 6);
		if (!MAC_ADDR_EQUAL(ezdev->ez_security.this_band_info.cli_peer_ap_mac, updated_configs->mac_addr) 
			&& !MAC_ADDR_EQUAL(ezdev->ez_security.this_band_info.cli_peer_ap_mac, ZERO_MAC_ADDR))
		{
			if(ezdev->ez_security.this_band_info.shared_info.link_duplicate && band_switched)
			{
				struct _ez_peer_security_info *ez_peer	= ez_peer_table_search_by_addr_hook(ezdev, ezdev->ez_security.this_band_info.cli_peer_ap_mac);
				if (ez_peer){
					struct _ez_peer_security_info *ez_other_band_peer = ez_get_other_band_ez_peer(ezdev,ez_peer);
					if(ez_other_band_peer == NULL)
					{
						ASSERT(FALSE);
					}
					NdisCopyMemory(&ez_peer->device_info.ez_node_number,&ez_other_band_peer->device_info.ez_node_number, sizeof(EZ_NODE_NUMBER));
				}
			} else {
				struct _ez_peer_security_info *ez_peer = NULL;
				ez_peer  = ez_peer_table_search_by_addr_hook(ezdev, ezdev->ez_security.this_band_info.cli_peer_ap_mac);
				if ((ez_peer) && (ez_peer->port_secured)){
					if (send_action_update_config(ez_ad, ez_peer, ezdev, updated_configs, TRUE, group_id_diff)== TRUE)
					ret_value = TRUE;
				}
			}
		}
#ifdef DUAL_CHIP
		if(((!IS_SINGLE_CHIP_DBDC((EZ_ADAPTER *)ez_ad)) && TRUE) && 
			band_switched)
			EZ_IRQ_UNLOCK(&ez_ad->ez_set_peer_lock, flags);
#endif

		
//	} else {
//	}
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s <<<\n", __FUNCTION__));
	return ret_value;
}


BOOLEAN ez_update_other_band_ap(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev, updated_configs_t *updated_configs, BOOLEAN group_id_diff)
{
	//EZ_ADAPTER *other_band_ez_ad;
	ez_dev_t  *other_band_ezdev = NULL; 
	BOOLEAN action_frame_sent = FALSE;
//! Levarage from MP1.0 CL 170192
	updated_configs_t *updated_configs_local = NULL;
	NDIS_STATUS NStatus;
	ez_dev_t * ap_ezdev =  &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev;
	ez_dev_t * cli_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev;

//! Levarage from MP1.0 CL 170192
	NStatus = EZ_MEM_ALLOC(NULL, (UCHAR **)&updated_configs_local, sizeof(updated_configs_t));
	other_band_ezdev = ez_get_otherband_ap_ezdev(ezdev);

    if(NStatus != NDIS_STATUS_SUCCESS)
    {
            EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%s() allocate memory failed \n", __FUNCTION__));
			ASSERT(FALSE);
            return FALSE;
    }




	if(other_band_ezdev == NULL || other_band_ezdev->ez_security.this_band_info.interface_activated != TRUE)
	{
		if(other_band_ezdev){
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("ez_update_other_band_ap -->other_band interface down !!!! %d\n",
											other_band_ezdev->ez_security.this_band_info.interface_activated));
		}

		NdisCopyMemory(&ap_ezdev->ez_security.other_band_info_backup,&updated_configs->other_band_info,sizeof(interface_info_t));
		ap_ezdev->ez_security.other_band_info_backup.interface_activated = 1;
		NdisCopyMemory(&cli_ezdev->ez_security.other_band_info_backup,&updated_configs->other_band_info,sizeof(interface_info_t));
		cli_ezdev->ez_security.other_band_info_backup.interface_activated = 1;
//! Levarage from MP1.0 CL 170192
		EZ_MEM_FREE(updated_configs_local);
		return FALSE;
	}

	if (other_band_ezdev->driver_ops->ApScanRunning(other_band_ezdev))
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("--->Cancel scan on other band\n"));
	
		other_band_ezdev->driver_ops->APScanCnclAction(other_band_ezdev);
	}
//! Levarage from MP1.0 CL 170192
	NdisCopyMemory(updated_configs_local,updated_configs,sizeof(updated_configs_t));
	NdisCopyMemory(&updated_configs_local->this_band_info,&updated_configs->other_band_info,sizeof(interface_info_t));
	NdisCopyMemory(&updated_configs_local->other_band_info,&updated_configs->this_band_info,sizeof(interface_info_t));
	action_frame_sent =  ez_update_ap(other_band_ezdev->ez_ad, other_band_ezdev, updated_configs_local, group_id_diff, TRUE);
//! Levarage from MP1.0 CL 170192
	EZ_MEM_FREE(updated_configs_local);
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("update beacon for other band\n"));
	return action_frame_sent;
}
BOOLEAN ez_update_this_band_ap(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev, updated_configs_t *updated_configs, BOOLEAN group_id_diff)
{
	ez_dev_t *ap_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev;
	
	if (ap_ezdev->driver_ops->ApScanRunning(ap_ezdev))
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("--->Cancel scan on other band\n"));

		ap_ezdev->driver_ops->APScanCnclAction(ap_ezdev);
	}
	return ez_update_ap(ez_ad, ap_ezdev, updated_configs, group_id_diff, FALSE);
}
#endif

BOOLEAN ez_update_other_band_cli(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev, updated_configs_t *updated_configs, BOOLEAN group_id_diff)
{
	//EZ_ADAPTER *other_band_ez_ad;
	ez_dev_t  *other_band_ezdev = NULL, *other_ap_band_ezdev=NULL;
//! Levarage from MP1.0 CL 170192
	updated_configs_t *updated_configs_local=NULL;
	NDIS_STATUS NStatus;
	BOOLEAN action_frame_sent;
	
//! Levarage from MP1.0 CL 170192
	NStatus = EZ_MEM_ALLOC(NULL, (UCHAR **)&updated_configs_local, sizeof(updated_configs_t));
        if(NStatus != NDIS_STATUS_SUCCESS)
        {
                EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("%s() allocate memory failed \n", __FUNCTION__));
				ASSERT(FALSE);
                return FALSE;
        }

	other_band_ezdev = ez_get_otherband_cli_ezdev(ezdev);
	other_ap_band_ezdev = ez_get_otherband_ap_ezdev(ezdev);


		if(other_band_ezdev == NULL || other_band_ezdev->ez_security.this_band_info.interface_activated != TRUE)
		{
			if(other_band_ezdev){
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("ez_update_other_band_cli -->other_band interface down !!!! %d\n",
											other_band_ezdev->ez_security.this_band_info.interface_activated));
			}
			
			//NdisCopyMemory(&ezdev->ez_security.other_band_info_backup,&updated_configs->other_band_info,sizeof(interface_info_t));
			//ezdev->ez_security.other_band_info_backup.interface_activated = 1;
			if (other_ap_band_ezdev 
				&& other_ap_band_ezdev->ez_security.this_band_info.interface_activated 
				&& (updated_configs->context_linkdown == FALSE)) {	
				other_ap_band_ezdev->driver_ops->ez_restore_channel_config(other_ap_band_ezdev);
#if (defined(DOT11_N_SUPPORT) && defined(DOT11N_DRAFT3))
				if(other_ap_band_ezdev->ez_security.ap_did_fallback){
					if(other_ap_band_ezdev->ez_security.fallback_channel != 
						other_ap_band_ezdev->ez_security.this_band_info.shared_info.channel_info.channel){
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("\nez_update_other_band_cli: Last channel(%d) and current channel(%d) different, reset fallback context\n",
							other_ap_band_ezdev->ez_security.fallback_channel, other_ap_band_ezdev->ez_security.this_band_info.shared_info.channel_info.channel));
						ez_set_ap_fallback_context(other_ap_band_ezdev,FALSE,0);
					}
				}
#endif

				other_ap_band_ezdev->driver_ops->UpdateBeaconHandler
					(other_ap_band_ezdev, IE_CHANGE);
			}
//! Levarage from MP1.0 CL 170192
			EZ_MEM_FREE(updated_configs_local);
			return FALSE;
		}
		NdisCopyMemory(updated_configs_local,updated_configs,sizeof(updated_configs_t));
		NdisCopyMemory(&updated_configs_local->this_band_info,&updated_configs->other_band_info,sizeof(interface_info_t));
		NdisCopyMemory(&updated_configs_local->other_band_info,&updated_configs->this_band_info,sizeof(interface_info_t));

//! Levarage from MP1.0 CL 170197
		action_frame_sent = ez_update_cli(other_band_ezdev->ez_ad, other_band_ezdev, updated_configs_local, group_id_diff, TRUE);
		if ((updated_configs->context_linkdown == FALSE)){
			other_ap_band_ezdev->driver_ops->ez_restore_channel_config(other_ap_band_ezdev);
#if (defined(DOT11_N_SUPPORT) && defined(DOT11N_DRAFT3))
			if(other_ap_band_ezdev->ez_security.ap_did_fallback){
					if(other_ap_band_ezdev->ez_security.fallback_channel != other_ap_band_ezdev->ez_security.this_band_info.shared_info.channel_info.channel){
						EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("\nez_update_other_band_cli: Last channel(%d) and current channel(%d) different, reset fallback context\n",
							other_ap_band_ezdev->ez_security.fallback_channel, other_ap_band_ezdev->ez_security.this_band_info.shared_info.channel_info.channel));
						ez_set_ap_fallback_context(other_ap_band_ezdev,FALSE,0);
					}
			}
#endif
					other_ap_band_ezdev->driver_ops->UpdateBeaconHandler
						(other_ap_band_ezdev, IE_CHANGE);
		}

//! Levarage from MP1.0 CL 170192
		EZ_MEM_FREE(updated_configs_local);
		return action_frame_sent;
}

BOOLEAN ez_update_this_band_cli(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev, updated_configs_t *updated_configs, BOOLEAN group_id_diff)
{
	ez_dev_t *cli_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev;
	return ez_update_cli(ez_ad, cli_ezdev, updated_configs, group_id_diff, FALSE);
}

void ez_update_this_band_cli_peer_record(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev)
{
	ez_dev_t *cli_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev;
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s >>>\n",__FUNCTION__));

	cli_ezdev->driver_ops->ez_update_cli_peer_record(cli_ezdev, FALSE, ezdev->bssid);

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s <<<\n", __FUNCTION__));

}


void ez_switch_wdl_to_other_band(ez_dev_t *ezdev, void *other_band_obj)
{
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	if (ezdev->ezdev_type == EZDEV_TYPE_AP)
	{
		struct _ez_peer_security_info *ez_other_band_peer = other_band_obj;
		COPY_MAC_ADDR(ez_ad->device_info.weight_defining_link.peer_mac, ez_other_band_peer->this_band_info.shared_info.cli_mac_addr);
		COPY_MAC_ADDR(ez_ad->device_info.weight_defining_link.peer_ap_mac, ez_other_band_peer->this_band_info.shared_info.ap_mac_addr);
		ez_ad->device_info.weight_defining_link.ezdev = ez_other_band_peer->ezdev;
		NdisGetSystemUpTime(&ez_ad->device_info.weight_defining_link.ap_time_stamp);
		NdisGetSystemUpTime(&ez_ad->device_info.weight_defining_link.time_stamp);
		ez_inform_all_interfaces(ez_ad, ezdev, ACTION_UPDATE_DEVICE_INFO);
	} else {
		
		interface_info_t *other_band_interface = other_band_obj;
		ez_dev_t  *other_band_ezdev=NULL;
		ez_ad = ezdev->ez_ad;

		COPY_MAC_ADDR(ez_ad->device_info.weight_defining_link.peer_mac, other_band_interface->cli_peer_ap_mac);
		COPY_MAC_ADDR(ez_ad->device_info.weight_defining_link.peer_ap_mac, other_band_interface->cli_peer_ap_mac);
		ez_ad->device_info.weight_defining_link.ezdev = other_band_ezdev;
		NdisGetSystemUpTime(&ez_ad->device_info.weight_defining_link.ap_time_stamp);
		NdisGetSystemUpTime(&ez_ad->device_info.weight_defining_link.time_stamp);
		ez_inform_all_interfaces(ezdev->ez_ad, ezdev, ACTION_UPDATE_DEVICE_INFO);
	}
}

void ez_notify_roam(EZ_ADAPTER *ez_ad, 
	struct _ez_peer_security_info * from_ez_peer, 
	BOOLEAN for_roam, ez_custom_data_cmd_t *data, 
	unsigned char datalen)
{
	int index, i;
	BOOLEAN ap_band_switched = FALSE;
	BOOLEAN cli_band_switched = FALSE;
	
	//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("%s\n", __FUNCTION__));
	for (i = 0; i < MAX_EZ_BANDS; i++)
	{
			ez_dev_t  * ap_ezdev =  &ez_ad->ez_band_info[i].ap_ezdev;
			ez_dev_t  * cli_ezdev =  &ez_ad->ez_band_info[i].cli_ezdev;
						

			if (ap_ezdev)
			{
				//! first send an action frame to EZ peers so that they do not disconnect
				for (index = 0; index < EZ_MAX_STA_NUM; index ++)
				{
					if (EZ_GET_EZBAND_BAND(ap_ezdev->ez_ad,ap_ezdev->ez_band_idx)->ez_peer_table[index].port_secured
						&& EZ_GET_EZBAND_BAND(ap_ezdev->ez_ad,ap_ezdev->ez_band_idx)->ez_peer_table[index].ezdev == ap_ezdev
						&& (for_roam ? !EZ_GET_EZBAND_BAND(ap_ezdev->ez_ad,ap_ezdev->ez_band_idx)->ez_peer_table[index].ez_disconnect_due_roam : TRUE)
						&& &EZ_GET_EZBAND_BAND(ap_ezdev->ez_ad,ap_ezdev->ez_band_idx)->ez_peer_table[index] != from_ez_peer){
						
						if (from_ez_peer && (ez_is_link_duplicate(from_ez_peer) 
							&& (ez_get_other_band_ez_peer(from_ez_peer->ezdev ,from_ez_peer) == &EZ_GET_EZBAND_BAND(ap_ezdev->ez_ad,ap_ezdev->ez_band_idx)->ez_peer_table[index])))
						{
							continue;
						}
	
						if (ez_is_link_duplicate(&EZ_GET_EZBAND_BAND(ap_ezdev->ez_ad,ap_ezdev->ez_band_idx)->ez_peer_table[index]))
						{
							if (ap_band_switched)
							{
								continue;
							} 
						}
						if (for_roam)
						{
							send_action_notify_roam(ap_ezdev->ez_ad,ap_ezdev,&EZ_GET_EZBAND_BAND(ap_ezdev->ez_ad,ap_ezdev->ez_band_idx)->ez_peer_table[index]);
						}
						else
						{
							send_action_custom_data(ap_ezdev->ez_ad,ap_ezdev,&EZ_GET_EZBAND_BAND(ap_ezdev->ez_ad,ap_ezdev->ez_band_idx)->ez_peer_table[index], data, datalen);
						}
	
					}
				}
				ap_band_switched = TRUE;
				
			}
			if (cli_ezdev)
			{
				//! first send an action frame to EZ peers so that they do not disconnect
				for (index = 0; index < EZ_MAX_STA_NUM; index ++)
				{
					if (EZ_GET_EZBAND_BAND(cli_ezdev->ez_ad,cli_ezdev->ez_band_idx)->ez_peer_table[index].port_secured
						&& EZ_GET_EZBAND_BAND(cli_ezdev->ez_ad,cli_ezdev->ez_band_idx)->ez_peer_table[index].ezdev == cli_ezdev
						&& (for_roam ? !EZ_GET_EZBAND_BAND(cli_ezdev->ez_ad,cli_ezdev->ez_band_idx)->ez_peer_table[index].ez_disconnect_due_roam : TRUE)
						&& &EZ_GET_EZBAND_BAND(cli_ezdev->ez_ad,cli_ezdev->ez_band_idx)->ez_peer_table[index] != from_ez_peer){
						
						if (from_ez_peer && (ez_is_link_duplicate(from_ez_peer) 
							&& (ez_get_other_band_ez_peer(from_ez_peer->ezdev ,from_ez_peer) == &EZ_GET_EZBAND_BAND(cli_ezdev->ez_ad,cli_ezdev->ez_band_idx)->ez_peer_table[index])))
						{
							continue;
						}
	
						if (ez_is_link_duplicate(&EZ_GET_EZBAND_BAND(cli_ezdev->ez_ad,cli_ezdev->ez_band_idx)->ez_peer_table[index]))
						{
							if (cli_band_switched)
							{
								continue;
							} 
						}
						if (for_roam)
						{
							send_action_notify_roam(cli_ezdev->ez_ad,cli_ezdev,&EZ_GET_EZBAND_BAND(cli_ezdev->ez_ad,cli_ezdev->ez_band_idx)->ez_peer_table[index]);
						}
						else
						{
							send_action_custom_data(cli_ezdev->ez_ad,cli_ezdev,&EZ_GET_EZBAND_BAND(cli_ezdev->ez_ad,cli_ezdev->ez_band_idx)->ez_peer_table[index], data, datalen);
						}
					}
				}
				cli_band_switched = TRUE;
				
			}
		}
}

void ez_update_other_band_cli_peer_record(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev)
{
	
	ez_dev_t  *other_band_ezdev = NULL;

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s >>>\n",__FUNCTION__));

	other_band_ezdev = ez_get_otherband_cli_ezdev(ezdev);
	
	if(other_band_ezdev == NULL || other_band_ezdev->ez_security.this_band_info.interface_activated != TRUE)
	{
		return;
	}

	other_band_ezdev->driver_ops->ez_update_cli_peer_record(other_band_ezdev, TRUE, other_band_ezdev->bssid);
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s <<<\n", __FUNCTION__));

}

void ez_update_other_band_ap_peer_record(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev)
{
	ez_dev_t  *other_band_ezdev = NULL; 
	int index =0;
	int flags;

	other_band_ezdev = ez_get_otherband_ap_ezdev(ezdev);

	if(other_band_ezdev == NULL || other_band_ezdev->ez_security.this_band_info.interface_activated != TRUE)
	{
		return;
	}

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s >>>\n",__FUNCTION__));

	if (other_band_ezdev->driver_ops->ApScanRunning(other_band_ezdev))
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("--->Cancel scan on other band\n"));
			other_band_ezdev->driver_ops->APScanCnclAction(other_band_ezdev);
		}

		for (index = 0; index < EZ_MAX_STA_NUM; index ++)
		{
		
#ifdef DUAL_CHIP
			if((!IS_SINGLE_CHIP_DBDC((EZ_ADAPTER *)ez_ad)) && TRUE)
				EZ_IRQ_LOCK(&ez_ad->ez_set_peer_lock, flags);
#endif
			if (ez_ad->ez_band_info[ezdev->ez_band_idx].ez_peer_table[index].port_secured
				&& ez_ad->ez_band_info[ezdev->ez_band_idx].ez_peer_table[index].ezdev == other_band_ezdev
				)
			{
				//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("%s, wdev->idx:%x ap_peer_mac = %02x:%02x:%02x:%02x:%02x:%02x\n", 
				//	__FUNCTION__,wdev->wdev_idx,PRINT_MAC(wdev->ez_peer_table[index].mac_addr)));

#ifdef DUAL_CHIP
				if(!IS_SINGLE_CHIP_DBDC(ez_ad) && TRUE)
					EZ_IRQ_UNLOCK(&ez_ad->ez_set_peer_lock, flags);
#endif
					other_band_ezdev->driver_ops->ez_update_ap_peer_record(other_band_ezdev, TRUE,
										ez_ad->ez_band_info[ezdev->ez_band_idx].ez_peer_table[index].mac_addr);

			} else {
		
#ifdef DUAL_CHIP
			if((!IS_SINGLE_CHIP_DBDC(ez_ad)) && TRUE)
				EZ_IRQ_UNLOCK(&ez_ad->ez_set_peer_lock, flags);
#endif
			}
		}
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s <<<\n", __FUNCTION__));
}

void ez_update_this_band_ap_peer_record(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev)
{
	int index =0;
	int flags;
	ez_dev_t * ap_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev;
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s >>>\n",__FUNCTION__));

	if (ap_ezdev->driver_ops->ApScanRunning(ap_ezdev))
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("--->Cancel scan on other band\n"));

		ap_ezdev->driver_ops->APScanCnclAction(ap_ezdev);
	}

	
	for (index = 0; index < EZ_MAX_STA_NUM; index ++)
	{
	
#ifdef DUAL_CHIP
		if((!IS_SINGLE_CHIP_DBDC(ez_ad)) && TRUE)
			EZ_IRQ_LOCK(&ez_ad->ez_set_peer_lock, flags);
#endif
		if (ez_ad->ez_band_info[ap_ezdev->ez_band_idx].ez_peer_table[index].port_secured
			&& ez_ad->ez_band_info[ap_ezdev->ez_band_idx].ez_peer_table[index].ezdev == ezdev)
		{
			//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("%s, wdev->idx:%x ap_peer_mac = %02x:%02x:%02x:%02x:%02x:%02x\n", 
			//	__FUNCTION__,wdev->wdev_idx,PRINT_MAC(wdev->ez_peer_table[index].mac_addr)));

#ifdef DUAL_CHIP
			if((!IS_SINGLE_CHIP_DBDC(ez_ad)) && TRUE)
				EZ_IRQ_UNLOCK(&ez_ad->ez_set_peer_lock, flags);
#endif
			ap_ezdev->driver_ops->ez_update_ap_peer_record(ap_ezdev, FALSE,ez_ad->ez_band_info[ap_ezdev->ez_band_idx].ez_peer_table[index].mac_addr);

		} else {
	
#ifdef DUAL_CHIP
		if((!IS_SINGLE_CHIP_DBDC(ez_ad)) && TRUE)
			EZ_IRQ_UNLOCK(&ez_ad->ez_set_peer_lock, flags);
#endif
		}
	}
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("%s <<<\n", __FUNCTION__));

}

ez_dev_t * ez_get_otherband_ezdev(ez_dev_t *ezdev)
{
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	if((ezdev->ezdev_type == EZDEV_TYPE_AP))
	{
		if (ez_ad->ez_band_info[ezdev->ez_band_idx ^ 1].ap_ezdev.wdev)
			return (&ez_ad->ez_band_info[ezdev->ez_band_idx ^ 1].ap_ezdev);
	}
	else 
	{
		if (ez_ad->ez_band_info[ezdev->ez_band_idx ^ 1].cli_ezdev.wdev)
			return (&ez_ad->ez_band_info[ezdev->ez_band_idx ^ 1].cli_ezdev);
	}
	return NULL;		

}
ez_dev_t * ez_get_otherband_ap_ezdev(ez_dev_t *ezdev)
{
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	if (ez_ad->ez_band_info[ezdev->ez_band_idx ^ 1].ap_ezdev.wdev)
	{		
		return &(ez_ad->ez_band_info[ezdev->ez_band_idx ^ 1].ap_ezdev);
	}
	return NULL;
		
}
ez_dev_t * ez_get_otherband_cli_ezdev(ez_dev_t *ezdev)
{
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	if (ez_ad->ez_band_info[ezdev->ez_band_idx ^ 1].cli_ezdev.wdev)
		return &(ez_ad->ez_band_info[ezdev->ez_band_idx ^ 1].cli_ezdev);
	return NULL;
}
#if 0
void ez_convert_pmk_string_to_hex(char *sys_pmk_string, char *sys_pmk)
{
	int ret;
	unsigned char nibble;
	
	for (ret = 0; ret < 64; ret++)
	{
		nibble = sys_pmk_string[ret];
		if ((nibble <= '9'))
		{
			nibble = nibble - '0';
		} 
		else if (nibble < 'a') 
		{
			nibble = nibble - 'A' + 10;
		} else {
			nibble = nibble - 'a' + 10;			
		}
		if (ret % 2)
		{
			sys_pmk[ret/2] |= nibble; 
		}
		else 
		{
			sys_pmk[ret/2] = nibble << 4;
		}
	}
	
}
#endif
#ifdef EZ_NETWORK_MERGE_SUPPORT
#if 1
/*check whether peer node is child node of the own node*/
BOOLEAN ez_is_weight_same_mod(
	PUCHAR own_weight, 
	PUCHAR peer_weight)
{
	
	//ez_hex_dump("PeerWeight", (PUCHAR)peer_weight, NETWORK_WEIGHT_LEN);

	if ((own_weight[0] == 0xF) && (peer_weight[0] == 0xF))
	{
	
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_INFO,("Both are User Configured \n"));
		return TRUE;
	}
	else if (NdisEqualMemory(own_weight, peer_weight, NETWORK_WEIGHT_LEN-1))
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Wt Same\n"));
		ez_hex_dump("PeerWeight", (PUCHAR)peer_weight, NETWORK_WEIGHT_LEN);
		return TRUE;
	}
	else
	{
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("FALSE\n"));
		return FALSE;
	}
}
#endif
VOID EzRtmpOsMsDelay(ULONG msec)
{
	mdelay(msec);
}

void send_action_notify_roam(EZ_ADAPTER *ez_ad, 
					ez_dev_t *ezdev, 
					struct _ez_peer_security_info *ez_peer
					)
{

	unsigned char *out_buf;
	NDIS_STATUS NStatus;
	FRAME_ACTION_HDR frame;
	unsigned long frame_len;
	unsigned long tmp_len;
	
	out_buf = NULL;
	frame_len = 0;
	tmp_len = 0;
	
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("------> %s, Peer = %x:%x:%x:%x:%x:%x\n", __FUNCTION__, PRINT_MAC(ez_peer->mac_addr)));

	NStatus = EZ_MEM_ALLOC(ez_ad, &out_buf, MGMT_DMA_BUFFER_SIZE); /*Get an unused nonpaged memory */
	if (NStatus != NDIS_STATUS_SUCCESS) {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,
			 ("%s(): allocate memory failed \n",
			 __FUNCTION__));
		return;
	}
	
	ezdev->ez_security.ez_action_type = ACTION_TYPE_NOTIFY_ROAM;
	EzActHeaderInit(&frame.Hdr, ez_peer->mac_addr, ezdev->if_addr, ezdev->bssid);
	frame.Category = CATEGORY_PUBLIC;
	frame.Action = ACTION_WIFI_DIRECT;	// Action == 0x09, for vendor specific Action
	
	EzMakeOutgoingFrame(out_buf, &frame_len,
				  sizeof(FRAME_ACTION_HDR), &frame,
				  END_OF_ARGS);

	/*	Insert OUI Information	*/

	NdisMoveMemory(&out_buf[frame_len], mtk_oui, MTK_OUI_LEN);
	frame_len += MTK_OUI_LEN;
	
	
	ez_insert_tlv(EZ_TAG_NOTIFY_ROAM, 
		NULL, 
		0, 
		out_buf + frame_len, 
		&tmp_len);
	frame_len += tmp_len;

	ezdev->driver_ops->MiniportMMRequest(ezdev, out_buf, frame_len, TRUE);
	
	ezdev->ez_security.ez_action_type = ACTION_TYPE_NONE;
	EzRtmpOsMsDelay(100);
	EZ_MEM_FREE( out_buf);
}

void send_action_delay_disconnect(EZ_ADAPTER *ez_ad, 
					ez_dev_t *ezdev, 
					struct _ez_peer_security_info *ez_peer,
					unsigned char delay_disconnect_count
					)
{

	unsigned char *out_buf;
	NDIS_STATUS NStatus;
	FRAME_ACTION_HDR frame;
	unsigned long frame_len;
	unsigned long tmp_len;
	out_buf = NULL;
	frame_len = 0;
	tmp_len = 0;
	
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("------> %s\n", __FUNCTION__));

	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("delay_disconnect_count = %d\n", ez_ad->ez_delay_disconnect_count));
	
	NStatus = EZ_MEM_ALLOC(ez_ad, &out_buf, MGMT_DMA_BUFFER_SIZE); /*Get an unused nonpaged memory */
	if (NStatus != NDIS_STATUS_SUCCESS) {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,
			 ("%s(): allocate memory failed \n",
			 __FUNCTION__));
		return;
	}
	
	ezdev->ez_security.ez_action_type = ACTION_TYPE_DELAY_DISCONNECT;
	EzActHeaderInit(&frame.Hdr, ez_peer->mac_addr, ezdev->if_addr, ezdev->bssid);
	frame.Category = CATEGORY_PUBLIC;
	frame.Action = ACTION_WIFI_DIRECT;	// Action == 0x09, for vendor specific Action
	
	EzMakeOutgoingFrame(out_buf, &frame_len,
				  sizeof(FRAME_ACTION_HDR), &frame,
				  END_OF_ARGS);

	/*	Insert OUI Information	*/

	NdisMoveMemory(&out_buf[frame_len], mtk_oui, MTK_OUI_LEN);
	frame_len += MTK_OUI_LEN;
	
	
	ez_insert_tlv(EZ_TAG_DELAY_DISCONNECT_COUNT, 
		&delay_disconnect_count, 
		1, 
		out_buf + frame_len, 
		&tmp_len);
	frame_len += tmp_len;

	ezdev->driver_ops->MiniportMMRequest(ezdev, out_buf, frame_len, FALSE);
	
	ezdev->ez_security.ez_action_type = ACTION_TYPE_NONE;
	EzRtmpOsMsDelay(100);
	EZ_MEM_FREE( out_buf);
}
void send_action_update_weight(EZ_ADAPTER *ez_ad,
					unsigned char *mac_addr,
					ez_dev_t *ezdev, 
					unsigned char * network_weight)
{
	unsigned char *out_buf;
	NDIS_STATUS NStatus;
	FRAME_ACTION_HDR frame;
	unsigned long frame_len;
	unsigned long tmp_len;
	unsigned char network_weight_local[NETWORK_WEIGHT_LEN];

	
	NStatus = EZ_MEM_ALLOC(ez_ad, &out_buf, MGMT_DMA_BUFFER_SIZE); /*Get an unused nonpaged memory */
	if (NStatus != NDIS_STATUS_SUCCESS) {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,
			 ("%s(): allocate memory failed \n",
			 __FUNCTION__));
		return;
	}
	
	EzActHeaderInit(&frame.Hdr, mac_addr, ezdev->if_addr, ezdev->bssid);
	frame.Category = CATEGORY_PUBLIC;
	frame.Action = ACTION_WIFI_DIRECT;	// Action == 0x09, for vendor specific Action
	
	EzMakeOutgoingFrame(out_buf, &frame_len,
				  sizeof(FRAME_ACTION_HDR), &frame,
				  END_OF_ARGS);

	/*	Insert OUI Information	*/

	NdisMoveMemory(&out_buf[frame_len], mtk_oui, MTK_OUI_LEN);
	frame_len += MTK_OUI_LEN;
	NdisCopyMemory(network_weight_local,network_weight,NETWORK_WEIGHT_LEN);
	network_weight_local[0] |= BIT(7);
	ez_hex_dump("Update peer weight", network_weight_local, NETWORK_WEIGHT_LEN);
	ez_insert_tlv(EZ_TAG_NETWORK_WEIGHT, 
							(unsigned char *)network_weight_local, 
							NETWORK_WEIGHT_LEN, 
							out_buf + frame_len, 
							&tmp_len);
	
	//ez_hex_dump("Update peer weight", &out_buf[frame_len], NETWORK_WEIGHT_LEN);
		

	frame_len += tmp_len;
	ezdev->driver_ops->MiniportMMRequest(ezdev, out_buf, frame_len, TRUE);

	EZ_MEM_FREE( out_buf);
	ez_update_connection_permission_hook(ezdev,EZ_DISALLOW_ALL);

			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,
						 ("%s(): ez_ad->ez_wait_for_info_transfer : %d\n",
						 __FUNCTION__, ez_ad->ez_wait_for_info_transfer));

			ez_wait_for_connection_allow(ez_ad->ez_wait_for_info_transfer * EZ_SEC_TO_MSEC, ez_ad);
			
	//}
	//! TODO
	//! Add Node number as well
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("<------ %s\n", __FUNCTION__));
}

BOOLEAN ez_send_action_update_config_for_this_band(EZ_ADAPTER *ez_ad, 
	ez_dev_t *ezdev, 
	updated_configs_t *updated_configs, 
	BOOLEAN group_id_diff, 
	BOOLEAN band_switched, 
	BOOLEAN deauth_non_ez_sta)
{
	int index =0;
	BOOLEAN action_sent = FALSE;

	for (index = 0; index < EZ_MAX_STA_NUM; index ++)
	{
		//! if ez_peer is in connected state send action frame
		struct _ez_peer_security_info *ez_peer_table_entry = &EZ_GET_EZBAND_BAND(ezdev->ez_ad,ezdev->ez_band_idx)->ez_peer_table[index];
		if (ez_peer_table_entry->port_secured
			&& ez_peer_table_entry->ezdev == ezdev
			&& !MAC_ADDR_EQUAL(ez_peer_table_entry->mac_addr, updated_configs->mac_addr)){
			if (band_switched 
				&& ez_peer_table_entry->this_band_info.shared_info.link_duplicate)
			{
				struct _ez_peer_security_info * ez_peer = ez_get_other_band_ez_peer(ezdev,ez_peer_table_entry);
				if(ez_peer == NULL)
				{
					ASSERT(FALSE);
					return FALSE;
				}
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("duplicateLink: Update the node Number\n"));
				NdisCopyMemory(&ez_peer_table_entry->device_info.ez_node_number,&ez_peer->device_info.ez_node_number,sizeof(EZ_NODE_NUMBER));
				ez_hex_dump("ez_peerMAC",ez_peer_table_entry->mac_addr,6);
				ez_hex_dump("ez_node_number", (PUCHAR)&ez_peer_table_entry->device_info.ez_node_number, sizeof(EZ_NODE_NUMBER));
				continue;
			}
			if (send_action_update_config(ez_ad, ez_peer_table_entry, ezdev, updated_configs, TRUE, group_id_diff)== TRUE)
			action_sent = TRUE;
		}
	}

	if (updated_configs->context_linkdown == FALSE) {

		if(deauth_non_ez_sta){
			//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			//			("------> %s(): AP BASIC CONFIG CHANGED, BRDCST DEAUTH\n", __FUNCTION__));

			//! send out a broadcast de-auth
			 ezdev->driver_ops->ez_send_broadcast_deauth(ezdev);
		}
		else{
			//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			//			("------> %s(): AVOID DEAUTH TO NON EZ STA\n", __FUNCTION__));
		}

	}
	return action_sent;
}

BOOLEAN send_action_update_config(EZ_ADAPTER *ez_ad, 
					struct _ez_peer_security_info *ez_peer,
					ez_dev_t *ezdev, 
					updated_configs_t *updated_configs,
					BOOLEAN same_band,
					BOOLEAN group_id_update)
{

	unsigned char *out_buf;
	//NDIS_STATUS NStatus;
	FRAME_ACTION_HDR frame;
	unsigned long frame_len;
	unsigned long tmp_len;
	unsigned char *entrypted_data;
	unsigned int entrypted_data_len;
	struct _ez_security *ez_sec_info;
	unsigned char ez_this_band_psk_len;
	unsigned char ez_other_band_psk_len;
	unsigned char length_psk_with_padding;
	
	unsigned char update_group_id = group_id_update ? 1:0;

	ez_hex_dump("Send Action to peer",ez_peer->mac_addr,MAC_ADDR_LEN);

	ez_sec_info = &ezdev->ez_security;
	out_buf = NULL;
	frame_len = 0;
	tmp_len = 0;
	
	
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("------> %s\n", __FUNCTION__));

	if (ez_peer == NULL)
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("ez_peer empty\n"));
		return FALSE;
	}
	
	if (ezdev == NULL)
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("ezdev empty\n"));
		return FALSE;
	}
	
	if (updated_configs == NULL)
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("updated_configs empty\n"));
		return FALSE;
	}
	EZ_MEM_ALLOC(ez_ad, &out_buf, MGMT_DMA_BUFFER_SIZE); /*Get an unused nonpaged memory */
	if (out_buf == NULL) {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,
			 ("%s(): allocate memory failed \n",
			 __FUNCTION__));
		return FALSE;
	}
	
	EzActHeaderInit(&frame.Hdr, ez_peer->mac_addr, ezdev->if_addr, ezdev->bssid);
	frame.Category = CATEGORY_PUBLIC;
	frame.Action = ACTION_WIFI_DIRECT;	// Action == 0x09, for vendor specific Action
	
	EzMakeOutgoingFrame(out_buf, &frame_len,
				  sizeof(FRAME_ACTION_HDR), &frame,
				  END_OF_ARGS);

	/*	Insert OUI Information	*/

	NdisMoveMemory(&out_buf[frame_len], mtk_oui, MTK_OUI_LEN);
	frame_len += MTK_OUI_LEN;
	
	
	ez_insert_tlv(EZ_TAG_GROUP_ID_UPDATE, 
		&update_group_id, 
		1, 
		out_buf + frame_len, 
		&tmp_len);
	frame_len += tmp_len;
	
	if(ez_peer->gen_group_id)
	{
		unsigned char *entrypted_seed;
		unsigned int entrypted_seed_len;
		EZ_MEM_ALLOC(NULL, &entrypted_seed,ez_peer->gen_group_id_len  + EZ_AES_KEY_ENCRYPTION_EXTEND);
		if(entrypted_seed)
		{
			NdisZeroMemory(entrypted_seed,ez_peer->gen_group_id_len  + EZ_AES_KEY_ENCRYPTION_EXTEND);
			ezdev->driver_ops->AES_Key_Wrap(ezdev,
						ez_peer->gen_group_id,ez_peer->gen_group_id_len, ez_peer->sw_key, LEN_PTK_KEK, 
						 entrypted_seed, &entrypted_seed_len);
				
			/*
				Insert encrypted Generated Group ID Seed.
			*/
			ez_insert_tlv(EZ_TAG_GROUPID_SEED, 
				entrypted_seed, 
				entrypted_seed_len, 
				out_buf + frame_len, 
				&tmp_len);
			frame_len += tmp_len;
			EZ_MEM_FREE( entrypted_seed);
		}
	}
	/*
		Insert Encrypted Group ID
	*/
	EZ_MEM_ALLOC(NULL, &entrypted_data, updated_configs->group_id_len + EZ_AES_KEY_ENCRYPTION_EXTEND);
	if (entrypted_data) {
		/* encrypt */
		ezdev->driver_ops->AES_Key_Wrap(ezdev,
			(unsigned char *)updated_configs->group_id, 
			updated_configs->group_id_len, 
			ez_peer->sw_key, LEN_PTK_KEK, 
			entrypted_data, &entrypted_data_len);
		
		ez_insert_tlv(EZ_TAG_GROUP_ID, 
			entrypted_data, 
			entrypted_data_len, 
			out_buf + frame_len, 
			&tmp_len);
		frame_len += tmp_len;
		
		EZ_MEM_FREE( entrypted_data);

	}

	EZ_MEM_ALLOC(NULL, &entrypted_data, updated_configs->group_id_len + EZ_AES_KEY_ENCRYPTION_EXTEND);
	if (entrypted_data) {
		/* encrypt */
		ezdev->driver_ops->AES_Key_Wrap(ezdev,
			(unsigned char *)updated_configs->group_id, 
			updated_configs->group_id_len, 
			ez_peer->sw_key, LEN_PTK_KEK, 
			entrypted_data, &entrypted_data_len);
		
		ez_insert_tlv(EZ_TAG_GROUP_ID, 
			entrypted_data, 
			entrypted_data_len, 
			out_buf + frame_len, 
			&tmp_len);
		frame_len += tmp_len;
		
		EZ_MEM_FREE( entrypted_data);

	}

	if (ez_is_triband_hook()) {
		NON_EZ_BAND_INFO_TAG non_ez_tag[2];
		NON_EZ_BAND_PSK_INFO_TAG non_ez_psk_tag[2];
		NdisZeroMemory(non_ez_tag, sizeof(non_ez_tag));
		NdisZeroMemory(non_ez_psk_tag, sizeof(non_ez_psk_tag));
		ez_prepare_non_ez_tag(&non_ez_tag[0], &non_ez_psk_tag[0],ez_peer);
		
		ez_insert_tlv(EZ_TAG_NON_EZ_CONFIG, 
			(unsigned char *)&non_ez_tag[0], 
			sizeof(non_ez_tag), 
			out_buf + frame_len, 
			&tmp_len);
		frame_len += tmp_len;
		ez_insert_tlv(EZ_TAG_NON_EZ_PSK, 
			(unsigned char *)&non_ez_psk_tag[0], 
			sizeof(non_ez_psk_tag), 
			out_buf + frame_len, 
			&tmp_len);
		frame_len += tmp_len;

	} else if(ez_ad->is_man_nonman) {
//! Levarage from MP1.0 CL#170037
		NON_MAN_INFO_TAG non_man_tag;

		ez_prepare_non_man_tag(&non_man_tag, ez_peer);
		
		ez_insert_tlv(EZ_TAG_NON_MAN_CONFIG, 
			(unsigned char *)&non_man_tag, 
			sizeof(non_man_tag), 
			out_buf + frame_len, 
			&tmp_len);
		frame_len += tmp_len;


	}

	ez_insert_tlv(EZ_TAG_OPEN_GROUP_ID, 
			updated_configs->open_group_id, 
			updated_configs->open_group_id_len, 
			out_buf + frame_len, 
			&tmp_len);
	frame_len += tmp_len;

	
	EZ_MEM_ALLOC(NULL, &entrypted_data, EZ_PMK_LEN + EZ_AES_KEY_ENCRYPTION_EXTEND);
	if (entrypted_data) {
		/* encrypt */
		ez_peer->ezdev->driver_ops->AES_Key_Wrap(ez_peer->ezdev,
					(unsigned char *)updated_configs->this_band_info.pmk, EZ_PMK_LEN, 
					ez_peer->sw_key, LEN_PTK_KEK, 
					entrypted_data, &entrypted_data_len);
		ez_insert_tlv(EZ_TAG_PMK, 
			entrypted_data, 
			entrypted_data_len, 
			out_buf + frame_len, 
			&tmp_len);
		frame_len += tmp_len;

		ez_peer->ezdev->driver_ops->AES_Key_Wrap(ez_peer->ezdev,
				(unsigned char *)updated_configs->other_band_info.pmk, 
				EZ_PMK_LEN, 
				ez_peer->sw_key, LEN_PTK_KEK, 
				entrypted_data, &entrypted_data_len);
		ez_insert_tlv(EZ_TAG_OTHER_BAND_PMK, 
			entrypted_data, 
			entrypted_data_len, 
			out_buf + frame_len, 
			&tmp_len);
		frame_len += tmp_len;
		
		EZ_MEM_FREE( entrypted_data);

	}

	ez_this_band_psk_len = strlen(updated_configs->this_band_info.psk);
	ez_insert_tlv(EZ_TAG_PSK_LEN, 
		&ez_this_band_psk_len, 
		1, 
		out_buf + frame_len, 
		&tmp_len);
	frame_len += tmp_len;

	length_psk_with_padding = ez_this_band_psk_len;
	if (length_psk_with_padding % AES_KEYWRAP_BLOCK_SIZE != 0)
		length_psk_with_padding += AES_KEYWRAP_BLOCK_SIZE - (length_psk_with_padding % AES_KEYWRAP_BLOCK_SIZE);
	
	EZ_MEM_ALLOC(NULL, &entrypted_data, length_psk_with_padding + EZ_AES_KEY_ENCRYPTION_EXTEND);
	if (entrypted_data) {
		/* encrypt */
		ez_peer->ezdev->driver_ops->AES_Key_Wrap(ez_peer->ezdev,
			(unsigned char *)updated_configs->this_band_info.psk, length_psk_with_padding,
			ez_peer->sw_key, LEN_PTK_KEK, 
			entrypted_data, &entrypted_data_len);
		ez_insert_tlv(EZ_TAG_PSK, 
			entrypted_data, 
			entrypted_data_len, 
			out_buf + frame_len, 
			&tmp_len);
		frame_len += tmp_len;

		EZ_MEM_FREE(entrypted_data);
	}

	ez_other_band_psk_len = strlen(updated_configs->other_band_info.psk);
	ez_insert_tlv(EZ_TAG_OTHER_BAND_PSK_LEN, 
		&ez_other_band_psk_len, 
		1, 
		out_buf + frame_len, 
		&tmp_len);
	frame_len += tmp_len;

	length_psk_with_padding = ez_other_band_psk_len;
	if (length_psk_with_padding % AES_KEYWRAP_BLOCK_SIZE != 0)
		length_psk_with_padding += AES_KEYWRAP_BLOCK_SIZE - (length_psk_with_padding % AES_KEYWRAP_BLOCK_SIZE);
	EZ_MEM_ALLOC(NULL, &entrypted_data, length_psk_with_padding + EZ_AES_KEY_ENCRYPTION_EXTEND);
	if (entrypted_data) {
		/* encrypt */
		ez_peer->ezdev->driver_ops->AES_Key_Wrap(ez_peer->ezdev,
			(unsigned char *)updated_configs->other_band_info.psk, length_psk_with_padding, 
			ez_peer->sw_key, LEN_PTK_KEK, 
			entrypted_data, &entrypted_data_len);
		ez_insert_tlv(EZ_TAG_OTHER_BAND_PSK, 
			entrypted_data, 
			entrypted_data_len, 
			out_buf + frame_len, 
			&tmp_len);
		frame_len += tmp_len;

		EZ_MEM_FREE(entrypted_data);
	}


	{
		interface_info_tag_t shared_info[2];
		interface_info_t other_band_info;
		NdisCopyMemory(&shared_info[0],&ezdev->ez_security.this_band_info.shared_info,sizeof(interface_info_tag_t));
		if (ez_get_other_band_info(ezdev, &other_band_info)){
			NdisCopyMemory(&shared_info[1],&other_band_info.shared_info,sizeof(interface_info_tag_t));
		} else {
			NdisZeroMemory(&shared_info[1],sizeof(interface_info_tag_t));
		}
		shared_info[1].ssid_len = updated_configs->other_band_info.shared_info.ssid_len;
		NdisCopyMemory(shared_info[1].ssid, updated_configs->other_band_info.shared_info.ssid, updated_configs->other_band_info.shared_info.ssid_len);
		NdisCopyMemory(&shared_info[1].channel_info, &updated_configs->other_band_info.shared_info.channel_info, sizeof(channel_info_t));
#ifdef DOT11R_FT_SUPPORT
		FT_SET_MDID(shared_info[1].FtMdId, updated_configs->other_band_info.shared_info.FtMdId);
#endif

		ez_insert_tlv(EZ_TAG_INTERFACE_INFO, 
			(unsigned char *)&shared_info[0], 
			sizeof(shared_info), 
			out_buf + frame_len, 
			&tmp_len);
		frame_len += tmp_len;

		ez_allocate_node_number_sta(ez_peer, TRUE);
		NdisCopyMemory(&updated_configs->device_info.ez_node_number,&ez_peer->device_info.ez_node_number,sizeof(EZ_NODE_NUMBER));

		ez_insert_tlv(EZ_TAG_DEVICE_INFO, 
			(unsigned char *)&updated_configs->device_info, 
			sizeof(device_info_t), 
			out_buf + frame_len, 
			&tmp_len);
		frame_len += tmp_len;
}


	ezdev->driver_ops->MiniportMMRequest(ezdev, out_buf, frame_len, TRUE);

	EZ_MEM_FREE( out_buf);
	
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,
				 ("%s(): ez_ad->ez_wait_for_info_transfer : %d\n",
				 __FUNCTION__, ez_ad->ez_wait_for_info_transfer));
	

	ez_update_connection_permission_hook(ezdev, EZ_DISALLOW_ALL);
	ez_wait_for_connection_allow(ez_ad->ez_wait_for_info_transfer * EZ_SEC_TO_MSEC, ez_ad);
	
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("<------ %s\n", __FUNCTION__));
	return TRUE;

}

BOOLEAN send_action_custom_data(EZ_ADAPTER *ez_ad, 
					ez_dev_t *ezdev, 
					struct _ez_peer_security_info *ez_peer,
					ez_custom_data_cmd_t *data, 
					unsigned char datalen 
					)
{

	unsigned char *out_buf;
	NDIS_STATUS NStatus;
	FRAME_ACTION_HDR frame;
	unsigned long frame_len;
	unsigned long tmp_len;
	out_buf = NULL;
	frame_len = 0;
	tmp_len = 0;
	
	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, 
			("------> %s\n", __FUNCTION__));

	//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("delay_disconnect_count = %d\n", ez_ad->ez_delay_disconnect_count));
	
	NStatus = EZ_MEM_ALLOC(ez_ad, &out_buf, MGMT_DMA_BUFFER_SIZE); /*Get an unused nonpaged memory */
	if (NStatus != NDIS_STATUS_SUCCESS) {
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,
			 ("%s(): allocate memory failed \n",
			 __FUNCTION__));
		return FALSE;
	}
	
	//ezdev->ez_security.ez_action_type = ACTION_TYPE_DELAY_DISCONNECT;
	EzActHeaderInit(&frame.Hdr, ez_peer->mac_addr, ezdev->if_addr, ezdev->bssid);
	frame.Category = CATEGORY_PUBLIC;
	frame.Action = ACTION_WIFI_DIRECT;	// Action == 0x09, for vendor specific Action
	
	EzMakeOutgoingFrame(out_buf, &frame_len,
				  sizeof(FRAME_ACTION_HDR), &frame,
				  END_OF_ARGS);

	/*	Insert OUI Information	*/

	NdisMoveMemory(&out_buf[frame_len], mtk_oui, MTK_OUI_LEN);
	frame_len += MTK_OUI_LEN;
	
	
	ez_insert_tlv(EZ_TAG_COUSTOM_DATA, 
		(unsigned char *)data, 
		datalen, 
		out_buf + frame_len, 
		&tmp_len);
	frame_len += tmp_len;

	ezdev->driver_ops->MiniportMMRequest(ezdev, out_buf, frame_len, FALSE);
	
	//ezdev->ez_security.ez_action_type = ACTION_TYPE_NONE;
	EzRtmpOsMsDelay(100);
	EZ_MEM_FREE( out_buf);

	return TRUE;
}

void ez_hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen)
{
		unsigned char *pt;
		int x;
		if (ez_adapter->debug < DBG_LVL_TRACE)
			return;
		pt = pSrcBufVA;
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%s: %p, len = %d\n", str, pSrcBufVA, SrcBufLen));
		for (x = 0; x < SrcBufLen; x++) {
			if (x % 16 == 0)
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("0x%04x : ", x));
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%02x ", ((unsigned char)pt[x])));
			if (x % 16 == 15)
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
		}
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("\n"));
}


 BOOLEAN is_other_band_rcvd_pkt(ez_dev_t *ezdev,struct sk_buff *pSkb)
{
	unsigned int recv_from = 0, band_from = 0;

	band_from = RTMP_GET_PACKET_BAND(pSkb); 
	recv_from = RTMP_GET_PACKET_RECV_FROM(pSkb);

	//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("BandFrom:0x%x, RecvdFrom: 0x%x\n",band_from,recv_from));
	if (*ezdev->channel > 14)
	{
			if((recv_from == RTMP_PACKET_RECV_FROM_2G_AP) ||
				(recv_from == RTMP_PACKET_RECV_FROM_2G_CLIENT)
			  )
			{
				//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Other band rcvd packet\n"));
				return TRUE;
			}
	}
	else
	{
		if( (recv_from == RTMP_PACKET_RECV_FROM_5G_AP) ||
			(recv_from == RTMP_PACKET_RECV_FROM_5G_H_AP) ||
			(recv_from == RTMP_PACKET_RECV_FROM_5G_CLIENT) ||
			(recv_from == RTMP_PACKET_RECV_FROM_5G_H_CLIENT)
		  )
		{
			//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Other band rcvd packet\n"));
			return TRUE;
		}
	}

	return FALSE;
}

 BOOLEAN is_other_band_cli_rcvd_pkt(ez_dev_t *ezdev,struct sk_buff *pSkb)
{
	unsigned int recv_from = 0, band_from = 0;

	band_from = RTMP_GET_PACKET_BAND(pSkb); 
	recv_from = RTMP_GET_PACKET_RECV_FROM(pSkb);

	//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("BandFrom:0x%x, RecvdFrom: 0x%x\n",band_from,recv_from));
	if (*ezdev->channel > 14)
	{
		if((recv_from == RTMP_PACKET_RECV_FROM_2G_CLIENT))
		{
			//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ApCliChannel(%d) : Other band CLI rcvd packet\n",*ezdev->channel));
			return TRUE;
		}
	}
	else
	{
		if( (recv_from == RTMP_PACKET_RECV_FROM_5G_CLIENT) ||
			(recv_from == RTMP_PACKET_RECV_FROM_5G_H_CLIENT)
		  )
		{
			//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ApCliChannel(%d) : Other band CLI rcvd packet\n",*ezdev->channel));
			return TRUE;
		}
	}

	return FALSE;
}

#endif /*CONFIG_WIFI_PKT_FWD */

void ez_apcli_uni_tx_on_dup_link(ez_dev_t *ezdev,struct sk_buff *pSkb)
{
	struct _ez_security *ez_sec_info = &ezdev->ez_security;
#if defined(CONFIG_WIFI_PKT_FWD) || defined(CONFIG_WIFI_PKT_FWD_MODULE)
	unsigned int recv_from = 0, band_from = 0;
#endif
	if(!ez_sec_info->this_band_info.shared_info.link_duplicate){
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_apcli_tx_group_packet_drop: Both ApCli not connected to same device \n"));
		return;
	}

#if defined(CONFIG_WIFI_PKT_FWD) || defined(CONFIG_WIFI_PKT_FWD_MODULE)
	band_from = RTMP_GET_PACKET_BAND(pSkb); 
	recv_from = RTMP_GET_PACKET_RECV_FROM(pSkb);

	if( (band_from==0) && (recv_from==0) && 
		( (*ezdev->channel >= H_CHANNEL_BIGGER_THAN) || (*ezdev->channel > 14) )){ // just chan 14 check should be enough but kept as in other places
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_apcli_uni_tx_on_dup_link: UNICAST PKT ON NON2G DUP LINK !!!\n"));
	}

#endif

}

/* Set/Clear Loop chk context on other band CLI */
void ez_set_other_band_cli_loop_chk_info(EZ_ADAPTER *ez_ad, ez_dev_t * ezdev, BOOLEAN test_start)
{	
	//PRTMP_ADAPTER ad = (PRTMP_ADAPTER)ez_ad;
	ez_dev_t  *other_band_ezdev = ez_get_otherband_cli_ezdev(ezdev);
	struct _ez_security *ez_sec_info = NULL;

	if(other_band_ezdev) {
		ez_sec_info = &other_band_ezdev->ez_security;
	} else {
		return;
	}
	if(test_start){ // can only be done by source
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("ez_set_other_band_cli_loop_chk_info: Set Dest role for other CLI\n"));
	    ez_sec_info->loop_chk_info.loop_chk_role = DEST;
		ez_sec_info->first_loop_check = FALSE;
	    NdisCopyMemory(ez_sec_info->loop_chk_info.source_mac,ezdev->ez_security.this_band_info.shared_info.cli_mac_addr,MAC_ADDR_LEN);
	}
	else{// common handling irrespective of role
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("ez_set_other_band_cli_loop_chk_info: Clear Loop Chk role for other CLI\n"));
		NdisZeroMemory(&ez_sec_info->loop_chk_info,sizeof(LOOP_CHK_INFO));
		ez_sec_info->dest_loop_detect = FALSE;

		other_band_ezdev->driver_ops->ez_cancel_timer(other_band_ezdev, ez_sec_info->ez_loop_chk_timer);
	}
}

/* Terminate Loop ck*/
void ez_cancel_loop_chk(ez_dev_t * ezdev)
{	
	if(ezdev->ez_security.loop_chk_info.loop_chk_role == SOURCE){
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Source clear loop chk\n"));
		ezdev->driver_ops->ez_cancel_timer(ezdev, ezdev->ez_security.ez_loop_chk_timer);

		ezdev->ez_security.loop_chk_info.loop_chk_role = NONE; // or zero out struct as below
	    ezdev->ez_security.first_loop_check = FALSE;
		ezdev->ez_security.dest_loop_detect = FALSE;
		ez_set_other_band_cli_loop_chk_info(ezdev->ez_ad,ezdev,FALSE);
	}

	if(ezdev->ez_security.loop_chk_info.loop_chk_role == DEST){
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Dest clear loop chk\n"));
		NdisZeroMemory(&ezdev->ez_security.loop_chk_info,sizeof(LOOP_CHK_INFO));
		ezdev->ez_security.dest_loop_detect = FALSE;
		ez_set_other_band_cli_loop_chk_info(ezdev->ez_ad,ezdev,FALSE);
	}

}

INT Set_EasySetup_LoopPktSend(
	EZ_ADAPTER* ez_ad,
	RTMP_STRING *arg)
{
	// Todo: pending
	return TRUE;

}

/* Trigger Loop check process when both CLI connected to non-easy root APs*/
void ez_chk_loop_thru_non_ez_ap(EZ_ADAPTER *ez_ad, ez_dev_t *ezdev)
{
    unsigned char  other_cli_mac[MAC_ADDR_LEN]={0};
	interface_info_t other_band_info;

    NdisZeroMemory(&other_band_info,sizeof(interface_info_t));

	//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_chk_loop_thru_non_ez_ap: ezdevtype=0x%x, func_idx=0x%x\n",pEntry->ezdev->ezdev_type,pEntry->ezdev->func_idx));

	ez_get_other_band_info(ezdev, &other_band_info);

	//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Other band Info=> Activated:%x, DuplicateLink:%x, NoneasyConnection:%x\n",
	//	other_band_info.interface_activated,
	//	other_band_info.shared_info.link_duplicate,
	//	other_band_info.non_easy_connection));

	if ( !other_band_info.interface_activated )
	{
	    //EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Other band Cli not yet activated \n"));
		return;
	}

	if(ezdev->ez_security.loop_chk_info.loop_chk_role != SOURCE){


	    ezdev->ez_security.loop_chk_info.loop_chk_role = SOURCE;

		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Loop chk trigger as source role\n"));

		ez_set_other_band_cli_loop_chk_info(ez_ad,ezdev,TRUE);
	}

    //trigger loop pkt
  	ezdev->ez_security.dest_loop_detect = FALSE;

	NdisCopyMemory(other_cli_mac,other_band_info.shared_info.cli_mac_addr,MAC_ADDR_LEN);
	//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("ez_chk_loop_thru_non_ez_ap=> Add in payload Other CLi MAC: %02x-%02x-%02x-%02x-%02x-%02x\n",
	//	other_cli_mac[0],other_cli_mac[1],other_cli_mac[2],other_cli_mac[3],other_cli_mac[4],other_cli_mac[5]));

	//start timer
	if(ezdev->ez_security.first_loop_check == TRUE){
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Start loop detect timeout of 5 seconds\n"));
		ezdev->driver_ops->ez_set_timer(ezdev, ezdev->ez_security.ez_loop_chk_timer, EZ_LOOP_CHK_TIMEOUT_5S);
	}else{
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Start loop detect timeout of 10 seconds\n"));
		ezdev->driver_ops->ez_set_timer(ezdev, ezdev->ez_security.ez_loop_chk_timer, EZ_LOOP_CHK_TIMEOUT_10S);
	}
	//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Start timeout for %d sec\n",EZ_LOOP_CHK_TIMEOUT/1000));

	ezdev->driver_ops->ez_send_loop_detect_pkt(ezdev, other_cli_mac);

}

/* Mark duplicate link and clear Loop chk context*/
 void ez_inform_other_band_cli_loop_detect( ez_dev_t * ezdev)
{	
	
	//PRTMP_ADAPTER ad = (PRTMP_ADAPTER)ez_ad;
	ez_dev_t  *other_band_ezdev = ez_get_otherband_cli_ezdev(ezdev);
	struct _ez_security *ez_sec_info = NULL;

	if(other_band_ezdev->wdev) {
		ez_sec_info = &other_band_ezdev->ez_security;
	} else {
		return;
	}

	if(ez_sec_info->this_band_info.shared_info.link_duplicate != TRUE){
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Mark duplicate link on other band cli\n"));
		ez_sec_info->this_band_info.shared_info.link_duplicate = TRUE;
	}

	if(ez_sec_info->dest_loop_detect){
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,("Loopk Detect pkt rcvd by Dest when already loop detected\n"));
	}

	// Do this always as source is doing periodic loop check
	ez_sec_info->dest_loop_detect = TRUE;
}


BOOLEAN ez_get_band( ez_dev_t * ezdev)
{
	if (*ezdev->channel >14)
	{
		return TRUE;
	} else {
		return FALSE;
	}
}



//void ez_update_ssid_pmk(EZ_ADAPTER *ez_ad, char * ssid, unsigned char ssid_len, unsigned char *pmk, struct _ez_peer_security_info  *from_peer)
void ez_update_ssid_pmk(EZ_ADAPTER *ez_ad, char * ssid1, unsigned char ssid_len1, unsigned char *psk1, unsigned char *pmk1	, 
								char * ssid2, unsigned char ssid_len2, unsigned char *psk2, unsigned char *pmk2, struct _ez_peer_security_info  *from_peer)
{
	ez_dev_t *ezdev;
//! Levarage from MP1.0 CL 170192
	updated_configs_t *updated_config=NULL;
	NDIS_STATUS NStatus;

	NStatus = EZ_MEM_ALLOC(NULL, (UCHAR **)&updated_config, sizeof(updated_configs_t));
    if(NStatus != NDIS_STATUS_SUCCESS)
    {
            EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("%s() allocate memory failed \n", __FUNCTION__));
			ASSERT(FALSE);
            return;
    }
	NdisZeroMemory(updated_config, sizeof(updated_configs_t));
	ezdev = &ez_ad->ez_band_info[0].ap_ezdev;

	if((ez_ad->device_info.ez_node_number.path_len == MAC_ADDR_LEN) 
		|| ((ez_ad->ez_band_info[0].cli_ezdev.wdev != NULL) && ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.non_easy_connection)
		|| ((ez_ad->ez_band_info[1].cli_ezdev.wdev != NULL) && (ez_ad->ez_band_info[1].cli_ezdev.ez_security.this_band_info.non_easy_connection)))
	{
		ez_init_updated_configs_for_push(updated_config, ezdev);

		if(ez_ad->band_count == 1) {
			updated_config->this_band_info.shared_info.ssid_len = ssid_len1;
			NdisZeroMemory(updated_config->this_band_info.shared_info.ssid, EZ_MAX_LEN_OF_SSID);
			NdisCopyMemory(updated_config->this_band_info.shared_info.ssid ,ssid1, ssid_len1);
			NdisCopyMemory(updated_config->this_band_info.pmk, pmk1, EZ_PMK_LEN);
			NdisZeroMemory(updated_config->this_band_info.psk, EZ_LEN_PSK);
			NdisCopyMemory(updated_config->this_band_info.psk, psk1, strlen(psk1));
		} else {

			if((updated_config->this_band_info.shared_info.channel_info.channel <= 14)
				|| ((updated_config->this_band_info.shared_info.channel_info.channel > 14)
					&& (updated_config->other_band_info.shared_info.channel_info.channel > 14))) {
				updated_config->this_band_info.shared_info.ssid_len = ssid_len1;
				NdisCopyMemory(updated_config->this_band_info.shared_info.ssid ,ssid1, ssid_len1);
				NdisCopyMemory(updated_config->this_band_info.pmk, pmk1, EZ_PMK_LEN);
				NdisZeroMemory(updated_config->this_band_info.psk, EZ_LEN_PSK);
				NdisCopyMemory(updated_config->this_band_info.psk, psk1, strlen(psk1));
				
				if(ssid_len2)
				{
					updated_config->other_band_info.shared_info.ssid_len = ssid_len2;	
					NdisCopyMemory(updated_config->other_band_info.shared_info.ssid ,ssid2, ssid_len2);
					NdisCopyMemory(updated_config->other_band_info.pmk, pmk2, EZ_PMK_LEN);
					NdisZeroMemory(updated_config->other_band_info.psk, EZ_LEN_PSK);
					NdisCopyMemory(updated_config->other_band_info.psk, psk2, strlen(psk2));
				}
			} else {

				if(ssid_len2) {
					updated_config->this_band_info.shared_info.ssid_len = ssid_len2;
					NdisCopyMemory(updated_config->this_band_info.shared_info.ssid ,ssid2, ssid_len2);
					NdisCopyMemory(updated_config->this_band_info.pmk, pmk2, EZ_PMK_LEN);
					NdisZeroMemory(updated_config->this_band_info.psk, EZ_LEN_PSK);
					NdisCopyMemory(updated_config->this_band_info.psk, psk2, strlen(psk2));
				}
				if(ssid_len1)
				{
					updated_config->other_band_info.shared_info.ssid_len = ssid_len1;	
					NdisCopyMemory(updated_config->other_band_info.shared_info.ssid ,ssid1, ssid_len1);
					NdisCopyMemory(updated_config->other_band_info.pmk, pmk1, EZ_PMK_LEN);
					NdisZeroMemory(updated_config->other_band_info.psk, EZ_LEN_PSK);
					NdisCopyMemory(updated_config->other_band_info.psk, psk1, strlen(psk1));

				}
			}
		}

//! Levarage from MP1.0 CL 170192
		updated_config->device_info.network_weight[0] |= BIT(7);
		updated_config->device_info.network_weight[0] |= BIT(5); 		

#ifdef CONFIG_PUSH_VER_SUPPORT
		if(ez_ad->device_info.network_weight[NETWORK_WEIGHT_LEN - 1] < 255) {

			ez_ad->device_info.network_weight[NETWORK_WEIGHT_LEN - 1] += 1;
			updated_config->device_info.network_weight[NETWORK_WEIGHT_LEN - 1] += 1;

			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("network_weight[7] %d\n", ez_ad->device_info.network_weight[NETWORK_WEIGHT_LEN - 1]));
		} else {
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("ERROR : network weight[7] config push reached limit(255) \n"));
		}
#endif		
		printk("This Band Channel = %d\n", updated_config->this_band_info.shared_info.channel_info.channel);
		printk("Other Band Channel = %d\n", updated_config->other_band_info.shared_info.channel_info.channel);

		push_and_update_ap_config(ez_ad, ezdev, updated_config, FALSE);
		push_and_update_cli_config(ez_ad, ezdev, updated_config, FALSE);
		ezdev->driver_ops->UpdateBeaconHandler(ezdev, IE_CHANGE);

		if (ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.non_easy_connection)
		{
			ez_ad->ez_band_info[0].cli_ezdev.driver_ops->ez_send_unicast_deauth
				(&ez_ad->ez_band_info[0].cli_ezdev, ez_ad->ez_band_info[0].cli_ezdev.bssid);
			ez_ad->ez_band_info[0].cli_ezdev.ez_security.disconnect_by_ssid_update = TRUE;
		}
		if (&ez_ad->ez_band_info[1].cli_ezdev)
		{
			if (ez_ad->ez_band_info[1].cli_ezdev.ez_security.this_band_info.non_easy_connection)
			{
				ez_ad->ez_band_info[1].cli_ezdev.driver_ops->ez_send_unicast_deauth
					(&ez_ad->ez_band_info[1].cli_ezdev, ez_ad->ez_band_info[1].cli_ezdev.bssid);
				ez_ad->ez_band_info[1].cli_ezdev.ez_security.disconnect_by_ssid_update = TRUE;
			}
		}
	} 
	else 
	{
		//ez_send_update_ssid_pmk(ez_ad, ssid, ssid_len, pmk, from_peer);
	}
//! Levarage from MP1.0 CL 170192
	if (updated_config)
		EZ_MEM_FREE(updated_config);
}
/* we assume the s1 and s2 both are strings.*/

#define ASC_LOWER(_x)	((((_x) >= 0x41) && ((_x) <= 0x5a)) ? (_x) + 0x20 : (_x))

BOOLEAN ezstrcasecmp(RTMP_STRING *s1, RTMP_STRING *s2)
{
	RTMP_STRING *p1 = s1, *p2 = s2;
	CHAR c1, c2;

	if (strlen(s1) != strlen(s2))
		return FALSE;

	while(*p1 != '\0')
	{
		c1 = ASC_LOWER(*p1);
		c2 = ASC_LOWER(*p2);
		if(c1 != c2)
			return FALSE;
		p1++;
		p2++;
	}

	return TRUE;
}

VOID ez_setWdevAuthMode (
    struct __ez_triband_sec_config *pSecConfig, 
    RTMP_STRING *arg)
{
    UINT32 AKMMap = 0;
		
    CLEAR_SEC_AKM(AKMMap);

    if (ezstrcasecmp(arg, "OPEN") == TRUE)
    {
        SET_AKM_OPEN(AKMMap);
    }
    else if (ezstrcasecmp(arg, "SHARED") == TRUE)
    {
        SET_AKM_SHARED(AKMMap);
    }
    else if (ezstrcasecmp(arg, "WEPAUTO") == TRUE) 
    {
        SET_AKM_OPEN(AKMMap);
	    SET_AKM_AUTOSWITCH(AKMMap);
    }
    else if (ezstrcasecmp(arg, "WPA") == TRUE)
    {
        SET_AKM_WPA1(AKMMap);
    }
    else if (ezstrcasecmp(arg, "WPAPSK") == TRUE)
    {
        SET_AKM_WPA1PSK(AKMMap);
    }
    else if (ezstrcasecmp(arg, "WPANONE") == TRUE)
    {
        SET_AKM_WPANONE(AKMMap);
    }
    else if (ezstrcasecmp(arg, "WPA2") == TRUE)
    {
        SET_AKM_WPA2(AKMMap);
    }
    else if (ezstrcasecmp(arg, "WPA2PSK") == TRUE)
    {
        SET_AKM_WPA2PSK(AKMMap);
    }
    else if (ezstrcasecmp(arg, "WPA1WPA2") == TRUE) 
    {
        SET_AKM_WPA1(AKMMap);
        SET_AKM_WPA2(AKMMap);
    }
    else if (ezstrcasecmp(arg, "WPAPSKWPA2PSK") == TRUE) 
    {
        SET_AKM_WPA1PSK(AKMMap);
        SET_AKM_WPA2PSK(AKMMap);
    }
    else if ((ezstrcasecmp(arg, "WPA_AES_WPA2_TKIPAES") == TRUE) 
                || (ezstrcasecmp(arg, "WPA_AES_WPA2_TKIP") == TRUE)
                || (ezstrcasecmp(arg, "WPA_TKIP_WPA2_AES") == TRUE)
                || (ezstrcasecmp(arg, "WPA_TKIP_WPA2_TKIPAES") == TRUE)
                || (ezstrcasecmp(arg, "WPA_TKIPAES_WPA2_AES") == TRUE)
                || (ezstrcasecmp(arg, "WPA_TKIPAES_WPA2_TKIPAES") == TRUE)
                || (ezstrcasecmp(arg, "WPA_TKIPAES_WPA2_TKIP") == TRUE))
    {
        SET_AKM_WPA1PSK(AKMMap);
        SET_AKM_WPA2PSK(AKMMap);
    }
#ifdef WAPI_SUPPORT
    else if (ezstrcasecmp(arg, "WAICERT") == TRUE)
    {
        SET_AKM_WAICERT(AKMMap);
    }
    else if (ezstrcasecmp(arg, "WAIPSK") == TRUE)
    {
        SET_AKM_WPIPSK(AKMMap);
    }
#endif /* WAPI_SUPPORT */
    else
    {
        EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("%s:: Not support (AuthMode=%s, len=%d)\n",
            __FUNCTION__, arg, (int) strlen(arg)));
    }

    if (AKMMap != 0x0)
        pSecConfig->AKMMap = AKMMap;

    EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("%s::AuthMode=0x%x\n",
            __FUNCTION__, pSecConfig->AKMMap));
}



VOID ez_setWdevEncrypMode (
    struct __ez_triband_sec_config *pSecConfig, 
    RTMP_STRING *arg)
{
    UINT Cipher = 0;
	
    if (ezstrcasecmp(arg, "NONE") == TRUE)
    {
        SET_CIPHER_NONE(Cipher);
    }
    else if (ezstrcasecmp(arg, "WEP") == TRUE)
    {
        SET_CIPHER_WEP(Cipher);
    }
    else if (ezstrcasecmp(arg, "TKIP") == TRUE) 
    {
        SET_CIPHER_TKIP(Cipher);
    }
    else if ((ezstrcasecmp(arg, "AES") == TRUE) || (ezstrcasecmp(arg, "CCMP128") == TRUE))
    {
        SET_CIPHER_CCMP128(Cipher);
    }
    else if (ezstrcasecmp(arg, "CCMP256") == TRUE)
    {
        SET_CIPHER_CCMP256(Cipher);
    }
    else if (ezstrcasecmp(arg, "GCMP128") == TRUE)
    {
        SET_CIPHER_GCMP128(Cipher);
    }
    else if (ezstrcasecmp(arg, "GCMP256") == TRUE)
    {
        SET_CIPHER_GCMP256(Cipher);
    }
    else if ((ezstrcasecmp(arg, "TKIPAES") == TRUE) || (ezstrcasecmp(arg, "TKIPCCMP128") == TRUE))
    {
        SET_CIPHER_TKIP(Cipher);
        SET_CIPHER_CCMP128(Cipher);
    }
    else if ((ezstrcasecmp(arg, "WPA_AES_WPA2_TKIPAES") == TRUE) 
                || (ezstrcasecmp(arg, "WPA_AES_WPA2_TKIP") == TRUE)
                || (ezstrcasecmp(arg, "WPA_TKIP_WPA2_AES") == TRUE)
                || (ezstrcasecmp(arg, "WPA_TKIP_WPA2_TKIPAES") == TRUE)
                || (ezstrcasecmp(arg, "WPA_TKIPAES_WPA2_AES") == TRUE)
                || (ezstrcasecmp(arg, "WPA_TKIPAES_WPA2_TKIPAES") == TRUE)
                || (ezstrcasecmp(arg, "WPA_TKIPAES_WPA2_TKIP") == TRUE))
    {
        SET_CIPHER_TKIP(Cipher);
        SET_CIPHER_CCMP128(Cipher);
    }
#ifdef WAPI_SUPPORT
    else if (ezstrcasecmp(arg, "SMS4") == TRUE)
    {
        SET_CIPHER_WPI_SMS4(Cipher);
    }
#endif /* WAPI_SUPPORT */
    else
    {
        EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("%s:: Not support (EncrypType=%s, len=%d)\n",
                __FUNCTION__, arg, (int) strlen(arg)));
    }

    if (Cipher != 0x0)
    {
        pSecConfig->PairwiseCipher = Cipher;
        CLEAR_GROUP_CIPHER(pSecConfig);
    }

    EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("%s::PairwiseCipher=0x%x\n",
            __FUNCTION__, GET_PAIRWISE_CIPHER(pSecConfig)));
}

//void ez_update_ssid_pmk(RTMP_ADAPTER *pAd, char * ssid, unsigned char ssid_len, unsigned char *pmk, struct _ez_peer_security_info  *from_peer)
void ez_update_triband_ssid_pmk(EZ_ADAPTER *ez_ad, char * ssid1, unsigned char ssid_len1, unsigned char *pmk1	, unsigned char *psk1,
								char * ssid2, unsigned char ssid_len2, unsigned char *pmk2, unsigned char *psk2,
								char * ssid3, unsigned char ssid_len3, unsigned char *pmk3, unsigned char *psk3,
								char * encryptype1, char *encryptype2,	char * authmode1, char *authmode2)
{
	ez_dev_t  *ezdev;
	updated_configs_t *updated_config = NULL;
	NDIS_STATUS NStatus;

//! Levarage from MP1.0 CL 170192
	NStatus = EZ_MEM_ALLOC(NULL, (UCHAR **)&updated_config, 
		sizeof(updated_configs_t));
    if(NStatus != NDIS_STATUS_SUCCESS)
    {
        EZ_DEBUG(DBG_CAT_MLME, DBG_SUBCAT_ALL, DBG_LVL_OFF,("%s() allocate memory failed \n", __FUNCTION__));
		ASSERT(FALSE);
        return;
    }

	NdisZeroMemory(updated_config, sizeof(updated_configs_t));
	ezdev = &ez_ad->ez_band_info[0].ap_ezdev;

	if((ez_ad->device_info.ez_node_number.path_len == MAC_ADDR_LEN) 
		|| ((ez_ad->ez_band_info[0].cli_ezdev.wdev != NULL) && ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.non_easy_connection))
	{

		if (!ssid_len2)
		{
			ssid_len2 = ssid_len1;
			NdisCopyMemory(ssid2,ssid1,ssid_len1);			
			NdisCopyMemory(psk2,psk1,strlen(psk1));						
			NdisCopyMemory(pmk2,pmk1,EZ_PMK_LEN);			
		}

		if (!ssid_len3)
		{
			ssid_len3 = ssid_len2;
			NdisCopyMemory(ssid3,ssid2,ssid_len2);
			NdisCopyMemory(psk3,psk2,strlen(psk2));			
			NdisCopyMemory(pmk3,pmk2,EZ_PMK_LEN);			
		}

		ezdev->ez_security.this_band_info.shared_info.ssid_len = ssid_len1;
		NdisZeroMemory(ezdev->ez_security.this_band_info.shared_info.ssid, MAX_LEN_OF_SSID);
		NdisCopyMemory(ezdev->ez_security.this_band_info.shared_info.ssid, ssid1,ssid_len1);
		NdisCopyMemory(ezdev->ez_security.this_band_info.pmk, pmk1,EZ_PMK_LEN);


		if((*ez_ad->non_ez_band_info[0].channel <= 14) 
			||((*ez_ad->non_ez_band_info[0].channel > 14)
					&& (*ez_ad->non_ez_band_info[1].channel > 14))) {

			ez_ad->non_ez_band_info[0].ssid_len = ssid_len2;
			NdisZeroMemory(ez_ad->non_ez_band_info[0].ssid, MAX_LEN_OF_SSID);
			NdisCopyMemory(ez_ad->non_ez_band_info[0].ssid, ssid2,ssid_len2);
			NdisCopyMemory(ez_ad->non_ez_band_info[0].pmk, pmk2,EZ_PMK_LEN);
			NdisZeroMemory(ez_ad->non_ez_band_info[0].psk, LEN_PSK);
			NdisCopyMemory(ez_ad->non_ez_band_info[0].psk, psk2, strlen(psk2));
			ez_setWdevEncrypMode(&ez_ad->non_ez_band_info[0].triband_sec, encryptype1);
			ez_setWdevAuthMode(&ez_ad->non_ez_band_info[0].triband_sec, authmode1);
			
			ez_ad->non_ez_band_info[1].ssid_len = ssid_len3;
			NdisZeroMemory(ez_ad->non_ez_band_info[1].ssid, MAX_LEN_OF_SSID);
			NdisCopyMemory(ez_ad->non_ez_band_info[1].ssid, ssid3,ssid_len3);
			NdisCopyMemory(ez_ad->non_ez_band_info[1].pmk, pmk3,EZ_PMK_LEN);
			NdisZeroMemory(ez_ad->non_ez_band_info[1].psk, LEN_PSK);
			NdisCopyMemory(ez_ad->non_ez_band_info[1].psk, psk3, strlen(psk3));
			ez_setWdevEncrypMode(&ez_ad->non_ez_band_info[1].triband_sec, encryptype2);
			ez_setWdevAuthMode(&ez_ad->non_ez_band_info[1].triband_sec, authmode2);
		} else {
			ez_ad->non_ez_band_info[1].ssid_len = ssid_len2;
			NdisZeroMemory(ez_ad->non_ez_band_info[1].ssid, MAX_LEN_OF_SSID);
			NdisCopyMemory(ez_ad->non_ez_band_info[1].ssid, ssid2,ssid_len2);
			NdisCopyMemory(ez_ad->non_ez_band_info[1].pmk, pmk2,EZ_PMK_LEN);
			NdisZeroMemory(ez_ad->non_ez_band_info[1].psk, LEN_PSK);
			NdisCopyMemory(ez_ad->non_ez_band_info[1].psk, psk2, strlen(psk2));
			ez_setWdevEncrypMode(&ez_ad->non_ez_band_info[1].triband_sec, encryptype1);
			ez_setWdevAuthMode(&ez_ad->non_ez_band_info[1].triband_sec, authmode1);

			ez_ad->non_ez_band_info[0].ssid_len = ssid_len3;
			NdisZeroMemory(ez_ad->non_ez_band_info[0].ssid, MAX_LEN_OF_SSID);
			NdisCopyMemory(ez_ad->non_ez_band_info[0].ssid, ssid3,ssid_len3);
			NdisCopyMemory(ez_ad->non_ez_band_info[0].pmk, pmk3,EZ_PMK_LEN);
			NdisZeroMemory(ez_ad->non_ez_band_info[0].psk, LEN_PSK);
			NdisCopyMemory(ez_ad->non_ez_band_info[0].psk, psk3, strlen(psk3));
			ez_setWdevEncrypMode(&ez_ad->non_ez_band_info[0].triband_sec, encryptype2);
			ez_setWdevAuthMode(&ez_ad->non_ez_band_info[0].triband_sec, authmode2);
		}
		ez_ad->non_ez_band_info[0].need_restart = TRUE;
		ez_ad->non_ez_band_info[1].need_restart = TRUE;		

		ez_init_updated_configs_for_push(updated_config, ezdev);

		updated_config->need_ez_update = TRUE;
		updated_config->need_non_ez_update_psk[0] = TRUE;
		updated_config->need_non_ez_update_psk[1] = TRUE;
		updated_config->need_non_ez_update_ssid[0] = TRUE;
		updated_config->need_non_ez_update_ssid[1] = TRUE;
		updated_config->need_non_ez_update_secconfig[0] = TRUE;
		updated_config->need_non_ez_update_secconfig[1] = TRUE;
		
		updated_config->device_info.network_weight[0] |= BIT(7); 		
		updated_config->device_info.network_weight[0] |= BIT(5); 		
				
//! Leverage form MP.1.0 CL 170364
#ifdef CONFIG_PUSH_VER_SUPPORT
		if(ez_ad->device_info.network_weight[NETWORK_WEIGHT_LEN - 1] < 255) {

			ez_ad->device_info.network_weight[NETWORK_WEIGHT_LEN - 1] += 1;
			updated_config->device_info.network_weight[NETWORK_WEIGHT_LEN - 1] += 1;

			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("network_weight[7] %d\n", ez_adapter->device_info.network_weight[NETWORK_WEIGHT_LEN - 1]));
		} else {
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("ERROR : network weight[7] config push reached limit(255) \n"));
		}
#endif		

		push_and_update_ap_config(ez_ad, ezdev, updated_config, FALSE);
		push_and_update_cli_config(ez_ad, ezdev, updated_config, FALSE);
		ezdev->driver_ops->UpdateBeaconHandler
		(ezdev, IE_CHANGE);
		if (ez_ad->ez_band_info[0].cli_ezdev.ez_security.this_band_info.non_easy_connection)
		{
			ezdev->driver_ops->ez_send_unicast_deauth(ezdev, ez_ad->ez_band_info[0].cli_ezdev.bssid);
			
			ez_ad->ez_band_info[0].cli_ezdev.ez_security.disconnect_by_ssid_update = TRUE;
		}
		
	} 
	else 
	{
	}
//! Levarage from MP1.0 CL 170192
		EZ_MEM_FREE(updated_config);
}



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
 *		EzMakeOutgoingFrame(Buffer, output_length, 2, &fc, 2, &dur, 6, p_addr1, 6,p_addr2, END_OF_ARGS);

 IRQL = PASSIVE_LEVEL
 IRQL = DISPATCH_LEVEL

 ****************************************************************************/
ULONG EzMakeOutgoingFrame(UCHAR *Buffer, ULONG *FrameLen, ...)
{
	UCHAR   *p;
	int 	leng;
	ULONG	TotLeng;
	va_list Args;

	/* calculates the total length*/
	TotLeng = 0;
	va_start(Args, FrameLen);
	do
	{
		leng = va_arg(Args, int);
		if (leng == END_OF_ARGS)
		{
			break;
		}
		p = va_arg(Args, PVOID);
		NdisMoveMemory(&Buffer[TotLeng], p, leng);
		TotLeng = TotLeng + leng;
	} while(TRUE);

	va_end(Args); /* clean up */
	*FrameLen = TotLeng;
	return TotLeng;
}

/*Unify Utility APIs*/
INT ez_os_alloc_mem(
	VOID *pAd,
	UCHAR **mem,
	ULONG size)
{
	*mem = (PUCHAR) kmalloc(size, GFP_ATOMIC);
	if (*mem) {
#if 0
#ifdef VENDOR_FEATURE4_SUPPORT
		OS_NumOfMemAlloc++;
#endif /* VENDOR_FEATURE4_SUPPORT */

#ifdef MEM_ALLOC_INFO_SUPPORT
		MIListAddHead(&MemInfoList, size, *mem, __builtin_return_address(0));
#endif /* MEM_ALLOC_INFO_SUPPORT */
#endif
		return NDIS_STATUS_SUCCESS;
	} else
		return NDIS_STATUS_FAILURE;

}

//#error check porting of malloc!!!
VOID ez_os_free_mem(
	PVOID mem)
{
#ifdef MEM_ALLOC_INFO_SUPPORT
	MEM_INFO_LIST_ENTRY *delEntry;
	delEntry = MIListRemove(&MemInfoList, mem);
	if(delEntry == NULL)
	{
		EZ_DEBUG(DBG_CAT_INIT, DBG_SUBCAT_ALL, DBG_LVL_ERROR,
			 ("the memory has not been allocated\n"));
		EZ_DEBUG(DBG_CAT_INIT, DBG_SUBCAT_ALL, DBG_LVL_ERROR,
			 ("mem addr = %p, caller is %pS\n", mem, __builtin_return_address(0)));
		dump_stack();
	}
	else
#endif /* MEM_ALLOC_INFO_SUPPORT */
	{
		ASSERT(mem);
		kfree(mem);
	}
#ifdef VENDOR_FEATURE4_SUPPORT
	OS_NumOfMemFree++;
#endif /* VENDOR_FEATURE4_SUPPORT */

}


RTMP_STRING *EzGetAuthModeStr (
   UINT32 authMode)
{
    if (IS_AKM_OPEN(authMode))
        return "OPEN";
    else if (IS_AKM_SHARED(authMode))
        return "SHARED";
    else if (IS_AKM_AUTOSWITCH(authMode))
        return "WEPAUTO";
    else if (IS_AKM_WPANONE(authMode))
        return "WPANONE";
    else if (IS_AKM_WPA1(authMode) && IS_AKM_WPA2(authMode))
        return "WPA1WPA2";
    else if (IS_AKM_WPA1PSK(authMode) && IS_AKM_WPA2PSK(authMode))
        return "WPAPSKWPA2PSK";
    else if (IS_AKM_WPA1(authMode))
        return "WPA";
    else if (IS_AKM_WPA1PSK(authMode))
        return "WPAPSK";
    else if (IS_AKM_WPA2(authMode))
        return "WPA2";
    else if (IS_AKM_WPA2PSK(authMode))
        return "WPA2PSK";
    else
        return "UNKNOW";
}

RTMP_STRING *EzGetEncryModeStr(
    UINT32 encryMode)
{
    if (IS_CIPHER_NONE(encryMode))
        return "NONE";
    else if (IS_CIPHER_WEP(encryMode))
        return "WEP";
    else if (IS_CIPHER_TKIP(encryMode) && IS_CIPHER_CCMP128(encryMode))
        return "TKIPAES";
    else if (IS_CIPHER_TKIP(encryMode))
        return "TKIP";
    else if (IS_CIPHER_CCMP128(encryMode))
        return "AES";
    else if (IS_CIPHER_CCMP256(encryMode))
        return "CCMP256";
    else if (IS_CIPHER_GCMP128(encryMode))
        return "GCMP128";
    else if (IS_CIPHER_GCMP256(encryMode))
        return "GCMP256";
    else
        return "UNKNOW";
}


void NonEzRtmpOSWrielessEventSend(
	void *ad_obj,
	int band_id,
	UINT32 eventType,
	INT flags,
	PUCHAR pSrcMac,
	PUCHAR pData,
	UINT32 dataLen)
{
	EZ_ADAPTER *ad = ad_obj;
	ad->non_ez_band_info[band_id].lut_driver_ops.RtmpOSWrielessEventSendExt(ad->non_ez_band_info[band_id].pAd, band_id, eventType, flags, pSrcMac, pData, dataLen);
}

void EzRtmpOSWrielessEventSend(
	ez_dev_t * ezdev,
	UINT32 eventType,
	INT flags,
	PUCHAR pSrcMac,
	PUCHAR pData,
	UINT32 dataLen)
{
	ezdev->driver_ops->RtmpOSWrielessEventSendExt(ezdev, eventType, flags, pSrcMac, pData, dataLen);
}



VOID EzActHeaderInit(
   	PHEADER_802_11 pHdr80211,
    UCHAR *da,
    UCHAR *sa,
    UCHAR *bssid)
{
    NdisZeroMemory(pHdr80211, sizeof(HEADER_802_11));
	pHdr80211->FC.Type = FC_TYPE_MGMT;
    pHdr80211->FC.SubType = SUBTYPE_ACTION;

	COPY_MAC_ADDR(pHdr80211->Addr1, da);
	COPY_MAC_ADDR(pHdr80211->Addr2, sa);
	COPY_MAC_ADDR(pHdr80211->Addr3, bssid);
}

void EzStartGroupMergeTimer(ez_dev_t* ezdev)
{
	ezdev->driver_ops->ez_set_timer(ezdev, ezdev->ez_security.ez_group_merge_timer, EZ_GROUP_MERGE_TIMEOUT);
}

void ez_init_triband_config(void)
{
	int band_count = 0;
//! Levarage from MP1.0 CL 170192
	NDIS_STATUS NStatus;
	updated_configs_t *updated_config = NULL;
	NStatus = EZ_MEM_ALLOC(NULL, (UCHAR **)&updated_config, 
		sizeof(updated_configs_t));

    if(NStatus != NDIS_STATUS_SUCCESS)
    {
	    EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("%s() allocate memory failed \n", __FUNCTION__));
		ASSERT(FALSE);
        return;
    }
	 
//! Levarage from MP1.0 CL 170192
	ez_init_updated_configs_for_push(updated_config, 
		&ez_adapter->ez_band_info[0].ap_ezdev);
	ez_adapter->ez_band_info[0].lut_driver_ops.set_ap_ssid_null(&ez_adapter->ez_band_info[0].ap_ezdev);
	ez_adapter->ez_band_info[0].lut_driver_ops.UpdateBeaconHandler(&ez_adapter->ez_band_info[0].ap_ezdev, IE_CHANGE);

	for (band_count = 0; band_count < MAX_NON_EZ_BANDS; band_count++)
	{
		if (ez_adapter->non_ez_band_info[band_count].pAd == NULL)
		{
			continue;
		}
		ez_adapter->non_ez_band_info[band_count].lut_driver_ops.ez_init_non_ez_ap(ez_adapter->non_ez_band_info[band_count].pAd,
			ez_adapter->non_ez_band_info[band_count].non_ez_ap_wdev, &ez_adapter->non_ez_band_info[band_count]);

	
	}
//! Levarage from MP1.0 CL 170192
	EZ_MEM_FREE(updated_config);	

}

void ez_timer_init(ez_dev_t *ezdev, void *timer, void *callback)
{
	ezdev->driver_ops->timer_init(ezdev, timer, callback);
}




/*this function is called to push own weight to all connected devices.*/
void update_and_push_weight(ez_dev_t *ezdev, char *peer_mac, unsigned char * network_weight)
{
//! Levarage from MP1.0 CL 170192
	NDIS_STATUS NStatus;
	//struct _ez_security *ez_sec_info = &ezdev->ez_security;
	BOOLEAN action_sent = FALSE;
	int irq_flags;
//! Levarage from MP1.0 CL 170192
	updated_configs_t *updated_configs = NULL;			
	channel_info_t current_chan_info;
	ez_dev_t  *ap_ezdev = NULL;
	BOOLEAN this_band_changed = FALSE;
	ez_dev_t *ezdev_2p4;
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;

	ezdev_2p4 = &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev;
//! Levarage from MP1.0 CL 170192
	NStatus = EZ_MEM_ALLOC(NULL, (UCHAR **)&updated_configs, sizeof(updated_configs_t));
   	if(NStatus != NDIS_STATUS_SUCCESS)
        {
               	EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("%s() allocate memory failed \n", __FUNCTION__));
				ASSERT(FALSE);
                return;
        }

	if (ez_get_band(ezdev_2p4)){
		if (ez_get_otherband_ezdev(ezdev_2p4) != NULL){
			ezdev_2p4 = ez_get_otherband_ezdev(ezdev_2p4);
		}
	}

	ez_update_connection_permission_hook(ezdev,EZ_DISALLOW_ALL);
	//! run loop for all active AP interfaces
	if (peer_mac == NULL){
		//! Weight providing link gone, assign self weight.
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("WDL Gone, assign self params\n"));
		NdisZeroMemory(ez_ad->device_info.weight_defining_link.peer_mac, MAC_ADDR_LEN);
		NdisZeroMemory(ez_ad->device_info.weight_defining_link.peer_ap_mac, MAC_ADDR_LEN);
		NdisCopyMemory(&ez_ad->device_info.network_weight[1], ezdev_2p4->if_addr,MAC_ADDR_LEN);	
		ez_ad->device_info.weight_defining_link.ezdev = NULL;
#ifdef EZ_DFS_SUPPORT
		if (ez_ad->dedicated_man_ap) {
#else
		if (ezdev->ez_security.user_configured) {
#endif
			ez_ad->device_info.network_weight[0] = 0xF;
		} else 
#ifdef WEIGHT_DEPENDS_ON_INET
		if (ezdev->ez_security.go_internet) {
			ez_ad->device_info.network_weight[0] = 0xE;
		} else 
#endif
		{
			ez_ad->device_info.network_weight[0] = 0x0;

		}

		ez_allocate_node_number(&ez_ad->device_info.ez_node_number,ezdev);

	} else {

		
		ap_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev;

		ezdev->ez_security.keep_finding_provider = FALSE;

		//! required to update SSID PMK???
#if 0		
		NdisCopyMemory(ezdev->ez_security.this_band_info.shared_info.ssid,apcli_entry->Ssid,apcli_entry->SsidLen);
		ezdev->ez_security.this_band_info.shared_info.ssid_len = apcli_entry->SsidLen;
		NdisCopyMemory(ezdev->ez_security.this_band_info.pmk, pEntry->ez_security.this_band_info.pmk,EZ_PMK_LEN);
#endif
		ezdev->driver_ops->ez_cancel_timer(ezdev, ezdev->ez_security.ez_scan_timer);
		//ezdev->driver_ops->ez_cancel_timer(ezdev, ezdev->ez_security.ez_stop_scan_timer);

		EZ_IRQ_LOCK(&ezdev->ez_security.ez_scan_pause_timer_lock, irq_flags);
		ezdev->driver_ops->ez_cancel_timer(ezdev, ezdev->ez_security.ez_scan_pause_timer);
		ezdev->driver_ops->apcli_stop_auto_connect(ezdev, FALSE);
		EZ_IRQ_UNLOCK(&ezdev->ez_security.ez_scan_pause_timer_lock, irq_flags);


		// 08/2016 : Rakesh: when channel switch happens on A band, currently many events are seen which delay other Rx
		// causing Link loss. Workaround to delay linkloss
		ezdev->ez_security.delay_disconnect_count = ez_ad->ez_delay_disconnect_count;
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Delay linkloss detection during Third party config adapt\n"));
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("1 update & push index = %d, delay_disconnect_count=%d\n",
									ezdev->ez_band_idx, ezdev->ez_security.delay_disconnect_count));

		//! I am taking configuration from this peer so this becomes my weight providing link.
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Updating to WDL\n"));
		NdisCopyMemory(ez_ad->device_info.weight_defining_link.peer_mac, peer_mac, MAC_ADDR_LEN);
		NdisCopyMemory(ez_ad->device_info.weight_defining_link.peer_ap_mac, peer_mac, MAC_ADDR_LEN);
		NdisCopyMemory(&ez_ad->device_info.network_weight[1], peer_mac,MAC_ADDR_LEN);
		
		NdisGetSystemUpTime(&ez_ad->device_info.weight_defining_link.ap_time_stamp);
		NdisGetSystemUpTime(&ez_ad->device_info.weight_defining_link.time_stamp);
		ez_ad->device_info.weight_defining_link.ezdev = ezdev;

		// Note: Backup current OPERATING params i.e. NOT CONFIG params
		NdisZeroMemory(&current_chan_info, sizeof(current_chan_info));
		current_chan_info.channel = *ezdev->channel;
#ifdef EZ_PUSH_BW_SUPPORT
		if(ez_ad->push_bw_config )
		{
			current_chan_info.ht_bw = ezdev->driver_ops->wlan_operate_get_ht_bw(ezdev); // operating ht bw
			current_chan_info.vht_bw = ezdev->driver_ops->wlan_operate_get_vht_bw(ezdev);// operating vht bw
		}
#endif
		current_chan_info.extcha= ezdev->driver_ops->wlan_operate_get_ext_cha(ezdev); // operating ext cha

		ap_ezdev->driver_ops->ez_restore_channel_config(ap_ezdev);

		if( (ezdev->ez_security.this_band_info.shared_info.channel_info.channel != current_chan_info.channel) ||
#ifdef EZ_PUSH_BW_SUPPORT
			( ez_ad->push_bw_config && 
			  ((ezdev->ez_security.this_band_info.shared_info.channel_info.ht_bw != current_chan_info.ht_bw ) ||
			  (ezdev->ez_security.this_band_info.shared_info.channel_info.vht_bw != current_chan_info.vht_bw ))) ||
#endif
			(ezdev->ez_security.this_band_info.shared_info.channel_info.extcha != current_chan_info.extcha) )
		{
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("This Band Info different \n"));
	
			this_band_changed = TRUE;
		}

		ezdev->ez_security.this_band_info.shared_info.channel_info.channel= current_chan_info.channel;
#ifdef EZ_PUSH_BW_SUPPORT
		if(ez_ad->push_bw_config )
		{
			ezdev->ez_security.this_band_info.shared_info.channel_info.ht_bw = current_chan_info.ht_bw;
			ezdev->ez_security.this_band_info.shared_info.channel_info.vht_bw = current_chan_info.vht_bw;
		}
#endif
		ezdev->ez_security.this_band_info.shared_info.channel_info.extcha= current_chan_info.extcha;

#ifdef DOT11R_FT_SUPPORT // MdId will not be changed on conenction to third party Ap
		//EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("%s::FtMdid =%c%c\n", __FUNCTION__,
		//							ezdev->ez_security.this_band_info.shared_info.FtMdId[0],
		//							ezdev->ez_security.this_band_info.shared_info.FtMdId[1]));
#endif


		ez_ad->device_info.network_weight[0] = 0xF;
		ez_ad->device_info.network_weight[0] |= BIT(6);
		
		COPY_MAC_ADDR(ezdev->ez_security.this_band_info.cli_peer_ap_mac, peer_mac);
				
		
		COPY_MAC_ADDR(ap_ezdev->ez_security.this_band_info.cli_peer_ap_mac, peer_mac);
		ez_apcli_allocate_self_node_number(&ez_ad->device_info.ez_node_number,ezdev,peer_mac);

	}

	ez_inform_all_interfaces(ezdev->ez_ad,ezdev,ACTION_UPDATE_DEVICE_INFO);
	//! initialize a local structure that will hold all the inforation to be pushed to peer AP
//! Levarage from MP1.0 CL 170192
	NdisZeroMemory(updated_configs, sizeof(updated_configs_t));
	ez_init_updated_configs_for_push(updated_configs, ezdev);

	updated_configs->device_info.network_weight[0] |= BIT(7);

	if (peer_mac== NULL){ // when disconnected
		updated_configs->context_linkdown = TRUE;
	}
	
	if (push_and_update_ap_config(ezdev->ez_ad, ezdev, updated_configs,FALSE)){
		action_sent = TRUE;
	}
//! Levarage from MP1.0 CL 170192
	if (push_and_update_cli_config(ezdev->ez_ad, ezdev, updated_configs,FALSE))
	{
		action_sent = TRUE;
	}
	
#ifdef EZ_ROAM_SUPPORT
	if(peer_mac != NULL && !MAC_ADDR_EQUAL(ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid,ZERO_MAC_ADDR))
	{
		NdisZeroMemory(ezdev->ez_security.ez_roam_info.ez_apcli_roam_bssid,MAC_ADDR_LEN);
		NdisZeroMemory(ap_ezdev->ez_security.ez_ap_roam_blocked_mac,MAC_ADDR_LEN);
		ez_update_connection_permission_hook(ezdev,EZ_DEQUEUE_PERMISSION);
	}
#endif

	if (action_sent == FALSE)
	{
		ez_update_connection_permission_hook(ezdev,EZ_ALLOW_ALL_TIMEOUT);
	}

	if (peer_mac && ap_ezdev){ // move back to third party ap channel
		//! switch back APCli channel to target channel
		ap_ezdev->driver_ops->ez_restore_channel_config(ap_ezdev);
#if (defined(DOT11_N_SUPPORT) && defined(DOT11N_DRAFT3))
		if(ap_ezdev->ez_security.ap_did_fallback){
			if(ap_ezdev->ez_security.fallback_channel != ap_ezdev->ez_security.this_band_info.shared_info.channel_info.channel){
				EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_ERROR,("\nupdate_and_push_weight: Last channel(%d) and current channel(%d) different, reset fallback context\n",
					ap_ezdev->ez_security.fallback_channel, ap_ezdev->ez_security.this_band_info.shared_info.channel_info.channel));
				ez_set_ap_fallback_context(ap_ezdev,FALSE,0);
			}
		}
#endif
		ap_ezdev->driver_ops->UpdateBeaconHandler(ap_ezdev, IE_CHANGE);

#ifdef EZ_PUSH_BW_SUPPORT
		// as CLI conencted, it's peer record should be already correct
		// update AP peer info to new config only for this band
		if( ez_ad->push_bw_config && this_band_changed){
			EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,("Change AP peer records as this band changed\n"));
			ez_update_this_band_ap_peer_record(ezdev->ez_ad, ezdev);
		}
#endif
	}
//! Levarage from MP1.0 CL 170192
	EZ_MEM_FREE(updated_configs);
	
}



INT Custom_EventHandle(ez_dev_t *ezdev, ez_custom_data_cmd_t *data, unsigned char datalen)
{
	INT Status;
	p_ez_custom_evt_t p_custom_event;

	Status = NDIS_STATUS_SUCCESS;
	
	EZ_MEM_ALLOC(NULL, (UCHAR **)&p_custom_event, sizeof(datalen));
	if (p_custom_event == NULL)
	{
		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("!!!(%s) : no memory!!!\n", __FUNCTION__));
		return NDIS_STATUS_FAILURE;
	}

	NdisZeroMemory(p_custom_event, datalen);
	NdisCopyMemory(p_custom_event, data, datalen);	

	ezdev->driver_ops->RtmpOSWrielessEventSendExt(
				ezdev,
				RT_WLAN_EVENT_CUSTOM,
				OID_WH_EZ_CUSTOM_DATA_EVENT,
				NULL,
				(UCHAR *) p_custom_event,
				datalen);

	EZ_MEM_FREE(p_custom_event);

	return Status;
}

void ez_initiate_new_scan(EZ_ADAPTER *ez_ad)
{

	//PRTMP_ADAPTER pAd = ad_obj;
	UCHAR i=0;
	for (i=0; i < MAX_EZ_BANDS; i++)
	{
		ez_dev_t* ezdev = &ez_ad->ez_band_info[i].cli_ezdev;
		ezdev->ez_security.first_scan = TRUE;
	}
}

void ez_port_secured_for_connection_offload()
{
	return;
}



BOOLEAN ez_is_loop_pkt_rcvd(ez_dev_t *ezdev, 
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

int ez_allocate_or_update_band(EZ_ADAPTER *ez_ad, ez_init_params_t *init_params)
{
	int band_count = 0;
	int allocated_band = -1;
	BOOLEAN found_band_entry = FALSE;
	for (band_count = 0; band_count < ez_ad->band_count; band_count++)
	{
		if((ez_ad->ez_band_info[band_count].pAd == init_params->ad_obj)
			&& ez_ad->ez_band_info[band_count].func_idx == init_params->func_idx) 
		{
			EZ_DEBUG(DBG_CAT_INIT, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("%s:: Found band entry at index: %d\n", __FUNCTION__ ,band_count));
			if((init_params->ezdev_type == EZDEV_TYPE_AP))
			{
				ASSERT(ez_ad->ez_band_info[band_count].ap_ezdev.wdev == NULL);
				ez_ad->ezdev_list[2 * band_count] 
					= &ez_ad->ez_band_info[band_count].ap_ezdev;
			} 
			else {
				ASSERT(ez_ad->ez_band_info[band_count].cli_ezdev.wdev == NULL);
				ez_ad->ezdev_list[2 * band_count + 1] 
					= &ez_ad->ez_band_info[band_count].cli_ezdev;

			} 
			found_band_entry = TRUE;
			allocated_band = band_count;
			
		}
	}
	if (!(found_band_entry) && (ez_ad->band_count < MAX_EZ_BANDS))
	{
		for (band_count = 0; band_count < MAX_EZ_BANDS; band_count++)
		{
			if (ez_ad->ez_band_info[band_count].cli_ezdev.wdev == NULL 
				&& ez_ad->ez_band_info[band_count].ap_ezdev.wdev== NULL)
			{
				break;
			} else {
				continue;
			}
		}
		if (band_count == MAX_EZ_BANDS)
		{
			ASSERT(FALSE);
		}
		EZ_DEBUG(DBG_CAT_INIT, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("%s:: add new band entry at index: %d\n", __FUNCTION__ ,band_count));

		ez_ad->ez_band_info[band_count].pAd = init_params->ad_obj;
		ez_ad->ez_band_info[band_count].func_idx = init_params->func_idx;
		if((init_params->ezdev_type == EZDEV_TYPE_AP))
		{
			ASSERT(ez_ad->ez_band_info[band_count].ap_ezdev.wdev == NULL);
			ez_ad->ezdev_list[band_count] 
				= &ez_ad->ez_band_info[band_count].ap_ezdev;

		} 
		else {
			ASSERT(ez_ad->ez_band_info[band_count].cli_ezdev.wdev == NULL);
			ez_ad->ezdev_list[band_count + 1] 
				= &ez_ad->ez_band_info[band_count].cli_ezdev;

		} 

		allocated_band = band_count;
		ez_ad->band_count++;		
	}
	return allocated_band;
}

void ez_dealloc_band(ez_dev_t *ezdev)
{
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	int i=0;
	ez_dev_t *ezdev_other_band_ap = NULL;
	ez_dev_t *ezdev_other_band_cli = NULL;
	void *ap_conn_wait_timer = NULL;
	void *cli_conn_wait_timer = NULL;

	ap_conn_wait_timer = ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev.ez_connect_wait_timer_backup;
	cli_conn_wait_timer = ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.ez_connect_wait_timer_backup;
	if((ezdev->ezdev_type == EZDEV_TYPE_AP))
	{
		ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev.wdev = NULL;
#ifdef IF_UP_DOWN
		if(ez_ad->ez_intf_count_current_ap > 0)
			ez_ad->ez_intf_count_current_ap--;
#endif
		if (ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.wdev &&
			ez_adapter->ez_connect_wait_timer == ap_conn_wait_timer) {
			ez_adapter->ez_connect_wait_timer = cli_conn_wait_timer;
			ez_adapter->ez_connect_wait_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev;
		}
	} else {
		ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.wdev = NULL;	
#ifdef IF_UP_DOWN
		if(ez_ad->ez_intf_count_current_cli > 0)
			ez_ad->ez_intf_count_current_cli--;
#endif
		if (ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev.wdev &&
			ez_adapter->ez_connect_wait_timer == cli_conn_wait_timer) {
			ez_adapter->ez_connect_wait_timer = ap_conn_wait_timer;
			ez_adapter->ez_connect_wait_ezdev = &ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev;
		}

		NdisZeroMemory(ezdev->ez_security.this_band_info.cli_peer_ap_mac,MAC_ADDR_LEN);
	}
	if (ez_ad->ez_band_info[ezdev->ez_band_idx].cli_ezdev.wdev == NULL 
		&& ez_ad->ez_band_info[ezdev->ez_band_idx].ap_ezdev.wdev == NULL)
	{
		ez_ad->ez_band_info[ezdev->ez_band_idx].pAd = NULL;
		ez_ad->band_count--;
		OS_NdisFreeSpinLock(&ez_ad->ez_band_info[ezdev->ez_band_idx].ez_peer_table_lock);
		for (i = 0; i < EZ_MAX_STA_NUM; i++) {
			struct _ez_peer_security_info *ez_peer = &ez_ad->ez_band_info[ezdev->ez_band_idx].ez_peer_table[i];
			if (ez_peer->gtk && ez_peer->ezdev == ezdev) {
				EZ_MEM_FREE(ez_peer->gtk);
				ez_peer->gtk = NULL;
				ez_peer->gtk_len = 0;
			}

			if (ez_peer->group_id && ez_peer->ezdev == ezdev) {
				EZ_MEM_FREE( ez_peer->group_id);
				ez_peer->group_id = NULL;
				ez_peer->group_id_len= 0;
			}
			if (ez_peer->gen_group_id && ez_peer->ezdev == ezdev) {
				EZ_MEM_FREE( ez_peer->gen_group_id);
				ez_peer->gen_group_id = NULL;
				ez_peer->gen_group_id_len= 0;
			}

		}

		/*In DBDC mode, two bands share conn wait timer,
		* when a band down, switch this timer to other band.*/
		if (ez_adapter->ez_connect_wait_timer == ap_conn_wait_timer ||
			ez_adapter->ez_connect_wait_timer == cli_conn_wait_timer) {
			ezdev_other_band_ap = ez_get_otherband_ap_ezdev(ezdev);
			ezdev_other_band_cli = ez_get_otherband_cli_ezdev(ezdev);

			if (ezdev_other_band_ap) {
				ez_adapter->ez_connect_wait_timer = ezdev_other_band_ap->ez_connect_wait_timer_backup;
				ez_adapter->ez_connect_wait_ezdev = ezdev_other_band_ap;
			} else if (ezdev_other_band_cli) {
				ez_adapter->ez_connect_wait_timer = ezdev_other_band_cli->ez_connect_wait_timer_backup;
				ez_adapter->ez_connect_wait_ezdev = ezdev_other_band_cli;
			} else {
				ez_adapter->ez_connect_wait_timer = NULL;
				ez_adapter->ez_connect_wait_ezdev = NULL;
			}
		}
	}
	if (ez_ad->band_count == 0) {
		NdisZeroMemory(ez_ad->ez_band_info, sizeof(ez_ad->ez_band_info));
		NdisZeroMemory(&ez_ad->device_info, sizeof(ez_ad->device_info));
		ez_ad->sanity_check = NULL;
		ez_ad->sanity_check1 = NULL;

		EZ_DEBUG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			("%s: clean ez_band_info.\n", __FUNCTION__));
	}
}

void ez_init_other_band_backup(ez_dev_t *ezdev, ez_dev_t *cli_ezdev)
{
	struct _ez_security *ez_sec_info, *cli_ez_sec_info;
	EZ_ADAPTER *ez_ad = ezdev->ez_ad;
	CHAR Ssid5[]="MTAP5";
	CHAR Ssid2[]="MTAP2";
	ez_sec_info = &ezdev->ez_security;
	cli_ez_sec_info = &cli_ezdev->ez_security;
	if(*ezdev->channel > 14)
	{
		ez_sec_info->other_band_info_backup.shared_info.ssid_len  = strlen(&Ssid2[0]);
		ez_sec_info->other_band_info_backup.shared_info.channel_info.channel = 1;
#ifdef EZ_PUSH_BW_SUPPORT
		if( ez_ad->push_bw_config )
		{
			ez_sec_info->other_band_info_backup.shared_info.channel_info.ht_bw = 1;
			ez_sec_info->other_band_info_backup.shared_info.channel_info.vht_bw = 0;
		}
		else
		{
			ez_sec_info->other_band_info_backup.shared_info.channel_info.ht_bw = 0xFF;
			ez_sec_info->other_band_info_backup.shared_info.channel_info.vht_bw = 0xFF;
		}
#else
		ez_sec_info->other_band_info_backup.shared_info.channel_info.rsvd1 = 0xFF;
		ez_sec_info->other_band_info_backup.shared_info.channel_info.rsvd2 = 0xFF;
#endif
		ez_sec_info->other_band_info_backup.shared_info.channel_info.extcha = 0x1;
		NdisCopyMemory(ez_sec_info->other_band_info_backup.shared_info.ssid, Ssid2, strlen(&Ssid2[0]));

	}
	else
	{
		ez_sec_info->other_band_info_backup.shared_info.ssid_len  = strlen(&Ssid5[0]);
		ez_sec_info->other_band_info_backup.shared_info.channel_info.channel = 36;
#ifdef EZ_PUSH_BW_SUPPORT
		if( ez_ad->push_bw_config )
		{
			ez_sec_info->other_band_info_backup.shared_info.channel_info.ht_bw = 1;
			ez_sec_info->other_band_info_backup.shared_info.channel_info.vht_bw = 1;
		}
		else
		{
			ez_sec_info->other_band_info_backup.shared_info.channel_info.ht_bw = 0xFF;
			ez_sec_info->other_band_info_backup.shared_info.channel_info.vht_bw = 0xFF;
		}
#else
		ez_sec_info->other_band_info_backup.shared_info.channel_info.rsvd1 = 0xFF;
		ez_sec_info->other_band_info_backup.shared_info.channel_info.rsvd2 = 0xFF;

#endif
		ez_sec_info->other_band_info_backup.shared_info.channel_info.extcha = 0x1;
		NdisCopyMemory(ez_sec_info->other_band_info_backup.shared_info.ssid, Ssid5, strlen(&Ssid5[0]));
	}

#ifdef DOT11R_FT_SUPPORT
	FT_SET_MDID(ez_sec_info->other_band_info_backup.shared_info.FtMdId, ez_sec_info->this_band_info.shared_info.FtMdId);
#endif
				ez_sec_info->other_band_info_backup.interface_activated = TRUE; 	
				if(*ezdev->channel > 14)
				{
					cli_ez_sec_info->other_band_info_backup.shared_info.ssid_len  = strlen(&Ssid2[0]);
					cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.channel = 1;
#ifdef EZ_PUSH_BW_SUPPORT
					if( ez_ad->push_bw_config )
					{
						cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.ht_bw = 1;
						cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.vht_bw = 0;
					}
					else
					{
						cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.ht_bw = 0xFF;
						cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.vht_bw = 0xFF;
					}
#else
					cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.rsvd1 = 0xFF;
					cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.rsvd2 = 0xFF;
#endif
					cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.extcha = 0x1;
					NdisCopyMemory(cli_ez_sec_info->other_band_info_backup.shared_info.ssid, Ssid2, strlen(&Ssid2[0]));
				}
				else
				{
					cli_ez_sec_info->other_band_info_backup.shared_info.ssid_len  = strlen(&Ssid5[0]);
					cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.channel = 36;
#ifdef EZ_PUSH_BW_SUPPORT
					if( ez_ad->push_bw_config )
					{
						cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.ht_bw = 1;
						cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.vht_bw = 1;
					}
					else
					{
						cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.ht_bw = 0xFF;
						cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.vht_bw = 0xFF;
					}
#else
					cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.rsvd1 = 0xFF;
					cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.rsvd2 = 0xFF;
#endif
					cli_ez_sec_info->other_band_info_backup.shared_info.channel_info.extcha = 0x1;
					NdisCopyMemory(cli_ez_sec_info->other_band_info_backup.shared_info.ssid, Ssid5, strlen(&Ssid5[0]));
				}
#ifdef DOT11R_FT_SUPPORT
				FT_SET_MDID(cli_ez_sec_info->other_band_info_backup.shared_info.FtMdId, cli_ez_sec_info->this_band_info.shared_info.FtMdId);
#endif
				cli_ez_sec_info->other_band_info_backup.interface_activated = TRUE; 	
			
}

#if 0
void ez_dealloc_non_ez_band(ez_dev_t *ezdev)
{
	int band_count = 0;
	EZ_ADAPTER *ez_ad = ezdev;
	BOOLEAN entry_deleted = FALSE;
	for (band_count = 0; band_count < MAX_NON_EZ_BANDS; band_count++)
	{
		if (ezdev->ezdev_type == EZDEV_TYPE_AP)
		{
			if(ezdev == ez_ad->non_ez_band_info[band_count].non_ap_ezdev)
			{
				ez_ad->non_ez_band_info[band_count].non_ap_ezdev = NULL;
				entry_deleted = TRUE;
			}
		} else {
			if(ezdev == ez_ad->non_ez_band_info[band_count].non_ez_cli_ezdev)
			{
				ez_ad->non_ez_band_info[band_count].non_ez_cli_ezdev = NULL;
				entry_deleted = TRUE;
			}
		}
		if (entry_deleted)
		{
			if (ez_ad->non_ez_band_info[band_count].non_ap_ezdev == NULL 
				&& ez_ad->non_ez_band_info[band_count].non_ez_cli_ezdev == NULL)
			{
				ez_ad->non_ez_band_info[ezdev->ez_band_idx].ez_ad = NULL;
				ez_ad->non_ez_band_count--;		
			}
			return;
		}
	}
}
#endif
EXPORT_SYMBOL(mtk_oui);
EXPORT_SYMBOL(ez_peer_table_delete);
