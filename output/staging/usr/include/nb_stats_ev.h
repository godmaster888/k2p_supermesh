#ifndef _NB_STATS_EV_
#define _NB_STATS_EV_

typedef enum
{
#ifdef _SUPER_MESH_
    CMD_NB_SET_NEW_CFG = 0x1801,                /* MSG02: Set New Config Northbound Command */
    CMD_NB_SET_REBOOT_AP,                       /* MSG12: Set Reboot AP Northbound Command */
#endif
#ifdef _SUPER_MESH_PRO_
    CMD_NB_SET_REBOOT_AP = 0x1801,              /* MSG12: Set Reboot AP Northbound Command */
#endif
    CMD_NB_SET_KICKOUT_CLIENT,                  /* MSG17: Set Kickout Client Northbound Command */
    CMD_NB_SET_AP_SYSTEM,                       /* MSG21: Set AP System Northbound Command */
    CMD_NB_CHECK_IMG,                           /* MSG29: Check Image Northbound Command */
    CMD_NB_GET_AP_IMG_INFO,                     /* MSG37: Get AP Image Info Northbound Command */
    CMD_NB_SET_AP_IMG_UPGRADE,                  /* MSG40: Set AP Image Upgrade Northbound Command */
    CMD_NB_GET_AP_UPGRADE_STATUS,               /* MSG48: Get AP Upgrade Status Northbound Command */
    CMD_NB_GET_AP_STATS,                        /* MSG75: Get AP Stats Northbound Command */
    CMD_NB_GET_WLAN_STATS,                      /* MSG76: Get Wlan Stats Northbound Command */
    CMD_NB_GET_STA_STATS,                       /* MSG77: Get Sta Stats Northbound Command */
    CMD_NB_GET_MESH_STATS,                      /* MSG78: Get Mesh Stats Northbound Command */
    CMD_NB_GET_SYS_WIDTH_STATS,                 /* MSG79: Get System-Width Stats Northbound Command */
    CMD_NB_COMMIT_NEW_CFG,                      /* Others: Commit New Config*/
    CMD_NB_GET_MASTER_IP,                       /* Others: Get Master IP*/
    CMD_NB_GET_RADIO_STATS,                     /* Othres: Get Radio Stats Northbound Command*/
    CMD_NB_GET_CLIENT_ACTIVITY,                 /* Othres: Get Client Activity Northbound Command*/
    CMD_NB_GET_PROFILE_INFO,                    /* Others: Get Profile information*/
#ifdef _SUPER_MESH_
    CMD_NB_SET_TIME_SCHEDULE_RULE,              /* Others: Set time schedule rules */
    CMD_NB_SET_DEV_MGMT_LIST,                   /* Others: Set device management list */
    CMD_NB_SET_SERVICE_WLAN,                    /* Others: Set service WLAN settings */
    CMD_NB_SET_GUEST_WLAN,                      /* Others: Set guest WLAN settings */
    CMD_NB_SET_WPS,                             /* Others: Set WPS settings */
    CMD_NB_SET_NTP,                             /* Others: Set NTP settings */
    CMD_NB_SET_DEV_SETTING,                     /* Others: Set Admin username/password */
#endif
    CMD_NB_GET_MESH_DL_AP_ACT,                  /* Others: Get Mesh Downlink AP Activity Command */
    CMD_NB_SET_AP_INFO,                         /* Others: Set AP information Command */
    CMD_NB_RSET_TO_DEF,                         /* Others: Reset to default Command */
    CMD_NB_SET_SITE_SURVEY,                     /* Others: Set site survey test command */
    CMD_NB_TEST_FUN,                            /* Others: Test function for debug */
#ifdef _SUPER_MESH_PRO_
    CMD_NB_SET_WLAN,                            /* Others: Set wlan for SuperMeshPro */
    CMD_NB_SET_RADIO,                           /* Others: Set radio for SuperMeshPro */
    CMD_NB_SET_MESH,                            /* Others: Set mesh for SuperMeshPro */
    CMD_NB_SET_CP_PROTAL,                       /* Others: Set captive portal for SuperMeshPro */
    CMD_NB_SET_ACL,                             /* Others: Set acl for SuperMeshPro */
    CMD_NB_SET_QOS,                             /* Others: Set qos for SuperMeshPro */
    CMD_NB_SET_VLAN,                            /* Others: Set vlan for SuperMeshPro */
    CMD_NB_SET_ARP_FIT,                         /* Others: Set arp filter for SuperMeshPro */
    CMD_NB_SET_TM_SCHE,                         /* Others: Set time schedule for SuperMeshPro */
    CMD_NB_SET_WLAN_PRTN,                       /* Others: Set wlan partition for SuperMeshPro */
    CMD_NB_SET_DVC_MGMT,                        /* Others: Set device management for SuperMeshPro */
    CMD_NB_SET_CONFIG,                          /* Others: Set all config for SuperMeshPro */
    CMD_NB_ADD_APPROVE_LIST,                    /* Others: Add AP into approve list */
    CMD_NB_DEL_APPROVE_LIST,                    /* Others: Delete AP from approve list */
    CMD_NB_GET_PENDING_LIST,                    /* Others: Get pending list */
    CMD_NB_SET_MESH_LOCAL_SET,                  /* Others: Set mesh local setting */
    CMD_NB_SET_APARRAY_MASTER_SYNC,             /* Others: Set ap array master sync information */
    CMD_NB_SET_APARRAY_MASTER_LOCAL,            /* Others: Set ap array master local information */
    CMD_NB_SET_APARRAY_MEMBER_LOCAL,            /* Others: Set ap array member local information */
    CMD_NB_RESET_AP_ARRAY_SETTING,              /* Others: Reset ap array setting */
#endif
    CMD_NB_GET_ID,                              /* Others: Get current ID(master: TRUE member:FALSE) */
    CMD_NB_GET_MASTER_INFO,                     /* Others: Get master info */
    CMD_NB_SET_MESH_SNR_THRESHOLD,              /* Others: Set mesh snr threshold in apson profile */
    CMD_NB_STATS_LAST                           /* Others: Last null event */
}NB_STATS_CMD_E;

