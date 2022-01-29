/* netlinkmon.h */
#ifndef _NETLINKMON_H
#define _NETLINKMON_H

#include <net/nlmon/nlmon.h>

#ifndef CONFIG_NETLINK_MONITOR_NEW
#if defined(CONFIG_DRU_RT_3052_ESW)
#include <asm/atomic.h>
extern atomic_t nlmon_is_active;
#endif
#endif

#if defined(CONFIG_NETLINK_MONITOR_SOFT_LED)
#include <linux/netdevice.h>	/* struct net_device */
#endif

#ifndef CONFIG_NETLINK_MONITOR_NEW
#define NETLINK_MONITOR 27

/* Message types exchanged using NETLINK_MONITOR */
#ifndef MSG_NETLINK_WAKEUP_MONITOR_TASK
#define MSG_NETLINK_WAKEUP_MONITOR_TASK 0X1000
#endif

#ifndef MSG_NETLINK_LINK_STATUS_CHANGED
#define MSG_NETLINK_LINK_STATUS_CHANGED 0X2000
#endif

extern void kerSysSendtoNlMonTask(int msgType, char *msgData, int msgDataLen);
#endif

#if defined(CONFIG_NETLINK_MONITOR_LED_CONTROL)
extern int  dru_led_init(void);
extern void dru_led_fini(void);

#if defined(CONFIG_NETLINK_MONITOR_LAN_LED)
extern void dru_led_link_changed(unsigned port, unsigned link);
#endif

#if defined(CONFIG_NETLINK_MONITOR_SOFT_LED)

extern int  dru_soft_led_init(void);
extern void dru_soft_led_fini(void);

#define SLED_NO_SW_PORT		(-1)
extern void *dru_soft_led_register_netdev(const char *ledname,
					  struct net_device *dev, int sw_port_num);
extern int dru_soft_led_unregister_netdev(void *handle);

enum soft_led_state {
	SLED_OFF	= 0,
	SLED_ON		= 1,
	SLED_BLINK	= 2
};
extern int dru_soft_led_update_internals(void *handle,
					 enum soft_led_state state, unsigned char color_id);
extern int dru_soft_led_update_internals_by_name(const char *ledname,
						 enum soft_led_state state,
						 unsigned char color_id);


/*
 * act | dis
 *  X  | 0   - on
 *  0  | X   - off
 *  X  | Y   - blink
 *  0  | 0   - internal failure
 *
 * X !=0, Y != 0.
 */
typedef int (*dru_led_func_t)(const char *ledname, unsigned char color, unsigned long act, unsigned long dis);

//для gpiom
extern void dru_led_set_func(dru_led_func_t f);
extern void dru_led_unset_func(void);

#if defined(CONFIG_NETLINK_MONITOR_AP_LED)
//для wifi-драйверов
extern int dru_led_set(void *handle, unsigned long act, unsigned long dis);
/* wrappers (for MT7610E wifi driver) */
static inline int dru_led_on(void *handle)
{
	return dru_led_set(handle, 1UL, 0UL);
}
static inline int dru_led_off(void *handle)
{
	return dru_led_set(handle, 0UL, 1UL);
}
static inline int dru_led_blink(void *handle, unsigned long period)
{
	return dru_led_set(handle, period >> 1, period >> 1); // period / 2
}

#endif	/* CONFIG_NETLINK_MONITOR_AP_LED */

#endif	/* CONFIG_NETLINK_MONITOR_SOFT_LED */

#endif /* CONFIG_NETLINK_MONITOR_LED_CONTROL */

#endif /*_NETLINKMON_H */
