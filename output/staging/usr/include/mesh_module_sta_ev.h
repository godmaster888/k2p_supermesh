#ifndef _MESH_M_STA_EV_
#define _MESH_M_STA_EV_

typedef enum
{
    CMD_FORCE_ROAMING_SET_WEAK_CLIENT_THRESHOLD=0x2001, /* MM26: Force Roaming Set Weak Client Threshold */
    CMD_FORCE_ROAMING_GET_MONITOR_LIST,                 /* MM27: Force Roaming Get Monitor List */
    CMD_FORCE_ROAMING_SET_MONITOR_CLIENT,               /* MM28: Force Roaming Set Moniror Client */
    CMD_FORCE_ROAMING_SET_ACCEPT_CRITERIA,              /* MM29: Force Roaming Set Accept Criteria */
    CMD_FORCE_ROAMING_KICK_OUT_CLIENT,                  /* MM30: Force Roaming Kick Out Client */
    CMD_CLIENT_BEACON_REQUEST,                          /* Others: Client beacon report request */
    CMD_BTM_CLIENT_STEERING_REQ,                        /* Others: Client Steering BTM request */
    CMD_STA_MESH_M_LAST
}STA_MESH_M_CMD_E;

typedef enum
{
    MSG_FORCE_ROAMING_WEAK_SIGNAL_CLIENT_INFORM=0x2101, /* MM31: Force Roaming Weak Signal Client Inform */
    MSG_FORCE_ROAMING_MONITOR_CLIENT_INFORM,            /* MM32: Force Roaming Monitor Client Inform */
    MSG_CLIENT_BEACON_REPORT,                           /* Others: Client beacon report reply */
    MSG_MESH_M_STA_LAST
}MESH_M_STA_MSG_E;

#endif /* _MESH_M_STA_EV_ */
