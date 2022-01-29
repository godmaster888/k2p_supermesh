#include "nlmon_led.h"
#include <net/nlmon/netlinkmon.h>	// SLED_(ON|OFF|BLINK)
#include <linux/proc_fs.h>
#include <linux/seq_file.h>


struct sled_iter_private {
	const struct soft_net_led *sled;//led we iterate throw
	int rcu_is_read_locked;
	int is_first_dev_in_led;
};

static struct proc_dir_entry *soft_led_proc = NULL;

static void *soft_led_seq_start(struct seq_file *s, loff_t *pos)
//__acquires(rcu) __releases(rcu) __must_hold(rcu)
{
	loff_t i = 1;
	struct sled_iter_private *priv = s->private;

	priv->sled = get_next_soft_led(NULL);//первый, может и NULL
	priv->rcu_is_read_locked = 0;
	priv->is_first_dev_in_led = 0;

	if (*pos == 0)
		return SEQ_START_TOKEN;

	//будем искать
	for (; priv->sled; priv->sled = get_next_soft_led(priv->sled)) {
		struct soft_led_netdev *sled_dev;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
		struct hlist_node *tnode;
#endif
		priv->rcu_is_read_locked = 1;
		priv->is_first_dev_in_led = 1;
		rcu_read_lock();
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
		hlist_for_each_entry_rcu(sled_dev, tnode, &priv->sled->netdev_hlist, hnode) {
#else
		hlist_for_each_entry_rcu(sled_dev, &priv->sled->netdev_hlist, hnode) {
#endif

			if (i == *pos)
				//rcu_dereference унутре for_each
				return sled_dev; //RCU под read лочкой

			priv->is_first_dev_in_led = 0;
			i++;
		}
		rcu_read_unlock();
		priv->rcu_is_read_locked = 0;
	}

	return NULL;//в stop попадает разлоченным
}

#define next_to_sled_dev(node) \
	container_of(rcu_dereference(node), struct soft_led_netdev, hnode);

static void *soft_led_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct sled_iter_private *priv = s->private;
	struct soft_led_netdev *sled_dev = v;

	(*pos)++;

	if (v == SEQ_START_TOKEN) {
		if (!priv->sled) return NULL;//нету ледов вообще

		//стоим на первом
		sled_dev = NULL;

		priv->is_first_dev_in_led = 1;

		do {
			//led in rcu-unlocked here
			rcu_read_lock();
			sled_dev = next_to_sled_dev(hlist_first_rcu(&priv->sled->netdev_hlist));

			if (sled_dev) {
				priv->rcu_is_read_locked = 1;
				break;
			}

			//пустой лед
			rcu_read_unlock();
			priv->sled = get_next_soft_led(priv->sled);

			if (!priv->sled) return NULL;//леды кончились
		} while (1);//(!sled_dev);//по сути while (1)

		return sled_dev;
	}

	/* из старта или некста
	 * из старта под rcu-лочкой
	 * из некста (ниже) тоже передавать залоченным
	 * sled и sled_dev тут ненулевые
	 * is_first_dev_in_led любой, соотв. прошлому состоянию
	 */
	sled_dev = next_to_sled_dev(hlist_next_rcu(&sled_dev->hnode));

	if (sled_dev) {
		//не первый в этом леде
		priv->is_first_dev_in_led = 0;
		return sled_dev;
	}

	//следующий лед
	priv->is_first_dev_in_led = 1;

	do {
		rcu_read_unlock();
		priv->sled = get_next_soft_led(priv->sled);

		if (!priv->sled) {
			priv->rcu_is_read_locked = 0;
			return NULL;
		}

		rcu_read_lock();
		sled_dev = next_to_sled_dev(hlist_first_rcu(&priv->sled->netdev_hlist));

		if (sled_dev)
			break;
	} while (1);

	return  sled_dev;
}
static void soft_led_seq_stop(struct seq_file *s, void *v)
{
	struct sled_iter_private *priv = s->private;

	if (priv->rcu_is_read_locked)
		rcu_read_unlock();
}
static int soft_led_seq_show(struct seq_file *s, void *v)
{
	struct sled_iter_private *priv = s->private;
	struct soft_led_netdev *sled_dev = v;

	if (v == SEQ_START_TOKEN) {
		const struct soft_net_led *sled = get_next_soft_led(NULL);
		int ind = 0;
		seq_printf(s, "# of soft leds: %u\n", soft_leds_num());

		for (; sled; sled = get_next_soft_led(sled), ind++) {
			seq_printf(s, "# [%d]: %s\n", ind, sled->ledname);
		}

		return 0;
	}

	if (priv->is_first_dev_in_led) {
		//м.б. NULL при SEQ_START_TOKEN, тут точно не NULL
		const struct soft_net_led *const sled = priv->sled;
		unsigned long led_state = atomic_read(&sled->led_state);
		seq_printf(s, "%s: freq_idx=%d ext_state=%s color=%d\n",
		           sled->ledname, sled->freq_idx,
		           (led_state == SLED_OFF) ? "off" :
		           (led_state == SLED_ON) ? "on"  :
		           "blinking",
		           (int)sled->color_id);
	}

	seq_printf(s, "\tdev=%8s rx=%llu, tx=%llu\n", sled_dev->dev->name,
	           sled_dev->stat.rx_packets, sled_dev->stat.tx_packets);
	return 0;
}

static struct seq_operations soft_led_seq_ops = {
	.start = soft_led_seq_start,
	.next  = soft_led_seq_next,
	.stop  = soft_led_seq_stop,
	.show  = soft_led_seq_show
};

static int soft_led_seq_open(struct inode *inode, struct file *file)
{
	return seq_open_private(file, &soft_led_seq_ops, sizeof(struct sled_iter_private));
}
static const struct file_operations soft_led_proc_ops = {
	.owner		= THIS_MODULE,
	.open		= soft_led_seq_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release_private,
};

int  soft_led_proc_init(void)
{
	soft_led_proc = proc_create("soft_led", S_IRUSR | S_IRGRP, NULL, &soft_led_proc_ops);

	if (!soft_led_proc) {
		printk(KERN_ERR "cannot create /proc/%s\n", "soft_led");
		return -1;
	}

	return 0;
}
void soft_led_proc_fini(void)
{
	if (soft_led_proc)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
		//a8ca16ea7b, if !NULL must be inside, but checked
		proc_remove(soft_led_proc);

#else
		remove_proc_entry(soft_led_proc->name, soft_led_proc->parent);
#endif
}
