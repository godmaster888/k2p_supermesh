#ifndef _MQTT_API_H
#define _MQTT_API_H

#include <stdio.h>
#include <stdbool.h>

#include "apson_typedef.h"

typedef enum
{
    THREAD_STATE_MACHINE = 0,
    THREAD_STATISTIC ,
    THREAD_STATION ,
    THREAD_NORTH_BOUND ,
    THREAD_SOUTH_BOUND_DAEMON ,
    THREAD_MESH_DAEMON ,
    THREAD_MQTT ,
    THREAD_APSON_MAIN ,
    THREAD_LAST
} THREAD_E;

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

#define INTERNAL_PORT            1883
#define OUTSIDE_PORT             8883

#define INTERNAL_MAC             NULL

#define INTERNAL_BROKER          "127.0.0.1"
#define INTERNAL_BROKERV6        "::1"

#define MAX_INFLIGHT             20

#define MQTT_PUB_PRE_ID          "MQTT_P"
#define MQTT_SUB_PRE_ID          "MQTT_S"

#ifndef MQTT_PROTOCOL_V31
#define MQTT_PROTOCOL_V31        3
#endif

#define OFFSETOF(type, member)  ((size_t)&((type *)0)->member)

#define KEEPALIVE                60
#define LOCAL_KEEPALIVE          86400
#define QOS_0                    0

#define CAFILE                   "/etc/mosquitto/ssl/cacrt.pem"
#define CERTFILE                 "/etc/mosquitto/ssl/servercrt.pem"
#define KEYFILE                  "/etc/mosquitto/ssl/serverkey.pem"

#define USERNAME                 "apson"
#define PASSWORD                 "wrsdd"

#define MQTT_HDR_EVENT_LEN              4
#define MQTT_HDR_VERSION_LEN            2
#define MQTT_HDR_MAC_LEN                6
#define MQTT_HDR_DST_THREAD_LEN         1
#define MQTT_HDR_SRC_THREAD_LEN         1
#define MQTT_HDR_GPBLEN_LEN             4
#define MQTT_HDR_RECV_LEN               2
#define APSON_MQTT_HEADER_LENGTH        OFFSETOF( MOSQ_MESSAGE_S, gpb )

//#define APSON_MSQ_BUF_SIZE       1024
#define MOSQ_GPB_BUF_SIZE        (1024 * 256)

#define APSON_VERSION            1

#define TOPIC_MASTER             "master"
#define TOPIC_AP_ALL             "ap/all"
#define TOPIC_APSON              "apson"
#define TOPIC_NORTHBOUND         "nb"
#define TOPIC_SOUTHBOUND         "sbd"
#define TOPIC_MESHD              "meshd"

typedef struct mosq_config_t {
    PCHAR id;
    PCHAR id_prefix;
    INT protocol_version;
    INT keepalive;
    PCHAR host;
    INT port;
    INT qos;
    BOOL retain;
    INT pub_mode; /* pub */
    PCHAR file_input; /* pub */
    PCHAR message; /* pub */
    LONG msglen; /* pub */
    PCHAR topic; /* pub */
    PCHAR bind_address;
    BOOL debug;
    BOOL quiet;
    UINT max_inflight;
    PCHAR username;
    PCHAR password;
    PCHAR will_topic;
    PCHAR will_payload;
    LONG will_payloadlen;
    INT will_qos;
    BOOL will_retain;
    PCHAR cafile;
    PCHAR capath;
    PCHAR certfile;
    PCHAR keyfile;
    PCHAR ciphers;
    BOOL insecure;
    PCHAR tls_version;
    BOOL clean_session; /* sub */
    PCHAR *topics; /* sub */
    INT topic_count; /* sub */
    BOOL no_retain; /* sub */
    PCHAR *filter_outs; /* sub */
    INT filter_out_count; /* sub */
    BOOL verbose; /* sub */
    BOOL eol; /* sub */
    INT msg_count; /* sub */
    BOOL disconnect;
} MOSQ_CONFIG_T, *PMOSQ_CONFIG_T;
/*
enum mosq_opt_t {
    MOSQ_OPT_PROTOCOL_VERSION = 1,
};*/

typedef struct mosq_message_t {
    ULONG event_id;
    short apson_version;
    UCHAR source_mac[6];
    CHAR dst_thread;
    CHAR source_thread;
    CHAR reserve[2];
    UINT gpb_length;
    UCHAR gpb[MOSQ_GPB_BUF_SIZE];

} MOSQ_MESSAGE_S, *PMOSQ_MESSAGE_S;

