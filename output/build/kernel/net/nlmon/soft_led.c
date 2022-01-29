
#include <net/nlmon/netlinkmon.h>
#include "nlmon_led.h"
#include <linux/string.h>	// strcmp
#include <linux/slab.h>		// kmalloc
#include <linux/rcupdate.h>
#include <linux/kernel.h>	// ARRAY_SIZE

#include <asm/rt2880/rt_mmap.h>

#if defined (CONFIG_RALINK_MT7621)
#include "../drivers/net/raeth/raether.h"	/* mii_mgr_read() */
#endif

#ifndef RALINK_REG
#define RALINK_REG(x) (*((volatile u32 *)(x)))
#endif

static const struct soft_net_led dru_soft_net_leds[] = {
	DRU_SOFT_LED_INIT("wlan5"),
	DRU_SOFT_LED_INIT("wlan"),
	DRU_SOFT_LED_INIT("wan"),
	DRU_SOFT_LED_INIT(NULL)
};
#define for_each_soft_led(sled) for (sled = (struct soft_net_led *)dru_soft_net_leds; sled->ledname; sled++)

unsigned int soft_leds_num(void)
{
	return (unsigned int)ARRAY_SIZE(dru_soft_net_leds) - 1;
}

const struct soft_net_led *get_next_soft_led(const struct soft_net_led *sled)
{
	if (!sled) return &(dru_soft_net_leds[0]);

	++sled;

	if (!sled->ledname) sled = NULL;

	return sled;
}

static struct soft_net_led *get_led_by_name(const char *name)
{
	struct soft_net_led *sled;

	for_each_soft_led(sled) {
		if (!strcmp(sled->ledname, name))
			return sled;
	}
	return NULL;
}


/* раз в полсекунды проверять счётчики
 * одна задача на всех
 * UPDATE: секунды хватит
 */
#define SLED_TICK_PERIOD	(HZ)//(HZ/2)
static void led_check(struct work_struct *work);
static DECLARE_DELAYED_WORK(led_work, led_check);
/*
 * #17462 адаптивная скорость мигания
 *
 * >> Вот таблица:
 *
 * f,Hz	rx+tx, pps
 * 1	<=3200
 * 2	<=6400
 * 3	<=9400
 * 4	<=13600
 * 5	>=15000
 * <<<<<
 * Шабаловский жжОт. А (13600, 1500) ??
 * Пусть 5 будет соответствовать >13600
 */

struct led_speed {
	unsigned int pps; //rx+tx
	unsigned int invert_delay; //в мсек
};
#define SPEED_LIMIT(rx_plus_tx_pps, freq) {			\
	.pps = (rx_plus_tx_pps) * SLED_TICK_PERIOD / HZ,	\
	.invert_delay = 1000 / (2 * (freq))			\
}
static const struct led_speed speed_limits[] = {
	SPEED_LIMIT(3200, 1),
	SPEED_LIMIT(6400, 2),
	SPEED_LIMIT(9400, 3),
	SPEED_LIMIT(13600, 4),
	SPEED_LIMIT(15000, 5),
	SPEED_LIMIT(0, 6)
};

void *dru_soft_led_register_netdev(const char *ledname,
                                   struct net_device *dev, int sw_port_num)
{
	struct soft_net_led *sled = get_led_by_name(ledname);
	struct soft_led_netdev *sled_dev;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
//removed in b67bfe0d42cac
	struct hlist_node *tnode;
#endif

	if (unlikely(!sled))
		return ERR_PTR(-ENOENT);

