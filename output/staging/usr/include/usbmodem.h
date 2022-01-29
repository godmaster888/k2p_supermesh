#ifndef _LIBUSBMODEM_H_
#define _LIBUSBMODEM_H_


#include <jansson.h>


// ======================================   State
typedef enum {
	USBMODEM_STATE_UNKNOWN = 0,
	USBMODEM_STATE_INSERTED,	// we have usb device info only
	USBMODEM_STATE_INITIALIZED,	// we have modem info only
	USBMODEM_STATE_WAITING_PIN,	// waiting for PIN code input
	USBMODEM_STATE_READY,		// we have all the information about the modem and his SIM cards
	USBMODEM_STATE_CONN_SEARCHING,	// we are searching for an appropriate connection
	USBMODEM_STATE_CONN_FOUND,	// we have found an appropriate connection
	USBMODEM_STATE_CONNECT,		// we are trying to connect
	USBMODEM_STATE_CONNECTED,	// we are connected
	USBMODEM_STATE_DISCONNECT,	// we are trying to disconnect
	USBMODEM_STATE_REMOVED,		// we are removed
	USBMODEM_STATE_MAX		// 11
} usbmodem_state_t;

// ======================================   SMS state
typedef enum {
  USBMODEM_SMS_STATE_UNREAD = 0,
  USBMODEM_SMS_STATE_READ,
  USBMODEM_SMS_STATE_UNSENT,
  USBMODEM_SMS_STATE_SENT,
  USBMODEM_SMS_STATE_ALL
} usbmodem_sms_state_t;

// ======================================   Type
#define USBMODEM_TYPE_MASK		0xf000

#define USBMODEM_TYPE_UNKNOWN	0x0000
#define USBMODEM_TYPE_PPP		0x1000
#define USBMODEM_TYPE_RNDIS		0x2000
#define USBMODEM_TYPE_QMI		0x3000
#define USBMODEM_TYPE_NCM		0x4000
#define USBMODEM_TYPE_DWP812	0x5000

#define USBMODEM_TYPE(descriptor)				(descriptor & USBMODEM_TYPE_MASK)
#define USBMODEM_SET_TYPE(descriptor, type)		do { \
													descriptor &= ~USBMODEM_TYPE_MASK; \
													descriptor |= type & USBMODEM_TYPE_MASK; \
												} while (0)

// ======================================   Subtype
#define USBMODEM_SUBTYPE_MASK		0x0f00

#define USBMODEM_SUBTYPE_UNKNOWN	0x0000
#define USBMODEM_SUBTYPE_HUAWEI		0x0100
#define USBMODEM_SUBTYPE_MOTOROLA	0x0200
#define USBMODEM_SUBTYPE_ANYDATA	0x0300 // CDMA
#define USBMODEM_SUBTYPE_CMOTECH	0x0400
#define USBMODEM_SUBTYPE_ZTE_M		0x0500
#define USBMODEM_SUBTYPE_ZTE_A		0x0600 // CDMA
#define USBMODEM_SUBTYPE_COMMON		0x0800

#define USBMODEM_SUBTYPE(descriptor)			(descriptor & USBMODEM_SUBTYPE_MASK)
#define USBMODEM_SET_SUBTYPE(descriptor, type)	do { \
													descriptor &= ~USBMODEM_SUBTYPE_MASK; \
													descriptor |= type & USBMODEM_SUBTYPE_MASK; \
												} while (0)

// ======================================   Flags
#define USBMODEM_FLAGS_MASK			0x00ff

#define USBMODEM_FLAG_HAS_AT		1 << 0
#define USBMODEM_AT_PPP_SAME_NODE	1 << 1
#define USBMODEM_FLAG_NCM_STATIC	1 << 2
#define USBMODEM_FLAG_BRIDGE		1 << 3
#define USBMODEM_FLAG_REMOTE		1 << 4


// ======================================