typedef enum
{
    MSG_NB_REPLY_AP_IMG_INFO = 0x1901,          /* MSG38: Reply AP Image Info AP SON Stats */
    MSG_NB_REPLY_AP_UPGRADE_STATUS,             /* MSG49: Reply AP Upgrade Status AP SON Stats */
    MSG_NB_REPLY_AP_STATS,                      /* MSG80: Reply AP Stats AP SON Stats */
    MSG_NB_REPLY_WLAN_STATS,                    /* MSG81: Reply Wlan Stats AP SON Stats */
    MSG_NB_REPLY_STA_STATS,                     /* MSG82: Reply Sta Stats AP SON Stats */
    MSG_NB_REPLY_MESH_STATS,                    /* MSG83: Reply Mesh Stats AP SON Stats */
    MSG_NB_REPLY_SYS_WIDTH_STATS,               /* MSG84: Reply System-Width Stats AP SON Stats */
#ifdef _SUPER_MESH_
    MSG_NB_REPLY_SET_NEW_CFG,                   /* Others: Reply Set New Config*/
#endif
    MSG_NB_REPLY_COMMIT_NEW_CFG,                /* Others: Reply commit New Config*/
    MSG_NB_REPLY_REBOOT_AP,                     /* Others: Reply Reboot AP*/
    MSG_NB_REPLY_KICKOUT_CLIENT,                /* Others: Reply Kickout client*/
    MSG_NB_REPLY_MASTER_IP,                     /* Others: Reply Master IP*/
    MSG_NB_REPLY_RADIO_STATS,                   /* Others: Reply Radio Stats AP SON Stats*/
    MSG_NB_REPLY_CLIENT_ACTIVITY,               /* Others: Reply Client Activity*/
    MSG_NB_REPLY_PROFILE_INFO,                  /* Others: Reply Profile information*/
#ifdef _SUPER_MESH_
    MSG_NB_REPLY_TIME_SCHEDULE_RULE,            /* Others: Reply time schedule rules */
    MSG_NB_REPLY_DEV_MGMT_LIST,                 /* Others: Reply device management list */
    MSG_NB_REPLY_SERVICE_WLAN,                  /* Others: Reply service WLAN settings */
    MSG_NB_REPLY_GUEST_WLAN,                    /* Others: Reply guest WLAN settings */
    MSG_NB_REPLY_WPS,                           /* Others: Reply WPS settings */
    MSG_NB_REPLY_NTP,                           /* Others: Reply NTP settings */
    MSG_NB_REPLY_DEV_SETTING,                   /* Others: Reply Admin username/password */
#endif
    MSG_NB_REPLY_WLAN_ONE_SHOT,                 /* Others: Reply Wlan Stats APSON Stats One-shot data*/
    MSG_NB_REPLY_AP_ONE_SHOT,                   /* Others: Reply AP Stats APSON Stats One-shot data */
    MSG_NB_REPLY_STA_ONE_SHOT,                  /* Others: Reply Station Stats APSON Stats One-shot data */
    MSG_NB_REPLY_MESH_STATS_ONE_SHOT,           /* Others: Reply Mesh Stats AP SON Stats One-shot data */
    MSG_NB_REPLY_MESH_DL_AP_ACT,                /* Others: Reply Mesh Downlink AP Activity */
    MSG_NB_REPLY_SET_AP_INFO,                   /* Others: Reply setting AP information */
    MSG_NB_RESET_TO_DEF,                        /* Others: Reply reset to default */
    MSG_NB_REPLY_SET_SITE_SURVEY,               /* Others: Reply site survey test command */
    MSG_NB_TEST_FUN,                            /* Others: Reply test function message */
#ifdef _SUPER_MESH_PRO_
    MSG_NB_REPLY_WLAN,                          /* Others: Reply wlan for SuperMeshPro */
    MSG_NB_REPLY_RADIO,                         /* Others: Reply radio for SuperMeshPro */
    MSG_NB_REPLY_MESH,                          /* Others: Reply mesh for SuperMeshPro */
    MSG_NB_REPLY_CP_PROTAL,                     /* Others: Reply captive portal for SuperMeshPro */
    MSG_NB_REPLY_ACL,                           /* Others: Reply acl for SuperMeshPro */
    MSG_NB_REPLY_QOS,                           /* Others: Reply qos for SuperMeshPro */
    MSG_NB_REPLY_VLAN,                          /* Others: SReply vlan for SuperMeshPro */
    MSG_NB_REPLY_ARP_FIT,                       /* Others: Reply arp filter for SuperMeshPro */
    MSG_NB_REPLY_TM_SCHE,                       /* Others: Reply time schedule for SuperMeshPro */
    MSG_NB_REPLY_WLAN_PRTN,                     /* Others: Reply wlan partition for SuperMeshPro */
    MSG_NB_REPLY_DVC_MGMT,                      /* Others: Reply device management for SuperMeshPro */
    MSG_NB_REPLY_CONFIG,                        /* Others: Reply all config for SuperMeshPro */
    MSG_NB_REPLY_ADD_APPROVE_LIST,              /* Others: Reply Add AP into approve list */
    MSG_NB_REPLY_DEL_APPROVE_LIST,              /* Others: Reply Delete AP from approve list */
    MSG_NB_REPLY_GET_PENDING_LIST,              /* Others: Reply Get pending list */
    MSG_NB_REPLY_SET_MESH_LOCAL_SET,            /* Others: Set mesh local setting */
    MSG_NB_SET_APARRAY_MASTER_SYNC,             /* Others: Reply Set ap array master sync information */
    MSG_NB_SET_APARRAY_MASTER_LOCAL,            /* Others: Reply Set ap array master local information */
    MSG_NB_SET_APARRAY_MEMBER_LOCAL,            /* Others: Reply Set ap array member local information */
    MSG_NB_RESET_AP_ARRAY_SETTING,              /* Others: Reset ap array setting */
#endif
    MSG_NB_REPLY_GET_ID,                        /* Others: Reply get current ID(master: TRUE member:FALSE)*/
    MSG_NB_REPLY_GET_MASTER_INFO,               /* Others: Reply get master info */
    MSG_NB_REPLY_SET_MESH_SNR_THRESHOLD,         /* Others: Reply set mesh snr threshold in apson profile */
    MSG_NB_STATS_LAST                           /* Others: Reply last null event */
}STATS_NB_MSG_E;


#endif /* _NB_STATS_EV_ */