	//поищем дубликат, не уверен, что NULL при выходе из each-цикла
	rcu_read_lock();
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
	hlist_for_each_entry_rcu(sled_dev, tnode, &sled->netdev_hlist, hnode) {
#else
	hlist_for_each_entry_rcu(sled_dev, &sled->netdev_hlist, hnode) {
#endif

		if (sled_dev->dev == dev) {
			rcu_read_unlock();
			return ERR_PTR(-EEXIST);
		}
	}
	rcu_read_unlock();

	sled_dev = kzalloc(sizeof(*sled_dev), GFP_KERNEL);

	if (unlikely(!sled_dev))
		return ERR_PTR(-ENOMEM);

	INIT_HLIST_NODE(&sled_dev->hnode);
	sled_dev->dev = dev;
	sled_dev->sled = sled;
	sled_dev->sw_port_num = sw_port_num;
	//sled_dev->stat is zeroed by kzalloc

	if (dev) dev_hold(dev);

	spin_lock(&sled->hlist_lock);
	hlist_add_head_rcu(&sled_dev->hnode, &sled->netdev_hlist);
	spin_unlock(&sled->hlist_lock);

	return sled_dev;
}
EXPORT_SYMBOL(dru_soft_led_register_netdev);

static int __dru_soft_led_unregister_netdev(void *handle)
{
	struct soft_led_netdev *sled_dev = (struct soft_led_netdev *)handle;
	struct soft_net_led *sled = sled_dev->sled;
	struct net_device *dev = sled_dev->dev;

	/*
	 * вот по идее эта лочка нужна только вокруг hlist_del_rcu,
	 * как я понял из Example 1: Maintaining Multiple Versions During Deletion
	 * по адресу https://lwn.net/Articles/262464/
	 * Но что-то мне не нравится. После отпускания rcu_read_unlock()
	 * кто-то может вклиниться, удалить и освободить найденный
	 * sled_dev, а вытесненный код потом попытается сделать
	 * удаление и освобождение того, чего уже нет.
	 * Что-то я не понимаю...
	 * UPDATE: изменил код, теперь всё по-другому немного
	 */
	spin_lock(&sled->hlist_lock);
	//удалим элемент
	hlist_del_rcu(&sled_dev->hnode);
	spin_unlock(&sled->hlist_lock);

	synchronize_rcu();

	if (dev) dev_put(dev);

	kfree(sled_dev);
	return 0;
}
int dru_soft_led_unregister_netdev(void *handle)
{
	if (IS_ERR_OR_NULL(handle))
		return -ENOENT;

	return __dru_soft_led_unregister_netdev(handle);
}
EXPORT_SYMBOL(dru_soft_led_unregister_netdev);

static void update_internals(struct soft_net_led *sled,
                             enum soft_led_state state, unsigned char color_id)
{
	mutex_lock(&sled->internals_lock);
	atomic_set(&sled->led_state, state);
	sled->color_id = color_id;
	mutex_unlock(&sled->internals_lock);
}

int dru_soft_led_update_internals(void *handle,
                                  enum soft_led_state state, unsigned char color_id)
{
	struct soft_led_netdev *sled_dev;
	struct soft_net_led *sled;

	if (IS_ERR_OR_NULL(handle))
		return -ENOENT;

	sled_dev = (struct soft_led_netdev *)handle;
	sled = sled_dev->sled;
	update_internals(sled, state, color_id);

