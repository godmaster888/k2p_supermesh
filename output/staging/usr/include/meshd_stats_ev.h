#ifndef _MESHD_STATS_EV_
#define _MESHD_STATS_EV_

typedef enum
{
    CMD_GET_MESH_NEIGHBOR_STATS = 0x1201,               /* MM43: Get Mesh Neighbor Stats */
    CMD_GET_MESH_PROFILE_REQ,                           /* Profile request */
    CMD_GET_MESH_UPLINK_STATS,                          /* MM64: Get Mesh Uplink Stats */
    CMD_STATS_MESHD_LAST
}STATS_MESHD_CMD_E;

typedef enum
{
    MSG_UPDATE_MESH_NEIGHBOR_STATS = 0x1301,            /* MM50: Update Mesh Neighbor Stats */
    MSG_UPDATE_MESH_ACTIVITY,                           /* TBD: Update Mesh Activity */
    MSG_UPDATE_MESH_PROFILE,                            /* TBD: Update Mesh Profile */
    MSG_WIRELESS_UPLINK,                                /* MM62: Wireless Uplink */
    MSG_WIRED_UPLINK,                                   /* MM63: Wired Uplink */
    MSG_REPLY_MESH_UPLINK_STATS,                        /* MM65: Reply Mesh Uplink Stats */
    MSG_MESHD_STATS_LAST
}MESHD_STATS_MSG_E;

#endif /* _MESHD_STATS_EV_ */
