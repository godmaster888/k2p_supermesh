#ifndef __EZ_CMM_H__
#define __EZ_CMM_H__

#ifdef WH_EZ_SETUP

#include "ez_mod_os.h"
#include "rtmp_type.h"

#ifdef DOT11R_FT_SUPPORT
#include "dot11r_ft.h"
#endif

#include "shared_structs.h"




#define OS_HZ					RtmpOsTickUnitGet()


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
#define RTMP_TIME_AFTER(a,b)		\
	(typecheck(unsigned long, (unsigned long)a) && \
	 typecheck(unsigned long, (unsigned long)b) && \
	 ((long)(b) - (long)(a) < 0))

#define RTMP_TIME_AFTER_EQ(a,b)	\
	(typecheck(unsigned long, (unsigned long)a) && \
	 typecheck(unsigned long, (unsigned long)b) && \
	 ((long)(a) - (long)(b) >= 0))
#define RTMP_TIME_BEFORE(a,b)	RTMP_TIME_AFTER_EQ(b,a)
#else
#define typecheck(type,x) \
({      type __dummy; \
        typeof(x) __dummy2; \
        (void)(&__dummy == &__dummy2); \
        1; \
})
#define RTMP_TIME_AFTER_EQ(a,b)	\
	(typecheck(unsigned long, (unsigned long)a) && \
	 typecheck(unsigned long, (unsigned long)b) && \
	 ((long)(a) - (long)(b) >= 0))
#define RTMP_TIME_BEFORE(a,b)	RTMP_TIME_AFTER_EQ(b,a)
#define RTMP_TIME_AFTER(a,b) time_after(a, b)
#endif









#ifndef TRUE
    #define TRUE    1
#endif

#ifndef FALSE
    #define FALSE    0
#endif


#define MLME_SYNC_LOCK					0x1
#define BEACON_UPDATE_LOCK				0x2
#define EZ_MINIPORT_LOCK				0x3
#define SCAN_PAUSE_TIMER_LOCK			0x4

#ifndef APCLI_DISCONNECT_SUB_REASON_MNT_NO_BEACON
#define APCLI_DISCONNECT_SUB_REASON_MNT_NO_BEACON           5
#endif
#ifdef WH_EZ_SETUP
enum {
	OID_WH_EZ_ENABLE = 0x2000,
	OID_WH_EZ_CONF_STATUS = 0x2001,
	OID_WH_EZ_GROUP_ID = 0x2002,
	OID_WH_EZ_GEN_GROUP_ID = 0x2003,
	OID_WH_EZ_RSSI_THRESHOLD = 0x2004,
	OID_WH_EZ_INTERNET_COMMAND = 0x2005,
	OID_WH_EZ_GET_GUI_INFO = 0x2006,
#ifdef EZ_PUSH_BW_SUPPORT
	OID_WH_EZ_PUSH_BW = 0x2007,
#endif
	OID_WH_EZ_CUSTOM_DATA_CMD = 0x2008,
	OID_WH_EZ_CUSTOM_DATA_EVENT = 0x2009
};
#define OID_WH_EZ_UPDATE_STA_INFO				  0x2010
#define OID_WH_EZ_MAN_DEAMON_EVENT	 					 0x200A
#define OID_WH_EZ_MAN_TRIBAND_EZ_DEVINFO_EVENT	 			 0x200B
#define OID_WH_EZ_MAN_TRIBAND_NONEZ_DEVINFO_EVENT	 			 0x200c
#define OID_WH_EZ_MAN_TRIBAND_SCAN_COMPLETE_EVENT	 			 0x200d
//! Levarage from MP1.0  CL #170037
#define OID_WH_EZ_MAN_PLUS_NONMAN_EZ_DEVINFO_EVENT	 			 0x200E
#define OID_WH_EZ_MAN_PLUS_NONMAN_NONEZ_DEVINFO_EVENT	 		 0x200F
#define OID_WH_EZ_MAN_CONF_EVENT	 					 0x2011
#define OID_WH_EZ_GROUP_ID_UPDATE	 			 0x2014
#endif /* WH_EZ_SETUP */




#define RT_WLAN_EVENT_CUSTOM							0x01

#ifdef WH_EZ_SETUP
// For whole home coverage - easy setup wireless event - start
#define	IW_WH_EZ_EVENT_FLAG_START                  	0x0700
#define	IW_WH_EZ_PROVIDER_SEARCHING                 0x0700
#define	IW_WH_EZ_PROVIDER_FOUND       				0x0701
#define	IW_WH_EZ_PROVIDER_STOP_SEARCHING            0x0702
#define	IW_WH_EZ_CONFIGURED_AP_SEARCHING            0x0703
#define	IW_WH_EZ_CONFIGURED_AP_FOUND                0x0704
#define	IW_WH_EZ_MY_APCLI_CONNECTED                 0x0705
#define	IW_WH_EZ_MY_APCLI_DISCONNECTED              0x0706
#define	IW_WH_EZ_MY_AP_HAS_APCLI                    0x0707
#define	IW_WH_EZ_MY_AP_DOES_NOT_HAS_APCLI           0x0708
#define	IW_WH_EZ_BECOME_CONFIGURED                  0x0709
#define	IW_WH_EZ_EVENT_FLAG_END                     0x0709
#define	IW_WH_EZ_EVENT_TYPE_NUM						(IW_WH_EZ_EVENT_FLAG_END - IW_WH_EZ_EVENT_FLAG_START + 1)
/* For whole home coverage - easy setup wireless event - end */
#endif /* WH_EZ_SETUP */



#define EZDEV_NUM_MAX							6
#define SSID_EQUAL(ssid1, len1, ssid2, len2)    ((len1==len2) && (NdisEqualMemory(ssid1, ssid2, len1)))

#define IE_CHANGE								0
#define MAC_ADDR_EQUAL(pAddr1,pAddr2)           NdisEqualMemory((PVOID)(pAddr1), (PVOID)(pAddr2), MAC_ADDR_LEN)
#define MGMT_DMA_BUFFER_SIZE					1024
#define SWAP32(x) \
    ((UINT32) (\
	       (((UINT32) (x) & (UINT32) 0x000000ffUL) << 24) | \
	       (((UINT32) (x) & (UINT32) 0x0000ff00UL) << 8) | \
	       (((UINT32) (x) & (UINT32) 0x00ff0000UL) >> 8) | \
	       (((UINT32) (x) & (UINT32) 0xff000000UL) >> 24)))

#if 0
#define IS_EZ_SETUP_ENABLED(_wdev)  (((_wdev)->enable_easy_setup) == TRUE)
#define IS_ADPTR_EZ_SETUP_ENABLED(pAd) (pAd->ApCfg.MBSSID[0].wdev.enable_easy_setup)
#endif
#ifndef GNU_PACKED
#define GNU_PACKED  __attribute__ ((packed))
#endif /* GNU_PACKED */

#define BAND0                       0
#define BAND1                       1