typedef struct mosquitto MOSQ_S, *PMOSQ_S;

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_mqtt_lib_init()
 *
 * PURPOSE: To init library for mqtt.
 *
 * INPUT:   none
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          none
 * NOTE:
 *-----------------------------------------------------------
 */
void apson_mqtt_lib_init();

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_mqtt_init()
 *
 * PURPOSE: The init function for mqtt connect config 
 *
 * INPUT:   pCfg            -- mqtt connect config
 *          pGlobalcontext  -- apson global context
 *          pBuf            -- buffer for get message
 *          encrypt         -- config if this mqtt connect encrypt
 * OUTPUT:
 *          none
 * RETURN:
 *          PMOSQ_S
 * NOTE:
 *-----------------------------------------------------------
 */
PMOSQ_S apson_mqtt_init( PCHAR pBuf, BOOL encrypt );


PMOSQ_S apson_mqtt_init_withid( PCHAR pBuf, BOOL encrypt, PUCHAR mac);
/*-----------------------------------------------------------
 * FUNCTION NAME - apson_mqtt_connect()
 *
 * PURPOSE: The init function for mqtt connect config
 *
 * INPUT:   pMosq           -- mqtt connect config
 *          pBroker_ip      -- the ip address or host name of mqtt broker
 *          port            -- 1883 for no encrypt, 8883 for encrypt
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          APSON_STATUS
 * NOTE:
 *-----------------------------------------------------------
 */
APSON_STATUS apson_mqtt_connect( PMOSQ_S pMosq, PCHAR pBroker_ip, INT port, LONG keepalive);

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_mqtt_get_socket()
 *
 * PURPOSE: The function for get socket in mqtt connect
 *
 * INPUT:   pMosq     -- mqtt connect config
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          INT
 * NOTE:
 *-----------------------------------------------------------
 */
INT apson_mqtt_get_socket( PMOSQ_S pMosq);

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_mqtt_subscribe()
 *
 * PURPOSE: The function for subscribe topic at mqtt broker.
 *
 * INPUT:   pMosq     -- mqtt connect config
 *          topics    -- the topic you want to subscribe
 * OUTPUT:
 *          none
 * RETURN:
 *          APSON_STATUS
 * NOTE:
 *-----------------------------------------------------------
 */
INT apson_mqtt_subscribe(PMOSQ_S pMosq, CHAR topics[]);

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_mqtt_unsubscribe()
 *
 * PURPOSE: The function for subscribe topic at mqtt broker.
 *
 * INPUT:   pMosq     -- mqtt connect config
 *          topics    -- the topic you want to unsubscribe
 * OUTPUT:
 *          none
 * RETURN:
 *          APSON_STATUS
 * NOTE:
 *-----------------------------------------------------------
 */
APSON_STATUS apson_mqtt_unsubscribe(PMOSQ_S pMosq, CHAR topics[]);

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_mqtt_loop_read()
 *
 * PURPOSE: The function for setting listen mqtt socket.
 *
 * INPUT:   pMosq      -- mqtt connect config
 *          read_fds   -- the sokcet you want to read
 *          write_fds  -- the sokcet you want to write
 * OUTPUT:
 *          none
 * RETURN:
 *          INT
 * NOTE:
 *-----------------------------------------------------------
 */
INT apson_mqtt_loop_read( PMOSQ_S pMosq, fd_set *read_fds, fd_set *write_fds);

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_mqtt_loop_misc()
 *
 * PURPOSE: The function for deal with healthy ping for mqtt
 *
 * INPUT:   pMosq      -- mqtt connect config
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          INT
 * NOTE:
 *-----------------------------------------------------------
 */
INT apson_mqtt_loop_misc(PMOSQ_S pMosq);

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_mqtt_disconnect()
 *
 * PURPOSE: The function for disconnectt from mqtt broker.
 *
 * INPUT:   pMosq     -- mqtt connect config
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          INT
 * NOTE:
 *-----------------------------------------------------------
 */
INT apson_mqtt_disconnect( PMOSQ_S pMosq );

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_mqtt_destroy()
 *
 * PURPOSE: The function for destroy mqtt connect.
 *
 * INPUT:   pMosq     -- mqtt connect config
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          INT
 * NOTE:
 *-----------------------------------------------------------
 */