// ======================================   Transparent Bridge Flag
#define USBMODEM_CAN_BE_BRIDGED(descriptor)		(descriptor & USBMODEM_FLAG_BRIDGE)


#define USBMODEM_ERR_NOT_INITIALIZED	-1

#define USBMODEM_DEFAULT_TIMEOUT	5*1000


#define USBMODEM_ONESHOT(modem, func, ...)	do { \
													if (usbmodem_launch(modem, USBMODEM_DEFAULT_TIMEOUT) == 0) { \
														usbmodem_ ## func(__VA_ARGS__); \
														usbmodem_land(); \
													} \
											} while(0)

#define USBMODEM_ONESHOT_INT(modem, func, ...)	({ \
													int oneshot_res; \
													oneshot_res = usbmodem_launch(modem, USBMODEM_DEFAULT_TIMEOUT); \
													if (oneshot_res == 0) { \
														oneshot_res = usbmodem_ ## func(__VA_ARGS__); \
														usbmodem_land(); \
													} \
													oneshot_res; \
												})
#define USBMODEM_ONESHOT_JSON(modem, func, ...)	({ \
													json_t *oneshot_res = NULL; \
													if (usbmodem_launch(modem, USBMODEM_DEFAULT_TIMEOUT) == 0) { \
														oneshot_res = usbmodem_ ## func(__VA_ARGS__); \
														usbmodem_land(); \
													} \
													oneshot_res; \
												})


typedef enum { // the same as g3_pinstatus
	USBMODEM_PINSTATUS_UNKNOWN = -1,
	USBMODEM_PINSTATUS_READY = 0,
	USBMODEM_PINSTATUS_PIN,
	USBMODEM_PINSTATUS_PUK,
	USBMODEM_PINSTATUS_PIN2,
	USBMODEM_PINSTATUS_PUK2,
	USBMODEM_PINSTATUS_RUIM_ABSENT,
	USBMODEM_PINSTATUS_RUIM_BLOCKED,
	USBMODEM_PINSTATUS_RUIM_TYPE_WRONG,
	USBMODEM_PINSTATUS_MAX,
} usbmodem_pinstatus_t;

typedef enum {
	USBMODEM_LOCK_UNKNOWN = -1,
	USBMODEM_IS_NOT_LOCKED = 0,
	USBMODEM_IS_LOCKED,
} usbmodem_pin_lock_t;


static inline int descriptor_to_lib3g_type(int descriptor)
{
	return USBMODEM_SUBTYPE(descriptor) >> 8;
}

// usbmodem.c
extern int usbmodem_launch_dbg(const char *func, json_t *info, int timeout);
extern int usbmodem_land_dbg(const char *func);
#  define usbmodem_launch(info, timeout)	usbmodem_launch_dbg(__func__, info, timeout)
#  define usbmodem_launch_endless(info)		usbmodem_launch_dbg(__func__, info, -1)
#  define usbmodem_land()					usbmodem_land_dbg(__func__)

extern int usbmodem_at_test(const char *node, const char *cmd, const char *test_str);
extern int usbmodem_at_cmd(const char *cmd, const char *node);

extern int usbmodem_operability_test();

// state.c
extern usbmodem_state_t usbmodem_state(const char *strstate);
extern const char *usbmodem_strstate(usbmodem_state_t state);

// qmi.c
extern json_t *usbmodem_qmi_cmd(int service, int cmd, int tlvc, char *tlvs[]);

// initdongle.c
extern int usbmodem_initdongle(json_t *modem);

// modem_info.c
extern int usbmodem_fill_modem_info(json_t *info);

// sim.c
extern void usbmodem_fill_sim_info(json_t *modem, json_t *sim);
extern const char *usbmodem_get_imsi();
extern int usbmodem_fill_imsi(json_t *sim);
extern int usbmodem_attach_sim(int attach);