#define GET_SEC_AKM(_SecConfig)              ((_SecConfig)->AKMMap)
#define CLEAR_SEC_AKM(_AKMMap)              (_AKMMap = 0x0)
#define SET_AKM_OPEN(_AKMMap)           (_AKMMap |= (1 << SEC_AKM_OPEN))
#define SET_AKM_SHARED(_AKMMap)       (_AKMMap |= (1 << SEC_AKM_SHARED))
#define SET_AKM_AUTOSWITCH(_AKMMap)       (_AKMMap |= (1 << SEC_AKM_AUTOSWITCH))
#define SET_AKM_WPA1(_AKMMap)          (_AKMMap |= (1 << SEC_AKM_WPA1))
#define SET_AKM_WPA1PSK(_AKMMap)    (_AKMMap |= (1 << SEC_AKM_WPA1PSK))
#define SET_AKM_WPANONE(_AKMMap)  (_AKMMap |= (1 << SEC_AKM_WPANone))
#define SET_AKM_WPA2(_AKMMap)          (_AKMMap |= (1 << SEC_AKM_WPA2))
#define SET_AKM_WPA2PSK(_AKMMap)    (_AKMMap |= (1 << SEC_AKM_WPA2PSK))
#define SET_AKM_FT_WPA2(_AKMMap)                  (_AKMMap |= (1 << SEC_AKM_FT_WPA2))
#define SET_AKM_FT_WPA2PSK(_AKMMap)            (_AKMMap |= (1 << SEC_AKM_FT_WPA2PSK))
#define SET_AKM_WPA2_SHA256(_AKMMap)         (_AKMMap |= (1 << SEC_AKM_WPA2_SHA256))
#define SET_AKM_WPA2PSK_SHA256(_AKMMap)   (_AKMMap |= (1 << SEC_AKM_WPA2PSK_SHA256))
#define SET_AKM_TDLS(_AKMMap)                           (_AKMMap |= (1 << SEC_AKM_TDLS))
#define SET_AKM_SAE_SHA256(_AKMMap)              (_AKMMap |= (1 << SEC_AKM_SAE_SHA256))
#define SET_AKM_FT_SAE_SHA256(_AKMMap)        (_AKMMap |= (1 << SEC_AKM_FT_SAE_SHA256))
#define SET_AKM_SUITEB_SHA256(_AKMMap)         (_AKMMap |= (1 << SEC_AKM_SUITEB_SHA256))
#define SET_AKM_SUITEB_SHA384(_AKMMap)         (_AKMMap |= (1 << SEC_AKM_SUITEB_SHA384))
#define SET_AKM_FT_WPA2_SHA384(_AKMMap)     (_AKMMap |= (1 << SEC_AKM_FT_WPA2_SHA384))
#ifdef WAPI_SUPPORT
#define SET_AKM_WAICERT(_AKMMap)                   (_AKMMap |= (1 << SEC_AKM_WAICERT))
#define SET_AKM_WPIPSK(_AKMMap)                     (_AKMMap |= (1 << SEC_AKM_WAIPSK))
#endif /* WAPI_SUPPORT */


#define IS_CIPHER_NONE(_Cipher)          (((_Cipher) & (1 << SEC_CIPHER_NONE)) > 0)
#define IS_CIPHER_WEP40(_Cipher)          (((_Cipher) & (1 << SEC_CIPHER_WEP40)) > 0)
#define IS_CIPHER_WEP104(_Cipher)        (((_Cipher) & (1 << SEC_CIPHER_WEP104)) > 0)
#define IS_CIPHER_WEP128(_Cipher)        (((_Cipher) & (1 << SEC_CIPHER_WEP128)) > 0)
#define IS_CIPHER_WEP(_Cipher)              (((_Cipher) & ((1 << SEC_CIPHER_WEP40) | (1 << SEC_CIPHER_WEP104) | (1 << SEC_CIPHER_WEP128))) > 0)
#define IS_CIPHER_TKIP(_Cipher)              (((_Cipher) & (1 << SEC_CIPHER_TKIP)) > 0)
#define IS_CIPHER_WEP_TKIP_ONLY(_Cipher)     ((IS_CIPHER_WEP(_Cipher) || IS_CIPHER_TKIP(_Cipher)) && (_Cipher < (1 << SEC_CIPHER_CCMP128)))
#define IS_CIPHER_CCMP128(_Cipher)      (((_Cipher) & (1 << SEC_CIPHER_CCMP128)) > 0)
#define IS_CIPHER_CCMP256(_Cipher)      (((_Cipher) & (1 << SEC_CIPHER_CCMP256)) > 0)
#define IS_CIPHER_GCMP128(_Cipher)     (((_Cipher) & (1 << SEC_CIPHER_GCMP128)) > 0)
#define IS_CIPHER_GCMP256(_Cipher)     (((_Cipher) & (1 << SEC_CIPHER_GCMP256)) > 0)
#define IS_CIPHER_BIP_CMAC128(_Cipher)     (((_Cipher) & (1 << SEC_CIPHER_BIP_CMAC128)) > 0)


#define IS_AKM_OPEN(_AKMMap)                           ((_AKMMap & (1 << SEC_AKM_OPEN)) > 0)
#define IS_AKM_SHARED(_AKMMap)                       ((_AKMMap & (1 << SEC_AKM_SHARED)) > 0)
#define IS_AKM_AUTOSWITCH(_AKMMap)              ((_AKMMap & (1 << SEC_AKM_AUTOSWITCH)) > 0)
#define IS_AKM_WPA1(_AKMMap)                           ((_AKMMap & (1 << SEC_AKM_WPA1)) > 0)
#define IS_AKM_WPA1PSK(_AKMMap)                    ((_AKMMap & (1 << SEC_AKM_WPA1PSK)) > 0)
#define IS_AKM_WPANONE(_AKMMap)                  ((_AKMMap & (1 << SEC_AKM_WPANone)) > 0)
#define IS_AKM_WPA2(_AKMMap)                          ((_AKMMap & (1 << SEC_AKM_WPA2)) > 0)
#define IS_AKM_WPA2PSK(_AKMMap)                    ((_AKMMap & (1 << SEC_AKM_WPA2PSK)) > 0)
#define IS_AKM_FT_WPA2(_AKMMap)                     ((_AKMMap & (1 << SEC_AKM_FT_WPA2)) > 0)
#define IS_AKM_FT_WPA2PSK(_AKMMap)              ((_AKMMap & (1 << SEC_AKM_FT_WPA2PSK)) > 0)
#define IS_AKM_WPA2_SHA256(_AKMMap)            ((_AKMMap & (1 << SEC_AKM_WPA2_SHA256)) > 0)
#define IS_AKM_WPA2PSK_SHA256(_AKMMap)      ((_AKMMap & (1 << SEC_AKM_WPA2PSK_SHA256)) > 0)
#define IS_AKM_TDLS(_AKMMap)                             ((_AKMMap & (1 << SEC_AKM_TDLS)) > 0)
#define IS_AKM_SAE_SHA256(_AKMMap)                ((_AKMMap & (1 << SEC_AKM_SAE_SHA256)) > 0)
#define IS_AKM_FT_SAE_SHA256(_AKMMap)          ((_AKMMap & (1 << SEC_AKM_FT_SAE_SHA256)) > 0)
#define IS_AKM_SUITEB_SHA256(_AKMMap)          ((_AKMMap & (1 << SEC_AKM_SUITEB_SHA256)) > 0)
#define IS_AKM_SUITEB_SHA384(_AKMMap)          ((_AKMMap & (1 << SEC_AKM_SUITEB_SHA384)) > 0)
#define IS_AKM_FT_WPA2_SHA384(_AKMMap)      ((_AKMMap & (1 << SEC_AKM_FT_WPA2_SHA384)) > 0)
#ifdef WAPI_SUPPORT
#define IS_AKM_WAICERT(_AKMMap)                      ((_AKMMap & (1 << SEC_AKM_WAICERT)) > 0)
#define IS_AKM_WPIPSK(_AKMMap)                        ((_AKMMap & (1 << SEC_AKM_WAIPSK)) > 0)
#endif /* WAPI_SUPPORT */

