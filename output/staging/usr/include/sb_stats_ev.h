#ifndef _SB_STATS_EV_
#define _SB_STATS_EV_

typedef enum
{
    MSG_SB_REPLY_SET_CFG = 0x1A01,              /* MSG06: Reply Config Result Southbound Message */
    MSG_SB_REPLY_GET_CFG,                       /* MSG10: Reply Cared Config Southbound Message */
    MSG_SB_REPLY_IMAGE_CHECK,                   /* MSG33: Reply Image Check Southbound Message */
    MSG_SB_REPLY_IMAGE_UPGRADE_RESULT,          /* MSG45: Reply Image Upgrade Result Southbound Message */
    MSG_SB_REPLY_SET_AP_INFO,                   /* ADD: Reply Set Ap Information Result */
    MSG_SB_REPLY_FW_STATUS,                     /* ADD: Reply Set Firmware Status Result */
    MSG_SB_REPLY_MEMBER_LIST,                   /* ADD: Reply Member List */
    MSG_SB_REPLY_STATS_LAST
}SB_STATS_MSG_E;

typedef enum
{
    CMD_SB_SET_CFG = 0x1B01,                    /* MSG03: Set Config Change AP SON Command */
    CMD_SB_SET_REBOOT_AP,                       /* MSG14: Set Reboot AP AP SON Command */
    CMD_SB_KICK_OUT,
    CMD_SB_IMAGE_CHECK,                         /* MSG31: Check Image AP SON Command */
    CMD_SB_IMAGE_UPGRADE,                       /* MSG42: Set AP Image Upgrade AP SON Command */
    CMD_SB_SET_LED,
#ifdef _SUPER_MESH_
    CMD_SB_SET_TIME_SCHEDULE,
    CMD_SB_SET_DEVICE_MANAGEMENT,
    CMD_SB_SET_SERVICE_WLAN,
    CMD_SB_SET_GUEST_WLAN,
    CMD_SB_SET_WPS,
    CMD_SB_SET_NTP,
    CMD_SB_SET_DEVICE_SETTING,
#endif
#ifdef _SUPER_MESH_PRO_
    CMD_SB_SET_WLAN,                            /* Others: Set wlan for SuperMeshPro */
    CMD_SB_SET_RADIO,                           /* Others: Set radio for SuperMeshPro */
    CMD_SB_SET_MESH,                            /* Others: Set mesh for SuperMeshPro */
    CMD_SB_SET_CP_PROTAL,                       /* Others: Set captive portal for SuperMeshPro */
    CMD_SB_SET_ACL,                             /* Others: Set acl for SuperMeshPro */
    CMD_SB_SET_QOS,                             /* Others: Set qos for SuperMeshPro */
    CMD_SB_SET_VLAN,                            /* Others: Set vlan for SuperMeshPro */
    CMD_SB_SET_ARP_FIT,                         /* Others: Set arp filter for SuperMeshPro */
    CMD_SB_SET_TM_SCHE,                         /* Others: Set time schedule for SuperMeshPro */
    CMD_SB_SET_WLAN_PRTN,                       /* Others: Set wlan partition for SuperMeshPro */
    CMD_SB_SET_DVC_MGMT,                        /* Others: Set device management for SuperMeshPro */
    CMD_SB_SET_AP_ARRAY,                        /* Others: Set device management for SuperMeshPro */
    CMD_SB_SET_LOCAL_AP_ARRAY,
    CMD_SB_RESET_AP_ARRAY,
    CMD_SB_SET_MESH_CONFIG,
    CMD_SB_GET_MEMBER,
#endif
    CMD_SB_CHANGE_MASTER_EVENT,                 /* NEW, Apson change master */
    CMD_SB_SET_PROVISION_LED,
    CMD_SB_SET_AP_INFO,
    CMD_SB_SET_RESET_TO_DEFAULT,
    CMD_SB_SET_CHANNEL,
    CMD_SB_DOWNLOAD_FW,
    CMD_SB_GET_FW_STATUS,
    CMD_SB_STATS_LAST
}STATS_SB_CMD_E;

#endif /* _SB_STATS_EV_ */
