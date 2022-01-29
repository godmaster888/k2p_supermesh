#ifndef _MESH_M_STATS_EV_
#define _MESH_M_STATS_EV_

typedef enum
{
    CMD_CLIENT_ROAM_OUT = 0x1001,                       /* MM06: Client Roam Out */
    CMD_CLIENT_KICK_OFF,                                /* MM07: Client Kick Off */
    CMD_CLIENT_LOCK_UP,                                 /* MM08: Client Lock Up */
    CMD_MESH_DOWNLINK_ROAM_OUT,                         /* MM38: Mesh Downlink Roam Out */
    CMD_GET_RADIO_STATS,                                /* MM39: Get Radio Stats */
    CMD_GET_VAP_STATS,                                  /* MM40: Get VAP Stats */
    CMD_GET_STA_STATS,                                  /* MM41: Get Sta Stats */
    CMD_GET_MESH_DOWNLINK_STATS,                        /* MM44: Get Mesh Downlink Stats */
    CMD_GET_MESH_DOWNLINK_WLAN_STATS,                   /* MM45: Get Mesh Downlink Wlan Stats */
    CMD_WLAN_GET_LIST,                                  /* MM53: Wlan Get List */
    CMD_RRM_SET_SITE_SURVEY,                            /* MM34: RRM Set Site Survey */
    CMD_RRM_GET_SITE_SURVEY,                            /* MM35: RRM Get Site Survey */
    CMD_RRM_SET_NEW_CHANNEL,                            /* MM37: RRM Set New Channel */
    CMD_STATS_MESH_M_LAST
}STATS_MESH_M_CMD_E;

typedef enum
{
    MSG_CLIENT_ASSOCIATED = 0x1101,                     /* MM01: Client Associated */
    MSG_CLIENT_AUTHORIZED,                              /* MM02: Client Authorized */
    MSG_CLIENT_IP_ASSIGNED,                             /* MM03: Client IP Assigned */
    MSG_CLIENT_DISCONNECTED,                            /* MM04: Client DisConnected */
    MSG_CLIENT_IDLE_TIMEOUT,                            /* MM05: Client Idle Timeout */
    MSG_MESH_DL_ASSOCIATED,                             /* MM09: Mesh Downlink Associated */
    MSG_MESH_DL_AUTHORIZED,                             /* MM10: Mesh Downlink Authorized */
    MSG_MESH_DL_DISCONNECTED,                           /* MM11: Mesh Downlink DisConnected */
    MSG_MESH_DL_IDLE_TIMEOUT,                           /* MM12: Mesh Downlink Idle Timeout */
    MSG_REPLY_RADIO_STATS,                              /* MM46: Reply Radio Stats */
    MSG_REPLY_VAP_STATS,                                /* MM47: Reply VAP Stats */
    MSG_REPLY_STA_STATS,                                /* MM48: Reply Sta Stats */
    MSG_UPDATE_MESH_DOWNLINK_STATS,                     /* MM51: Update Mesh Downlink Stats */
    MSG_UPDATE_MESH_DOWNLINK_WLAN_STATS,                /* MM52: Update Mesh Downlink Wlan Stats */
    MSG_WLAN_CREATE,                                    /* MM55: Wlan Create */
    MSG_WLAN_REMOVE,                                    /* MM56: Wlan Remove */
    MSG_WLAN_UPDATE,                                    /* MM57: Wlan Update */
    MSG_WLAN_REPLY_LIST,                                /* MM60: Wlan Reply List */
    MSG_RRM_REPLY_SITE_SURVEY,                          /* MM36: RRM Reply Site Survey */
    MSG_MESH_M_STATS_LAST
}MESH_M_STATS_MSG_E;

#endif /* _MESH_M_STATS_EV_ */