#define IS_AKM_PSK(_AKMMap)     (IS_AKM_WPA1PSK(_AKMMap)  \
                                                              || IS_AKM_WPA2PSK(_AKMMap))

#define IS_AKM_1X(_AKMMap)     (IS_AKM_WPA1(_AKMMap)  \
                                                              || IS_AKM_WPA2(_AKMMap))

#define IS_AKM_WPA_CAPABILITY(_AKMMap)     (IS_AKM_WPA1(_AKMMap)  \
                                                                              || IS_AKM_WPA1PSK(_AKMMap) \
                                                                              || IS_AKM_WPANONE(_AKMMap) \
                                                                              || IS_AKM_WPA2(_AKMMap) \
                                                                              || IS_AKM_WPA2PSK(_AKMMap) \
                                                                              || IS_AKM_WPA2_SHA256(_AKMMap) \
                                                                              || IS_AKM_WPA2PSK_SHA256(_AKMMap))
typedef enum _SEC_CIPHER_MODE {
	SEC_CIPHER_NONE,
	SEC_CIPHER_WEP40,
	SEC_CIPHER_WEP104,
	SEC_CIPHER_WEP128,
	SEC_CIPHER_TKIP,
	SEC_CIPHER_CCMP128,
	SEC_CIPHER_CCMP256,
	SEC_CIPHER_GCMP128,
	SEC_CIPHER_GCMP256,
	SEC_CIPHER_BIP_CMAC128,
	SEC_CIPHER_BIP_CMAC256,
	SEC_CIPHER_BIP_GMAC128,
	SEC_CIPHER_BIP_GMAC256,
	SEC_CIPHER_WPI_SMS4, /* WPI SMS4 support */
	SEC_CIPHER_MAX /* Not a real mode, defined as upper bound */
} SEC_CIPHER_MODE;


#define CLEAR_PAIRWISE_CIPHER(_SecConfig)      ((_SecConfig)->PairwiseCipher = 0x0)
#define CLEAR_GROUP_CIPHER(_SecConfig)          ((_SecConfig)->GroupCipher = 0x0)
#define GET_PAIRWISE_CIPHER(_SecConfig)         ((_SecConfig)->PairwiseCipher)
#define GET_GROUP_CIPHER(_SecConfig)              ((_SecConfig)->GroupCipher)
#define CLEAR_CIPHER(_cipher)               	    (_cipher  = 0x0)
#define SET_CIPHER_NONE(_cipher)               (_cipher |= (1 << SEC_CIPHER_NONE))
#define SET_CIPHER_WEP40(_cipher)             (_cipher |= (1 << SEC_CIPHER_WEP40))
#define SET_CIPHER_WEP104(_cipher)           (_cipher |= (1 << SEC_CIPHER_WEP104))
#define SET_CIPHER_WEP128(_cipher)           (_cipher |= (1 << SEC_CIPHER_WEP128))
#define SET_CIPHER_WEP(_cipher)                 (_cipher |= (1 << SEC_CIPHER_WEP40) | (1 << SEC_CIPHER_WEP104) | (1 << SEC_CIPHER_WEP128))
#define SET_CIPHER_TKIP(_cipher)                  (_cipher |= (1 << SEC_CIPHER_TKIP))
#define SET_CIPHER_CCMP128(_cipher)          (_cipher |= (1 << SEC_CIPHER_CCMP128))
#define SET_CIPHER_CCMP256(_cipher)          (_cipher |= (1 << SEC_CIPHER_CCMP256))
#define SET_CIPHER_GCMP128(_cipher)         (_cipher |= (1 << SEC_CIPHER_GCMP128))
#define SET_CIPHER_GCMP256(_cipher)         (_cipher |= (1 << SEC_CIPHER_GCMP256))
#ifdef WAPI_SUPPORT
#define SET_CIPHER_WPI_SMS4(_cipher)       (_cipher |= (1 << SEC_CIPHER_WPI_SMS4))
#endif /* WAPI_SUPPORT */


/* 802.11 authentication and key management */
typedef enum _SEC_AKM_MODE {
    SEC_AKM_OPEN,
    SEC_AKM_SHARED,
    SEC_AKM_AUTOSWITCH,
    SEC_AKM_WPA1, /* Enterprise security over 802.1x */
    SEC_AKM_WPA1PSK,
    SEC_AKM_WPANone, /* For Win IBSS, directly PTK, no handshark */
    SEC_AKM_WPA2, /* Enterprise security over 802.1x */
    SEC_AKM_WPA2PSK,
    SEC_AKM_FT_WPA2,
    SEC_AKM_FT_WPA2PSK,
    SEC_AKM_WPA2_SHA256,
    SEC_AKM_WPA2PSK_SHA256,
    SEC_AKM_TDLS,
    SEC_AKM_SAE_SHA256,
    SEC_AKM_FT_SAE_SHA256,
    SEC_AKM_SUITEB_SHA256,
    SEC_AKM_SUITEB_SHA384,
    SEC_AKM_FT_WPA2_SHA384,
    SEC_AKM_WAICERT, /* WAI certificate authentication */
    SEC_AKM_WAIPSK, /* WAI pre-shared key */
    SEC_AKM_MAX /* Not a real mode, defined as upper bound */
} SEC_AKM_MODE, *PSEC_AKM_MODE;

#define END_OF_ARGS                 -1
#define NdisStatus				int
#define MAX_EZ_BANDS 						2
#define MAX_NON_EZ_BANDS 						2
#define LEN_PMK						32

#define DEDICATED_MAN_AP   			1
//! Levarage from CL170210

#define MAX_EZ_PEERS_PER_BAND		8
#define MAC_ADDR_LEN				6
#define FT_MDID_LEN					2
#define LENGTH_802_11               24
#define TIMESTAMP_LEN                 8

#define NDIS_STATUS_SUCCESS 0
#define NDIS_STATUS_FAILURE 1

#define MLME_SUCCESS				NDIS_STATUS_SUCCESS
#define MLME_INVALID_FORMAT		 	0x51
#define MLME_FAIL_NO_RESOURCE           0x52