INT apson_mqtt_destroy( PMOSQ_S pMosq );

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_mqtt_lib_cleanup()
 *
 * PURPOSE: The function for cleanup mqtt library.
 *
 * INPUT:   none
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          INT
 * NOTE:
 *-----------------------------------------------------------
 */
INT apson_mqtt_lib_cleanup();

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_mqtt_publish()
 *
 * PURPOSE: publish message to broker.
 *
 * INPUT:   pMessage     -- the message for publish to broker
 *          pBroker_ip   -- the ip address or host name of mqtt broker
 *          msglen       -- the length of message
 *          pTopic       -- the topic for you want to publish
 * OUTPUT:
 *          publish a mqtt package to broker.
 * RETURN:
 *          APSON_STATUS
 * NOTE:
 *-----------------------------------------------------------
 */
APSON_STATUS apson_mqtt_publish(PMOSQ_MESSAGE_S pMessage, ULONG msglen, PCHAR pBroker_ip, PCHAR pTopic);

/*-----------------------------------------------------------
 * FUNCTION NAME - apson_mqtt_make_payload()
 *
 * PURPOSE: make a mqtt package payload.
 *
 * INPUT:   version         -- apson version.
 *          event_id        -- apson event id.
 *          src_mac         -- mac of mine.
 *          src_thread      -- send the packet from which thread.
 *          dst_thread      -- send the packet to which thread.
 *          gpb_payload     -- the gpb data.
 *          gpb_length      -- the gpb data length.
 * OUTPUT:
 *          mqtt_payload    -- mqtt payload.
 * RETURN:
 *          INT
 * NOTE:
 *-----------------------------------------------------------
 */

INT apson_mqtt_make_payload(PMOSQ_MESSAGE_S mqtt_payload, SHORT version, ULONG event_id, PUCHAR src_mac, CHAR src_thread, CHAR dst_thread, PUCHAR gpb_payload, UINT gpb_length);


/*-----------------------------------------------------------
 * FUNCTION NAME - meshd_make_mqtt_payload()
 *
 * PURPOSE: make a meshd mqtt package payload.
 *
 * INPUT:   event_id        -- apson event id.
 *          gpb_payload     -- the gpb data.
 *          gpb_length      -- the gpb data length.
 * OUTPUT:
 *          mqtt_payload    -- mqtt payload.
 * RETURN:
 *          APSON_STATUS
 * NOTE:
 *-----------------------------------------------------------
 */
APSON_STATUS meshd_make_mqtt_payload(PMOSQ_MESSAGE_S mqtt_payload, ULONG event_id, UINT gpb_length, PUCHAR gpb_payload);

/*-----------------------------------------------------------
 * FUNCTION NAME - meshd_mqtt_publish()
 *
 * PURPOSE: publish message to broker for meshd.
 *
 * INPUT:   mqtt_payload -- the message for publish to broker
 *          msglen       -- the length of message
 *          ipv6         -- the flag if enable ipv6
 * OUTPUT:
 *          publish a mqtt package to broker.
 * RETURN:
 *          APSON_STATUS
 * NOTE:
 *-----------------------------------------------------------
 */
APSON_STATUS meshd_mqtt_publish(PMOSQ_MESSAGE_S mqtt_payload, ULONG msglen, BOOL ipv6);

/*-----------------------------------------------------------
 * FUNCTION NAME - meshd_mqtt_loop_read()
 *
 * PURPOSE: The function for setting listen mqtt socket.
 *
 * INPUT:   pMosq      -- mqtt connect config
 *
 * OUTPUT:
 *          none
 * RETURN:
 *          none
 * NOTE:
 *-----------------------------------------------------------
 */
INT meshd_mqtt_loop_read(PMOSQ_S pMosq);

INT meshd_mqtt_reconnect(PMOSQ_S pMosq);

APSON_STATUS apson_mosquitto_user_data_set(PMOSQ_S pMosq, PVOID obj);

PMOSQ_S apson_nb_mqtt_init( PCHAR pBuf, BOOL encrypt );

PMOSQ_S apson_test_mqtt_init( PCHAR pBuf, BOOL encrypt );

APSON_STATUS apson_mosquitto_socket_connect_tls(PMOSQ_S pMosq);

#endif /* _MQTT_API_H */