	return 0;
}
EXPORT_SYMBOL(dru_soft_led_update_internals);

int dru_soft_led_update_internals_by_name(const char *ledname,
        enum soft_led_state state,
        unsigned char color_id)
{
	struct soft_net_led *sled = get_led_by_name(ledname);

	if (unlikely(!sled))
		return -ENOENT;

	update_internals(sled, state, color_id);
	return 0;
}
EXPORT_SYMBOL(dru_soft_led_update_internals_by_name);


static int fake_dru_led_set(const char *ledname, unsigned char color, unsigned long act, unsigned long dis)
{
	// gpiom is not loaded
	return 0;
}
static dru_led_func_t gpiom_led_handler = fake_dru_led_set;

void dru_led_set_func(dru_led_func_t f)
{
	rcu_assign_pointer(gpiom_led_handler, f);
}
EXPORT_SYMBOL(dru_led_set_func);

void dru_led_unset_func(void)
{
	dru_led_set_func(fake_dru_led_set);
	synchronize_rcu();// let gpiom is being unloaded
	//если один gpiom, то всё ok
	// если могут быть другие провайдеры,
	//то начнётся путаница с вытесением, и безопасней
	//поставить синхронизацию в set, а не только unset.
}
EXPORT_SYMBOL(dru_led_unset_func);

static int __dru_led_set(void *handle, unsigned long act, unsigned long dis)
{
	struct soft_led_netdev *sled_dev = (struct soft_led_netdev *)handle;
	struct soft_net_led *sled = sled_dev->sled;
	int ret;

	mutex_lock(&sled->internals_lock);

	if (!act && dis)
		atomic_set(&sled->led_state, SLED_OFF);
	else if (act && !dis)
		atomic_set(&sled->led_state, SLED_ON);
	else
		atomic_set(&sled->led_state, SLED_BLINK);

	//if (lan_led_is_locked)
	//	return 0;

	ret = rcu_dereference(gpiom_led_handler)(sled->ledname, sled->color_id, act, dis);
	mutex_unlock(&sled->internals_lock);
	return ret;
}
int dru_led_set(void *handle, unsigned long act, unsigned long dis)
{
	if (IS_ERR_OR_NULL(handle))
		return -ENOENT;

	return __dru_led_set(handle, act, dis);
}
EXPORT_SYMBOL(dru_led_set);


#if defined (CONFIG_RALINK_RT6855) || defined (CONFIG_RALINK_RT6855A) || \
    defined (CONFIG_RALINK_MT7620) || defined (CONFIG_RALINK_MT7628)

#define RA_ETH_SW_GET_REG(x) RALINK_REG(RALINK_ETH_SW_BASE + (x))

#elif defined (CONFIG_RALINK_MT7621)
static inline u32 RA_ETH_SW_GET_REG(u32 x)
{
	u32 reg_val;
	mii_mgr_read(31, x, &reg_val);
	return reg_val;
}
#else
#error your platform is not supported
#endif

static void check_one_sled(struct soft_net_led *sled)
{
	struct soft_led_netdev *sled_dev;
	struct short_dev_stat old, new;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
	struct hlist_node *tnode;
#endif
	int changed = 0;
	__u64 packets_per_period;
	const struct led_speed *ls;

	memset(&old, 0, sizeof(old));
	memset(&new, 0, sizeof(new));

	//соберём статистику по всем ифейсам
	rcu_read_lock();
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
	hlist_for_each_entry_rcu(sled_dev, tnode, &sled->netdev_hlist, hnode) {
#else
	hlist_for_each_entry_rcu(sled_dev, &sled->netdev_hlist, hnode) {
#endif
		struct rtnl_link_stats64 tmp;
		old.rx_packets += sled_dev->stat.rx_packets;
		old.tx_packets += sled_dev->stat.tx_packets;

		if (sled_dev->sw_port_num >= 0) {
			//порт свича
#if defined (CONFIG_RALINK_RT6855) || defined (CONFIG_RALINK_RT6855A) || \
    defined (CONFIG_RALINK_MT7620)
			//good frames count
			//TX, стр. 332
			tmp.rx_packets = RA_ETH_SW_GET_REG(0x4010 + 0x100 * sled_dev->sw_port_num) & 0xffff;
			//RX, стр. 333
			tmp.tx_packets = RA_ETH_SW_GET_REG(0x4020 + 0x100 * sled_dev->sw_port_num) & 0xffff;
#elif defined (CONFIG_RALINK_MT7621)
			//стр. 57, 61 из MT7621_ProgrammingGuide_GSW_v0_3.pdf
			__u64 rx_unicast_packets, rx_broadcast_packets, rx_multicast_packets;
			__u64 tx_unicast_packets, tx_broadcast_packets, tx_multicast_packets;

			rx_unicast_packets =    RA_ETH_SW_GET_REG(0x4068 + 0x100 * sled_dev->sw_port_num);
			rx_multicast_packets =  RA_ETH_SW_GET_REG(0x406C + 0x100 * sled_dev->sw_port_num);
			rx_broadcast_packets =  RA_ETH_SW_GET_REG(0x4070 + 0x100 * sled_dev->sw_port_num);

			tx_unicast_packets =    RA_ETH_SW_GET_REG(0x4008 + 0x100 * sled_dev->sw_port_num);
			tx_multicast_packets =  RA_ETH_SW_GET_REG(0x400C + 0x100 * sled_dev->sw_port_num);
			tx_broadcast_packets =  RA_ETH_SW_GET_REG(0x4010 + 0x100 * sled_dev->sw_port_num);

			tmp.rx_packets = rx_unicast_packets + rx_multicast_packets + rx_broadcast_packets;
			tmp.tx_packets = tx_unicast_packets + tx_multicast_packets + tx_broadcast_packets;
#elif defined (CONFIG_RALINK_MT7628)
			//MT7628_ProgrammingGuide_20140428(E2).pdf
			//RX, стр. 283
			tmp.rx_packets = RA_ETH_SW_GET_REG(0xE8 + 0x4 * sled_dev->sw_port_num) & 0xffff;
			//TX, стр. 298
			tmp.tx_packets = RA_ETH_SW_GET_REG(0x150 + 0x4 * sled_dev->sw_port_num) & 0xffff;
#else
#error fix your board
#endif
		} else
			dev_get_stats(sled_dev->dev, &tmp);

		sled_dev->stat.rx_packets = tmp.rx_packets;
		sled_dev->stat.tx_packets = tmp.tx_packets;
		new.rx_packets += tmp.rx_packets;
		new.tx_packets += tmp.tx_packets;
	}
	rcu_read_unlock();

	mutex_lock(&sled->internals_lock);

	if (atomic_read(&sled->led_state) != SLED_ON) {
		//не в "горящем состоянии", не можем мигать
		sled->freq_idx = -1;
		goto unlock;
	}

	if ( old.rx_packets != new.rx_packets )		changed = 1;
	else if (old.tx_packets != new.tx_packets)	changed = 1;

	if (!changed) {
		//не было трафа, просто зажжём
		rcu_dereference(gpiom_led_handler)(sled->ledname, sled->color_id, 1, 0);
		sled->freq_idx = -1;
		goto unlock;
	}

	packets_per_period = (new.rx_packets - old.rx_packets) + (new.tx_packets - old.tx_packets);

#if defined(CONFIG_NETLINK_MONITOR_WAN_LED_DIR_806A)
#warning remove me with correct array of blink freqs
	// консы заставили пойти на такое
	packets_per_period *= 2;
#endif

	for (changed = 0, ls = speed_limits;
	     changed < ARRAY_SIZE(speed_limits);
	     changed++, ls++) {
		if (!ls->pps)
			break;//достигли over max

		if (packets_per_period <= speed_limits[changed].pps)
			break;
	}

	if (changed == sled->freq_idx)
		goto unlock; //всё и так моргает как надо

	//if (net_ratelimit()) printk("++++ %s: %s freq: %d->%d\n", __func__, sled->ledname, sled->freq_idx, changed);
	sled->freq_idx = changed;
	rcu_dereference(gpiom_led_handler)(sled->ledname, sled->color_id, ls->invert_delay, ls->invert_delay);
unlock:
	mutex_unlock(&sled->internals_lock);
	//if (net_ratelimit()) printk("d(rx+tx) = %u, ind = %u, delay = %u\n", (unsigned)packets_per_period, led5_freq_index, led5_delays[led5_freq_index]);
}


static void led_check(struct work_struct *work)
{
	struct soft_net_led *sled;

	for_each_soft_led(sled) {
		check_one_sled(sled);
	}
	schedule_delayed_work(&led_work, SLED_TICK_PERIOD);
}


__init int  dru_soft_led_init(void)
{
	struct soft_net_led *sled;

	for_each_soft_led(sled) {
		mutex_init(&sled->internals_lock);
		spin_lock_init(&sled->hlist_lock);
		/*
		 * выключен, пусть его включают
		 * wifi драйвер в случае AP
		 * gpiom в случае WAN (ls дёрнет через dms_led)
		 */
		//atomic_set(&sled->led_state, SLED_OFF);
		//^ сделано в статическом инициализаторе
	}
#ifdef CONFIG_PROC_FS
	soft_led_proc_init();
#endif
	schedule_delayed_work(&led_work, SLED_TICK_PERIOD);
	return 0;
}

__exit void dru_soft_led_fini(void)
{
	struct soft_net_led *sled;

	if (delayed_work_pending(&led_work))
		cancel_delayed_work(&led_work);

	flush_delayed_work(&led_work);

#ifdef CONFIG_PROC_FS
	soft_led_proc_fini();
#endif
	for_each_soft_led(sled) {
		//я хуй его знает как делать правильно
		//надеюсь в fini никогда не зайдёт
		spin_lock(&sled->hlist_lock);

		while (!hlist_empty(&sled->netdev_hlist)) {
			struct hlist_node *n =  sled->netdev_hlist.first;
			struct soft_led_netdev *sled_dev = container_of(n, struct soft_led_netdev, hnode);
			hlist_del_rcu(n);
			synchronize_rcu();

			if (sled_dev->dev) dev_put(sled_dev->dev);

			kfree(sled_dev);
		}

		spin_unlock(&sled->hlist_lock);
	}
}