/* value domain of 802.11 MGMT frame's FC.subtype, which is b7..4 of the 1st-byte of MAC header */
#define SUBTYPE_ASSOC_REQ           0
#define SUBTYPE_ASSOC_RSP           1
#define SUBTYPE_REASSOC_REQ         2
#define SUBTYPE_REASSOC_RSP         3
#define SUBTYPE_PROBE_REQ           4
#define SUBTYPE_PROBE_RSP           5
#define SUBTYPE_TIMING_ADV			6
#define SUBTYPE_BEACON              8
#define SUBTYPE_ATIM                9
#define SUBTYPE_DISASSOC            10
#define SUBTYPE_AUTH                11
#define SUBTYPE_DEAUTH              12
#define SUBTYPE_ACTION              13
#define SUBTYPE_ACTION_NO_ACK		14

#define H_CHANNEL_BIGGER_THAN   100
#define RTPKT_TO_OSPKT(_p)		((struct sk_buff *)(_p))
#define OSPKT_TO_RTPKT(_p)		((PNDIS_PACKET)(_p))
#define NDIS_STATUS		INT

#define GET_OS_PKT_DATAPTR(_pkt) \
		(RTPKT_TO_OSPKT(_pkt)->data)
#define SET_OS_PKT_DATAPTR(_pkt, _dataPtr)	\
		(RTPKT_TO_OSPKT(_pkt)->data) = (_dataPtr)

#define GET_OS_PKT_LEN(_pkt) \
		(RTPKT_TO_OSPKT(_pkt)->len)

#define RTMP_GET_PACKET_IGMP(_p)	(PACKET_CB(_p, 37))

#ifdef CONFIG_WIFI_PKT_FWD_V1
#define RTMP_GET_PACKET_BAND(_p)	(PACKET_CB(_p, 34))
#else
#define RTMP_GET_PACKET_BAND(_p)	(PACKET_CB(_p, 33))
#endif
/* [CB_OFF + 34]: tag the packet received from which net device */
#ifdef CONFIG_WIFI_PKT_FWD_V1
#define RECV_FROM_CB			35
#else
#define RECV_FROM_CB			34
#endif
#define H_CHANNEL_BIGGER_THAN   100
#define RTMP_PACKET_RECV_FROM_2G_CLIENT 	0x1
#define RTMP_PACKET_RECV_FROM_5G_CLIENT 	0x2
#define RTMP_PACKET_RECV_FROM_2G_AP			0x4
#define RTMP_PACKET_RECV_FROM_5G_AP 		0x8
#define RTMP_PACKET_RECV_FROM_5G_H_CLIENT   0x10
#define RTMP_PACKET_RECV_FROM_5G_H_AP     	0x20


#define SHA1_BLOCK_SIZE    64	/* 512 bits = 64 bytes */
#define SHA1_DIGEST_SIZE   20	/* 160 bits = 20 bytes */
#ifdef CONFIG_WIFI_PKT_FWD_V1
#define RTMP_SET_PACKET_RECV_FROM(_p, _flg)	\
	do{                 	                        				\
          	if (_flg)                               				\
                	PACKET_CB(_p, 35) |= (_flg);    		\
                else                                   	 			\
                        PACKET_CB(_p, 35) &= (~_flg);   	\
          }while(0)
#else
#define RTMP_SET_PACKET_RECV_FROM(_p, _flg)	\
	do{                 	                        				\
          	if (_flg)                               				\
                	PACKET_CB(_p, 34) |= (_flg);    		\
                else                                   	 			\
                        PACKET_CB(_p, 34) &= (~_flg);   	\
          }while(0)
#endif
#ifdef RT_CFG80211_SUPPORT
#define CB_OFF  4
#else
#define CB_OFF  10
#endif
#define GET_OS_PKT_CB(_p)		(RTPKT_TO_OSPKT(_p)->cb)
#define PACKET_CB(_p, _offset)	((RTPKT_TO_OSPKT(_p)->cb[CB_OFF + (_offset)]))

#ifdef CONFIG_WIFI_PKT_FWD_V1
#define RTMP_GET_PACKET_RECV_FROM(_p)        (PACKET_CB(_p, 35))
#define RTMP_IS_PACKET_APCLI(_p)	((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+35]) & (RTMP_PACKET_RECV_FROM_2G_CLIENT | RTMP_PACKET_RECV_FROM_5G_CLIENT | RTMP_PACKET_RECV_FROM_5G_H_CLIENT))
#define RTMP_IS_PACKET_AP_APCLI(_p)	((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+35])!= 0)
#else
#define RTMP_GET_PACKET_RECV_FROM(_p)        (PACKET_CB(_p, 34))
#define RTMP_IS_PACKET_APCLI(_p)	((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+34]) & (RTMP_PACKET_RECV_FROM_2G_CLIENT | RTMP_PACKET_RECV_FROM_5G_CLIENT | RTMP_PACKET_RECV_FROM_5G_H_CLIENT))
#define RTMP_IS_PACKET_AP_APCLI(_p)	((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+34])!= 0)
#endif
#if 0

#define EZ_GET_EZBAND_CALLBACK(ez_ad, band_idx, chipop_callback) 			((EZ_ADAPTER *)ez_ad)->ez_band_info[band_idx].lut_chipops-> ## chipop_callback
#define EZ_GET_NONEZBAND_CALLBACK(ez_ad, band_idx, chipop_callback) 		((EZ_ADAPTER *)ez_ad)->nonez_band_info[band_idx].lut_chipops-> ## chipop_callback
#define EZ_GET_EZBAND_ADAPTER(ez_ad, band_idx)								((EZ_ADAPTER *)ez_ad)->ez_band_info[band_idx].pAd
#define EZ_GET_EZBAND_APDEV(ez_ad, band_idx)								((EZ_ADAPTER *)ez_ad)->ez_band_info[band_idx].ap_ezdev
#define EZ_GET_EZBAND_CLIDEV(ez_ad, band_idx)								((EZ_ADAPTER *)ez_ad)->ez_band_info[band_idx].cli_ezdev
#define EZ_GET_EZBAND_BAND(ez_ad, band_idx)									&((EZ_ADAPTER *)ez_ad)->ez_band_info[band_idx]

#else
#define EZ_GET_EZBAND_CALLBACK(ez_ad, band_idx, chipop_callback) 				ez_adapter->ez_band_info[band_idx].lut_chipops.chipop_callback
#define EZ_GET_NONEZBAND_CALLBACK(ez_ad, band_idx, chipop_callback) 		ez_adapter->non_ez_band_info[band_idx].lut_chipops.chipop_callback
#define EZ_GET_EZBAND_ADAPTER(ez_ad, band_idx)								ez_adapter->ez_band_info[band_idx].pAd
#define EZ_GET_EZBAND_APDEV(ez_ad, band_idx)								((ez_dev_t *)&ez_adapter->ez_band_info[band_idx].ap_ezdev)
#define EZ_GET_EZBAND_CLIDEV(ez_ad, band_idx)								((ez_dev_t *)&ez_adapter->ez_band_info[band_idx].cli_ezdev)
#define EZ_GET_EZBAND_BAND(ez_ad, band_idx)									((EZ_BAND_INFO *)&ez_adapter->ez_band_info[band_idx])