// connection.c
extern unsigned int usbmodem_connect(const char *apn, const char *auth, const char *username, const char *passwd, int ipv6, int ipv6_only);
extern int usbmodem_disconnect(unsigned int handle);

// preferences.c
extern int usbmodem_set_preferences(const char *mode, unsigned int band, unsigned int lte_band, unsigned int domain);

// pin.c
extern json_t *usbmodem_pin_info();
extern usbmodem_pin_lock_t usbmodem_pin_is_locked();
extern usbmodem_pinstatus_t usbmodem_pin_status(int *pin_att_left, int *puk_att_left);
extern int usbmodem_verify_pin(const char *pin, const char *puk);
extern int usbmodem_pin_lock(int enable, const char *pin);
extern int usbmodem_change_pin(const char *old, const char *new);
extern const char *usbmodem_pinstatus_str(usbmodem_pinstatus_t status);
extern usbmodem_pinstatus_t usbmodem_pinstatus(const char *strstatus);
extern const char *usbmodem_strpinstatus(usbmodem_pinstatus_t status);

// cell.c
extern void usbmodem_fill_cell_info(json_t *cell);

// gpio.c
typedef int (toggle_func_t)(const char *name, int state);
typedef int (get_func_t)(const char *name);

extern void usbmodem_init_gpio(toggle_func_t *toggle, get_func_t *get);
extern int usbmodem_switch_sim(json_t *modem, const char *sim_name);
extern int usbmodem_sim_is_inserted_hw(json_t *modem, const char *sim_name);
extern int usbmodem_attach_imsi(int attach);

// ussd.c
extern int usbmodem_ussd(const char *code, char *response, int response_size);

// sms.c
extern int usbmodem_send_sms(const char *number, const char *text);
extern json_t *usbmodem_sms_list(usbmodem_sms_state_t state);
extern int usbmodem_remove_sms_indexes(json_t *indexes);
extern int usbmodem_remove_sms_list(json_t *sms_list);
extern json_t *usbmodem_get_sms_statistics();

// tree.c
extern json_t *usbmodem_tree(const char *path);
extern json_t *usbmodem_full_tree();
extern json_t *usbmodem_full_tree_union();

// pdp_context.c
extern json_t *usbmodem_get_pdp_context();
extern int usbmodem_set_pdp_context(const char *af, const char *apn);
extern int usbmodem_activate_pdp_context(int activate, int cid);
extern int usbmodem_attach_pdp_context(int attach);

// xncm.c
extern int usbmodem_xncm_connect(const char *af, const char *apn);
extern int usbmodem_xncm_disconnect();
extern int usbmodem_enable_xdns(int enable);
extern json_t *usbmodem_get_xdns();

// gps.c
extern int usbmodem_gps_on();
extern int usbmodem_gps_off();
extern int usbmodem_gps_state();
extern int usbmodem_gps_switch_and_test(int on);
extern json_t *usbmodem_gps_location();

// dwp812.c
extern int usbmodem_dwp812_connect(const char *apn, const char *af);
extern int usbmodem_dwp812_disconnect();
extern json_t *usbmodem_dwp812_get_info();
extern int usbmodem_dwp812_sim_inserted();
extern void usbmodem_dwp812_fill_cell_info(json_t *cell);
extern int usbmodem_dwp812_is_always();
extern int usbmodem_dwp812_set_always(int enable);
extern int usbmodem_dwp812_reboot();
extern int usbmodem_dwp812_reset();
extern int usbmodem_dwp812_set_pdp_context(const char *apn, const char *af);
extern usbmodem_pinstatus_t usbmodem_dwp812_pinstatus(int *pin_left, int *puk_left);

// ip.c
extern const char *usbmodem_get_ip_address(int ipv6);

// imei.c
extern const char *usbmodem_imei_get(void);
extern int usbmodem_imei_set(const char *imei);

// post_connect.c
extern int usbmodem_post_connect(json_t *modem, const char *ip_addr);


#endif
