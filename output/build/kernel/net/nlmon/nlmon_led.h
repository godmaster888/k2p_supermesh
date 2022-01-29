#ifndef NLMON_LED_H
#define NLMON_LED_H

#include <linux/types.h>
#include <linux/list.h>
#include <linux/workqueue.h>	// works
#include <linux/spinlock.h>	// spinlock_t
#include <linux/mutex.h>	// struct mutex
#include <linux/atomic.h>	// atomic_t
#include <linux/netdevice.h>	// struct net_device
#include <linux/version.h>	// LINUX_VERSION_CODE, KERNEL_VERSION
#include <linux/kernel.h>	// U8_MAX

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
//added in 67bdbffd696f2
#define hlist_first_rcu(head)       (*((struct hlist_node __rcu **)(&(head)->first)))
#define hlist_next_rcu(node)        (*((struct hlist_node __rcu **)(&(node)->next)))
#endif

struct soft_net_led {
	const char *ledname;
	struct hlist_head netdev_hlist; //список net_device's
	struct mutex internals_lock; //для led_state и color_id, might_sleep()
	spinlock_t hlist_lock; //hlist_add_head_rcu()/hlist_del_rcu()
	int freq_idx; //частота мигания в таблице, -1 - undefined
	/* TODO: led_state меняется/читается только под internals_lock,
	 * так что смысл в atomic_t вообще отпадает. Разве что
	 * для proc-интерфейса, но это не самое важное.
	 * Убрать atomic_t и поставить просто int ?
	 */
	atomic_t led_state; //состояние леда от внешнего управления: off, on, blink
	unsigned char color_id; //каким цветом мигаем
};


extern const struct soft_net_led *get_next_soft_led(const struct soft_net_led *sled);
extern unsigned int soft_leds_num(void);

/*#define DRU_SOFT_LED_INIT(index, name) { */
/*.hlist_lock	= __SPIN_LOCK_UNLOCKED(dru_soft_net_leds[index].hlist_lock),*/

#define DRU_SOFT_LED_INIT(name) {					\
	.ledname	= name,						\
	.netdev_hlist	= HLIST_HEAD_INIT,				\
	/*.internals_lock in dru_soft_led_init() */			\
	/*.hlist_lock in dru_soft_led_init() */				\
	.freq_idx	= -1,	/* undefined */				\
	.led_state	= ATOMIC_INIT(SLED_OFF),			\
	.color_id	= U8_MAX,					\
	}

#endif

struct short_dev_stat {
	//начало struct rtnl_link_stats64
	__u64	rx_packets;		/* total packets received	*/
	__u64	tx_packets;		/* total packets transmitted	*/
};

struct soft_led_netdev {
	struct hlist_node hnode;
	struct net_device *dev;
	struct soft_net_led *sled; //back ptr
	int sw_port_num; //для WAN-порта; возможно вместо будет и port_mask
	struct short_dev_stat stat;
};

#ifdef CONFIG_PROC_FS
extern int  soft_led_proc_init(void);
extern void soft_led_proc_fini(void);
#endif