#endif
#ifdef BIG_ENDIAN

#define cpu2le64(x)    SWAP64((x))
#define le2cpu64(x)    SWAP64((x))
#define cpu2le32(x)    SWAP32((x))
#define le2cpu32(x)    SWAP32((x))
#define cpu2le16(x)    SWAP16((x))
#define le2cpu16(x)    SWAP16((x))
#define cpu2be64(x)    ((UINT64)(x))
#define be2cpu64(x)    ((UINT64)(x))
#define cpu2be32(x)    ((UINT32)(x))
#define be2cpu32(x)    ((UINT32)(x))
#define cpu2be16(x)    ((UINT16)(x))
#define be2cpu16(x)    ((UINT16)(x))

#else // Little_Endian

#define cpu2le64(x)    ((UINT64)(x))
#define le2cpu64(x)    ((UINT64)(x))
#define cpu2le32(x)    ((UINT32)(x))
#define le2cpu32(x)    ((UINT32)(x))
#define cpu2le16(x)    ((UINT16)(x))
#define le2cpu16(x)    ((UINT16)(x))
#define cpu2be64(x)    SWAP64((x))
#define be2cpu64(x)    SWAP64((x))
#define cpu2be32(x)    SWAP32((x))
#define be2cpu32(x)    SWAP32((x))
#define cpu2be16(x)    SWAP16((x))
#define be2cpu16(x)    SWAP16((x))

#endif // BIG_ENDIAN

enum ACTION_TYPE{
	ACTION_TYPE_NONE =0,
	ACTION_TYPE_DELAY_DISCONNECT =1,
	ACTION_TYPE_UPDATE_CONFIG = 2,
	ACTION_TYPE_NOTIFY_ROAM = 3
};


#ifdef DBG
extern unsigned long ez_memory_alloc_num;
extern unsigned long ez_memory_free_num;
#endif /* DBG */

#if 1
#ifdef EZ_DUAL_BAND_SUPPORT
#define IS_SINGLE_CHIP_DBDC(_pAd)	((_pAd)->SingleChip)
#define IS_DUAL_CHIP_DBDC(_pAd)		!(IS_SINGLE_CHIP_DBDC(_pAd))
#else
#define IS_SINGLE_CHIP_DBDC(_pAd)	1
#define IS_DUAL_CHIP_DBDC(_pAd)		0
#endif
#endif

#define MTK_VENDOR_CAPABILITY_SIZE    4
#define MTK_VENDOR_EASY_SETUP         0x40
#define MTK_OUI_LEN                   3
#define RALINK_OUI_LEN                3

#define AUTH_MODE_EZ                  0xFF01

#define EZ_DH_KEY_LEN                 32
#define EZ_NONCE_LEN                  32
#define EZ_RAW_KEY_LEN                192
#define EZ_MIC_LEN                    16
#define EZ_PMK_LEN                    32
#define EZ_PTK_LEN                    80
#define EZ_GTK_LEN                    32
#define EZ_CAPABILITY_LEN             4
#define EZ_GROUP_ID_LEN               4

#define EZ_TAG_SDH_PUBLIC_KEY         0x01
#define EZ_TAG_SNONCE                 0x02
#define EZ_TAG_ADH_PUBLIC_KEY         0x03
#define EZ_TAG_ANONCE                 0x04
#define EZ_TAG_GTK                    0x05
#define EZ_TAG_CAPABILITY_INFO        0x06
#define EZ_TAG_MIC                    0x07
#define EZ_TAG_GROUP_ID               0x08
#define EZ_TAG_PMK                    0x09
#define EZ_TAG_APCLI_ACTION_INFO      0x0a
#define EZ_TAG_PSK					  0x0b
#ifdef EZ_NETWORK_MERGE_SUPPORT
//#define EZ_TAG_SSID					  0x0b
#define EZ_TAG_NETWORK_WEIGHT		  0x0c	
#define EZ_TAG_OTHER_BAND_PMK		  0x0d
#define EZ_TAG_OTHER_BAND_PSK		  0x0e
//#define EZ_TAG_OTHER_BAND_SSID		  0x0e
#define EZ_TAG_GROUP_ID_UPDATE        0x0f
//#define EZ_TAG_TARGET_CHANNEL         0x10
#define EZ_TAG_DELAY_DISCONNECT_COUNT 0x11
#define EZ_TAG_DEVICE_INFO            0x13
#ifdef EZ_DUAL_BAND_SUPPORT
#define EZ_TAG_INTERFACE_INFO         0x12
#endif
#define EZ_TAG_BEACON_INFO         	  0x14
#define EZ_TAG_NOTIFY_ROAM			  0x15
#endif
#define EZ_TAG_NON_EZ_BEACON       	  0x16

#define EZ_TAG_TRIBAND_SEC       	  0x17

#define EZ_TAG_NODE_NUMBER			  0x20
#define EZ_TAG_AP_MAC		  0x21
#define EZ_TAG_OPEN_GROUP_ID	0x22

#define EZ_TAG_NON_EZ_CONFIG	0x23
#define EZ_TAG_NON_EZ_PSK		0x24
#define EZ_TAG_COUSTOM_DATA		0x25
#define EZ_TAG_NON_MAN_CONFIG   0x26
#define EZ_TAG_GROUPID_SEED   0x27
#define EZ_TAG_PSK_LEN				  0x28
#define EZ_TAG_OTHER_BAND_PSK_LEN				  0x29

#define EZ_TLV_TAG_SIZE               1
#define EZ_TLV_LEN_SIZE               1

#define EZ_TAG_OFFSET                 (MTK_OUI_LEN+MTK_VENDOR_CAPABILITY_SIZE)
#define EZ_TAG_LEN_OFFSET             (MTK_OUI_LEN+MTK_VENDOR_CAPABILITY_SIZE+EZ_TLV_TAG_SIZE)
#define EZ_TAG_DATA_OFFSET            (MTK_OUI_LEN+MTK_VENDOR_CAPABILITY_SIZE+EZ_TLV_TAG_SIZE+EZ_TLV_LEN_SIZE)

#define EZ_BEACON_TAG_COUNT           1
#ifdef EZ_NETWORK_MERGE_SUPPORT
#define EZ_PROB_REQ_TAG_COUNT         2 //! aditional, capabilities tag required in preobe-response
#define EZ_UDATE_CONFIG_TAG_COUNT     12
#else 
#define EZ_PROB_REQ_TAG_COUNT         1
#endif
#define EZ_PROB_RSP_TAG_COUNT         2
#define EZ_AUTH_REQ_TAG_COUNT         4
#define EZ_AUTH_RSP_TAG_COUNT         6
#define EZ_ASSOC_REQ_TAG_COUNT        4

#define EZ_ASSOC_RSP_TAG_COUNT        8//! other band PMK, SSID and network weight


#define EZ_STATUS_CODE_SUCCESS         0x0000
#define EZ_STATUS_CODE_MIC_ERROR       0x0001
#define EZ_STATUS_CODE_INVALID_DATA    0x0002
#define EZ_STATUS_CODE_NO_RESOURCE     0x0003
#define EZ_STATUS_CODE_PEER_CONNECTED  0x0004
#define EZ_STATUS_CODE_LOOP			   0x0005
#define EZ_STATUS_CODE_UNKNOWN         0xFFFF

#define EZ_MAX_STA_NUM 8

#define EZ_AES_KEY_ENCRYPTION_EXTEND	8 /* 8B for AES encryption extend size */

#define EZ_PAUSE_SCAN_TIME_OUT          1000 /* 1 second */
#define EZ_MAX_SCAN_TIME_OUT            10000 /* 10 seconds */
#define EZ_STOP_SCAN_TIME_OUT           120000 /* 120 seconds */
#ifdef EZ_NETWORK_MERGE_SUPPORT
#define EZ_GROUP_MERGE_TIMEOUT          120000 /* 120 seconds */
#endif
#ifdef EZ_DUAL_BAND_SUPPORT
#define EZ_LOOP_CHK_TIMEOUT_10S          	10000 /* 10 seconds */
#define EZ_LOOP_CHK_TIMEOUT_5S          	5000 /* 5 seconds */
#endif

#define EZ_UNCONFIGURED                 1
#define EZ_CONFIGURED                   2
#define EZ_DEFAULT_RSSI_THRESHOLD		(-70)



#define EZ_INDEX_NOT_FOUND              0xFF

#define EZ_CAP_CONFIGURED 			(1 << 0)
#define EZ_CAP_INTERNET   			(1 << 1)
#define EZ_CAP_MEMBER_COUNT(__x) 	(__x << 2)
#define EZ_CAP_CONNECTED  			(1 << 11)
#ifdef  EZ_NETWORK_MERGE_SUPPORT
#define EZ_CAP_ALLOW_MERGE  		(1 << 12)
#define EZ_CAP_AP_CONFIGURED        (1 << 13)
#endif
#define EZ_SET_CAP_CONFIGRED(__cap) (__cap |= EZ_CAP_CONFIGURED)
#define EZ_SET_CAP_INTERNET(__cap) (__cap |= EZ_CAP_INTERNET)
#define EZ_SET_CAP_CONNECTED(__cap) (__cap |= EZ_CAP_CONNECTED)
#ifdef EZ_NETWORK_MERGE_SUPPORT
#define EZ_SET_CAP_ALLOW_MERGE(__cap) (__cap |= EZ_CAP_ALLOW_MERGE)
#endif
#define EZ_SET_CAP_AP_CONFIGURED(__cap) (__cap |= EZ_CAP_AP_CONFIGURED)

#define EZ_CLEAR_CAP_CONFIGRED(__cap) (__cap &= 0xFFFFFFFE)
#define EZ_CLEAR_CAP_INTERNET(__cap) (__cap &= 0xFFFFFFFD)
#define EZ_CLEAR_CAP_CONNECTED(__cap) (__cap &= 0xFFFFF7FF)
#ifdef EZ_NETWORK_MERGE_SUPPORT
#define EZ_CLEAR_CAP_ALLOW_MERGE(__cap) (__cap &= 0xFFFFEFFF)
#endif
#define EZ_CLEAR_CAP_AP_CONFIGURED(__cap) (__cap &= 0xFFFFEFFF)

#define EZ_GET_CAP_CONFIGRED(__cap) (__cap & EZ_CAP_CONFIGURED)
#define EZ_GET_CAP_INTERNET(__cap) (__cap & EZ_CAP_INTERNET)
#define EZ_GET_CAP_CONNECTED(__cap) (__cap & EZ_CAP_CONNECTED)
#ifdef EZ_NETWORK_MERGE_SUPPORT
#define EZ_GET_CAP_ALLOW_MERGE(__cap) (__cap & EZ_CAP_ALLOW_MERGE)
#endif
#define EZ_GET_CAP_AP_CONFIGURED(__cap) (__cap & EZ_CAP_AP_CONFIGURED)

#define EZ_CLEAR_ACTION 0
#define EZ_SET_ACTION 1
typedef void* PNDIS_PACKET;


#define EZ_DROP_GROUP_DATA_BAND24G			0
#define EZ_DROP_GROUP_DATA_BAND5G			1
#if 0
#define EZ_UPDATE_CAPABILITY_INFO(__ad, __action, __cap_item, __inf_idx) \
{ \
	do { \
		if (!(__ad->ApCfg.MBSSID[__inf_idx].wdev.enable_easy_setup)) \
			break; \
		if (__action == EZ_SET_ACTION) \
			EZ_SET_CAP_ ## __cap_item((__ad->ApCfg.MBSSID[__inf_idx].wdev.ez_security.capability)); \
		else \
			EZ_CLEAR_CAP_ ## __cap_item((__ad->ApCfg.MBSSID[__inf_idx].wdev.ez_security.capability)); \
		UpdateBeaconHandler(__ad, &(__ad->ApCfg.MBSSID[__inf_idx].wdev), IE_CHANGE); \
	} while(0); \
}
#define EZ_UPDATE_APCLI_CAPABILITY_INFO(__ad, __action, __cap_item, __inf_idx) \
{ \
	do { \
		if (!(__ad->ApCfg.ApCliTab[__inf_idx].wdev.enable_easy_setup)) \
			break; \
		if (__action == EZ_SET_ACTION) \
			EZ_SET_CAP_ ## __cap_item((__ad->ApCfg.ApCliTab[__inf_idx].wdev.ez_security.capability)); \
		else \
			EZ_CLEAR_CAP_ ## __cap_item((__ad->ApCfg.ApCliTab[__inf_idx].wdev.ez_security.capability)); \
	} while(0); \
}
#else
#define EZ_UPDATE_CAPABILITY_INFO(ezdev, __action, __cap_item) \
{ \
	do { \
		if (__action == EZ_SET_ACTION) \
			EZ_SET_CAP_ ## __cap_item((ezdev->ez_security.capability)); \
		else \
			EZ_CLEAR_CAP_ ## __cap_item((ezdev->ez_security.capability)); \
		ezdev->driver_ops->UpdateBeaconHandler(ezdev, IE_CHANGE); \
	} while(0); \
}
#define EZ_UPDATE_APCLI_CAPABILITY_INFO(__ad, __action, __cap_item, __inf_idx) \
{ \
	do { \
		if (__action == EZ_SET_ACTION) \
			EZ_SET_CAP_ ## __cap_item((__ad->ez_band_info[__inf_idx].cli_ezdev.ez_security.capability)); \
		else \
			EZ_CLEAR_CAP_ ## __cap_item((__ad->ez_band_info[__inf_idx].cli_ezdev.ez_security.capability)); \
	} while(0); \
}

#endif


#define DBG_CAT_ALL     0xFFFFFFFFu
#define DBG_SUBCAT_ALL	0xFFFFFFFFu


/* Debug Level */
#define DBG_LVL_OFF     0
#define DBG_LVL_ERROR   1
#define DBG_LVL_WARN    2
#define DBG_LVL_TRACE   3
#define DBG_LVL_INFO    4
#define DBG_LVL_LOUD    5
#define DBG_LVL_NOISY   6
#define DBG_LVL_MAX     DBG_LVL_NOISY

#define EZ_DEBUG(__debug_cat, __debug_sub_cat, __debug_level, __fmt) \
do{ \
		if (__debug_level <= ez_adapter->debug) \
			printk __fmt;\
}while(0)


#define ASSERT(x)                                                               \
{                                                                               \
    if (!(x))                                                                   \
    {                                                                           \
        printk(__FILE__ ":%d assert " #x "failed\n", __LINE__);    		\
        dump_stack();\
	/*panic("Unexpected error occurs!\n");					*/ \
    }                                                                           \
}


#define MEDIATEK_EASY_SETUP (1 << 6)

typedef struct GNU_PACKED _ie_hdr {
    UCHAR eid;
    UINT8 len;
} IE_HEADER;

struct GNU_PACKED _mediatek_ie {
    IE_HEADER ie_hdr;
    UCHAR oui[3];
    UCHAR cap0;
    UCHAR cap1;
    UCHAR cap2;
    UCHAR cap3;
};



#ifdef DBG
#define EZ_MEM_ALLOC(__ad, __ptr, __size) \
	ez_os_alloc_mem(__ad, (UCHAR **)(__ptr), __size); \
	ez_memory_alloc_num++;
#define EZ_MEM_FREE(__ptr) \
	ez_os_free_mem( __ptr); \
	ez_memory_free_num++;
#else /* DBG */
#define EZ_MEM_ALLOC(__ad, __ptr, __size) \
	ez_os_alloc_mem(__ad, (UCHAR **)(__ptr), __size);
#define EZ_MEM_FREE(__ptr) \
	ez_os_free_mem( __ptr);
#endif /* !DBG */


#define EZ_INFO_PASS_DELAY_MSEC  1000
#define OPEN_GROUP_MAX_LEN		20


#define EZ_IRQ_LOCK(__lock, __irqflags)                        \
{																\
	__irqflags = 0;												\
	OS_SEM_LOCK(__lock); 		\
}

#define EZ_IRQ_UNLOCK(__lock, __irqflag)                       \
{																\
	OS_SEM_UNLOCK(__lock);	\
}

/***********************************************************************************
 *	OS Memory Access related data structure and definitions
 ***********************************************************************************/
#define MEM_ALLOC_FLAG      (GFP_ATOMIC) /*(GFP_DMA | GFP_ATOMIC) */

#define NdisMoveMemory(Destination, Source, Length) memmove(Destination, Source, Length)
#define NdisCopyMemory(Destination, Source, Length) memcpy(Destination, Source, Length)
#define NdisZeroMemory(Destination, Length)         memset(Destination, 0, Length)
#define NdisFillMemory(Destination, Length, Fill)   memset(Destination, Fill, Length)
#define NdisCmpMemory(Destination, Source, Length)  memcmp(Destination, Source, Length)
#define NdisEqualMemory(Source1, Source2, Length)   (!memcmp(Source1, Source2, Length))
#define RTMPEqualMemory(Source1, Source2, Length)	(!memcmp(Source1, Source2, Length))

#define MlmeAllocateMemory(_pAd, _ppVA)		os_alloc_mem(_pAd, _ppVA, MGMT_DMA_BUFFER_SIZE)
#define MlmeFreeMemory( _pVA)			os_free_mem(_pVA)

#define COPY_MAC_ADDR(Addr1, Addr2)             memcpy((Addr1), (Addr2), MAC_ADDR_LEN)

typedef struct GNU_PACKED internet_command_s
{
	BOOLEAN Net_status;
	
} internet_command_t, *p_internet_command_t;


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
} FRAME_CONTROL, *PFRAME_CONTROL;

typedef struct  _HEADER_802_11 {
    FRAME_CONTROL FC;
    USHORT        Duration;
    UCHAR         Addr1[MAC_ADDR_LEN];
    UCHAR         Addr2[MAC_ADDR_LEN];
    UCHAR         Addr3[MAC_ADDR_LEN];
    USHORT        Frag : 4;
    USHORT        Sequence : 12;
} HEADER_802_11, *PHEADER_802_11;

typedef struct _FRAME_802_11 {
    HEADER_802_11 Hdr;
    UCHAR         Octet[1];
} FRAME_802_11, *PFRAME_802_11;


#define CATEGORY_PUBLIC		4
#define ACTION_WIFI_DIRECT					9 	/* 11y */

typedef struct _FRAME_ACTION_HDR {
    HEADER_802_11 Hdr;
    UCHAR         Category;
    UCHAR         Action;
} FRAME_ACTION_HDR, *PFRAME_ACTION_HDR;

typedef struct GNU_PACKED _EID_STRUCT{
    UCHAR   Eid;
    UCHAR   Len;
    UCHAR   Octet[1];
} EID_STRUCT,*PEID_STRUCT, BEACON_EID_STRUCT, *PBEACON_EID_STRUCT;
#define IE_VENDOR_SPECIFIC              221	/* Wifi WMM (WME) */

#ifdef EZ_NETWORK_MERGE_SUPPORT
typedef enum inform_other_band_action_e
{
	ACTION_UPDATE_DUPLICATE_LINK_ENTRY,
	ACTION_UPDATE_DEVICE_INFO,
	ACTION_UPDATE_CONFIG_STATUS,
	ACTION_UPDATE_INTERNET_STATUS
	
}inform_other_band_action_t;

//! Levarage from MP1.0 CL 170210

#endif

#ifdef EZ_DUAL_BAND_SUPPORT


#define BEST_AP_RSSI_THRESHOLD_LEVEL_MAX 			ez_adapter->best_ap_rssi_threshld_max
 

#endif


typedef struct GNU_PACKED ez_custom_data_cmd_s {
	UINT8 data_len;
	UINT8 data_body[0];
}ez_custom_data_cmd_t, *p_ez_custom_data_cmd_t;

typedef struct GNU_PACKED ez_custom_evt_s {
	UINT8 data_len;
	UINT8 data_body[0];
}ez_custom_evt_t, *p_ez_custom_evt_t;



typedef struct device_info_to_app_s
{
	unsigned char dual_chip_dbdc;
	unsigned char ssid_len1;
	unsigned char ssid_len2;
	unsigned char internet_access;
	char ssid1[MAX_LEN_OF_SSID];
	char ssid2[MAX_LEN_OF_SSID];
	unsigned char pmk1[LEN_PMK];
	unsigned char pmk2[LEN_PMK];
	unsigned char device_connected[2];
	unsigned char network_weight[NETWORK_WEIGHT_LEN];
	EZ_NODE_NUMBER node_number;
	char peer2p4mac[MAC_ADDR_LEN];
	unsigned char non_ez_connection;
	unsigned char update_parameters;
	unsigned char is_forced;
	unsigned char third_party_present;
	unsigned char new_updated_received;
	unsigned char is_push;	
	unsigned char sta_cnt;
	unsigned char stamac[10][MAC_ADDR_LEN];	
	
} device_info_to_app_t;

typedef struct triband_ez_device_info_to_app_s
{
	unsigned char ssid_len;
	unsigned char internet_access;
	unsigned char is_non_ez_connection;
	char ssid[MAX_LEN_OF_SSID];
	char non_ez_ssid1[MAX_LEN_OF_SSID];
	char non_ez_ssid2[MAX_LEN_OF_SSID];
	unsigned char non_ez_ssid1_len;
	unsigned char non_ez_ssid2_len;
	unsigned char need_non_ez_update_ssid[2];
	unsigned char pmk[LEN_PMK];
	unsigned char network_weight[NETWORK_WEIGHT_LEN];
	EZ_NODE_NUMBER node_number;
	char peer_mac[MAC_ADDR_LEN];
	unsigned char is_forced;	
	unsigned char update_parameters;
	unsigned char third_party_present;
	unsigned char new_updated_received;
} triband_ez_device_info_to_app_t;


typedef struct triband_non_ez_device_info_to_app_s
{
	unsigned char non_ez_psk1[EZ_LEN_PSK];
	unsigned char non_ez_psk2[EZ_LEN_PSK];
	unsigned char non_ez_auth_mode1[20];
	unsigned char non_ez_auth_mode2[20];	
	unsigned char non_ez_encryptype1[20];
	unsigned char non_ez_encryptype2[20];		
	unsigned char need_non_ez_update_psk[2];
	unsigned char need_non_ez_update_secconfig[2];
} triband_nonez_device_info_to_app_t;
//! Levarage from MP1.0 CL #170037
typedef struct man_plus_nonman_ez_device_info_to_app_s
{
	unsigned char ssid_len;
	unsigned char internet_access;
	char is_non_ez_connection;
	char ssid[MAX_LEN_OF_SSID];
	unsigned char pmk[LEN_PMK];
	unsigned char network_weight[NETWORK_WEIGHT_LEN];
	EZ_NODE_NUMBER node_number;
	char peer_mac[MAC_ADDR_LEN];	
	char update_parameters;
	char third_party_present;
	char new_updated_received;
} man_plus_nonman_ez_device_info_to_app_t;


typedef struct man_plus_nonman_non_ez_device_info_to_app_s
{
	unsigned char non_ez_ssid[MAX_LEN_OF_SSID];
//! Leverage form MP.1.0 CL 170364
	unsigned char non_ez_ssid_len;
	unsigned char non_ez_psk[EZ_LEN_PSK];
//! Leverage form MP.1.0 CL 170364
	unsigned char non_ez_encryptype[32];
	unsigned char non_ez_auth_mode[32];
	UINT8 ftmdid[FT_MDID_LEN];
//! Leverage form MP.1.0 CL 170364
	unsigned char need_non_ez_update_ssid;
	unsigned char need_non_ez_update_psk;
	unsigned char need_non_ez_update_secconfig;
} man_plus_nonman_nonez_device_info_to_app_t;

extern EZ_ADAPTER *ez_adapter;

void ez_hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen);
ULONG EzMakeOutgoingFrame(UCHAR *Buffer, ULONG *FrameLen, ...);

#ifdef EZ_NETWORK_MERGE_SUPPORT	


#define GET_MAX_UCAST_NUM(_pAd, _band_num) _pAd->ez_band_info[_band_num].HcGetMaxStaNum(_pAd->ez_band_info[_band_num].pAd)
#define BROADCAST_WCID          0xFF
typedef enum enum_config_update_action
{
	ACTION_NOTHING,
	ACTION_PUSH,
	ACTION_ADAPT	
		
}enum_config_update_action_t;
typedef enum enum_group_merge_action
{
	EXIT_SWITCH_NOT_GROUP_MERGE,
	TERMINATE_LOOP_MULTIPLE_AP_FOUND,
	TERMINATE_LOOP_TARGET_AP_FOUND,
	CONTINUE_LOOP_TARGET_AP_FOUND,
	CONTINUE_LOOP
}enum_group_merge_action_t;
#endif



#define EZ_WAIT_FOR_INFO_TRANSFER 1000 // 1 sec
#define EZ_WAIT_FOR_ROAM_COMPLETE 60000 // 60 sec
#define EZ_MAX_SCAN_DELAY 30000
#define EZ_SCAN_DELAY_WAIT 10000
#define EZ_SEC_TO_MSEC 1000 // 1 sec

#define EZ_DELAY_DISCONNECT_FOR_PBC 4
#define MAC_ADDR_IS_GROUP(Addr)       (((Addr[0]) & 0x01))

enum EZ_CONN_ACTION
{
	EZ_ALLOW_ALL,
	EZ_DISALLOW_ALL,
	EZ_ADD_DISALLOW,
	EZ_ADD_ALLOW,
	EZ_DISALLOW_ALL_ALLOW_ME,
	EZ_ALLOW_ALL_TIMEOUT,
	EZ_ENQUEUE_PERMISSION,
	EZ_DEQUEUE_PERMISSION,
};

#define PRINT_MAC(addr) \
    addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

/* value domain of 802.11 header FC.Tyte, which is b3..b2 of the 1st-byte of MAC header */
#define FC_TYPE_MGMT	0
#define FC_TYPE_CNTL	1
#define FC_TYPE_DATA	2
#define FC_TYPE_RSVED	3
VOID EzActHeaderInit(
   	PHEADER_802_11 pHdr80211,
    UCHAR *da,
    UCHAR *sa,
    UCHAR *bssid);


#define CIPHER_TEXT_LEN                 128

typedef struct _AUTH_FRAME_INFO{
	UCHAR addr1[MAC_ADDR_LEN];
	UCHAR addr2[MAC_ADDR_LEN];
	USHORT auth_alg;
	USHORT auth_seq;
	USHORT auth_status;
	CHAR Chtxt[CIPHER_TEXT_LEN];
#ifdef DOT11R_FT_SUPPORT
	FT_INFO FtInfo;
#endif /* DOT11R_FT_SUPPORT */
}AUTH_FRAME_INFO;

#ifndef _LINUX_BITOPS_H
#define BIT(n)                          ((UINT32) 1 << (n))
#endif /* BIT */


#ifdef DOT11_N_SUPPORT
#define HT_BW_20                         0
#define HT_BW_40                         1
#endif /* DOT11_N_SUPPORT */

#define EXTCHA_NONE                                                0
#define EXTCHA_ABOVE                              0x1
#define EXTCHA_BELOW                              0x3


#define FT_SET_MDID(__D, __S) \
        NdisMoveMemory((PUCHAR)(__D), (PUCHAR)(__S), FT_MDID_LEN)

#endif /* WH_EZ_SETUP */
#endif/* __EZ_CMM_H__ */
