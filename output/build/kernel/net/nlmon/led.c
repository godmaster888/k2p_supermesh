/*
 * farisey: #13756
 * UPDATE: and others
 */
#include <net/nlmon/netlinkmon.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#include <asm/rt2880/rt_mmap.h>

#ifndef RALINK_REG
#define RALINK_REG(x) (*((volatile u32 *)(x)))
#endif

#if defined (CONFIG_NETLINK_MONITOR_LAN_LED)
/*
 * если не ноль, то не лезть в леды
 * нужно, чтоб не конфликтовать с лед-тестами
 */
static int lan_led_is_locked = 0;

//состояние всех 4-х портов побитно
static unsigned status = 0; // 0 - down, 1 - up

/*
 * группа гпио 39-24 (стр. 87)
 * на DWR-921 какая-то странная
 * в 0x48 (DATA) лежит 0x1000, и если
 * писать в прочие биты, то оно срабатывает,
 * но при read почему-то не отображается.
 * Тут 2 леда бит 14 - sms (inverted) и
 * бит 15 - LAN (normal!).
 * Так что для мигания ланом, надо хранить
 * его значение.
 * PS. В какой-то момент после бута прочитал там 0x1002,
 * но потом скакануло на 0x1000 и там остаётся.
 *
 * UPDATE: #16813
 * У DWR-953A1 вроде бы нормальные гпио, но пусть
 * логика остаётся такой же - с сохраненённым регистром.
 */
#if defined(CONFIG_NETLINK_MONITOR_LAN_LED_DWR_921C1)

#define LAN_LED_BIT	15
#define PIO_DATA_OFFSET		0x48	/* block 1 */
#define PIO_SET_OFFSET		0x54	/* block 1 */
#define PIO_RESET_OFFSET	0x58	/* block 1 */

#elif defined(CONFIG_NETLINK_MONITOR_LAN_LED_DWR_953A1)

#define LAN_LED_BIT	12
#define PIO_DATA_OFFSET		0x20	/* block 0 */

#else
#error fix your Mediatek board
#endif

#if defined(CONFIG_RALINK_MT7620)
#define PIO_DIR_OFFSET		(PIO_DATA_OFFSET + 0x4)
#elif defined(CONFIG_RALINK_MT7621)
#define PIO_DIR_OFFSET		(PIO_DATA_OFFSET - 0x20)
#else
#error fix your MTK arch
#endif

static unsigned long lan_led = (0UL << LAN_LED_BIT);//выкл.

static inline void apply_led_bit(unsigned long x)
{
#if defined(CONFIG_NETLINK_MONITOR_LAN_LED_DWR_921C1)
	//применим этот костыльный регистр
	// #18906, #20550
	if (x)
		RALINK_REG(RALINK_PIO_BASE + PIO_SET_OFFSET) = x;
	else
		RALINK_REG(RALINK_PIO_BASE + PIO_RESET_OFFSET) = (1UL << LAN_LED_BIT);
#elif defined(CONFIG_NETLINK_MONITOR_LAN_LED_DWR_953A1)

	//на 953 регистр нормальный, а gpio инвертированный
	if (x) //надо включить - сбросим бит в ноль
		RALINK_REG(RALINK_PIO_BASE + PIO_DATA_OFFSET) &= ~x;
	else //выключить - выставить единичку
		RALINK_REG(RALINK_PIO_BASE + PIO_DATA_OFFSET) |= (1UL << LAN_LED_BIT);

#else
#error fix your Mediatek board
#endif
}


static void led_tick(struct work_struct *work);
static DECLARE_DELAYED_WORK(led_work, led_tick);


#if defined (CONFIG_RALINK_RT6855) || defined (CONFIG_RALINK_RT6855A) || \
    defined (CONFIG_RALINK_MT7620)

#define RA_ETH_SW_GET_REG(x) RALINK_REG(RALINK_ETH_SW_BASE + (x))

#elif defined (CONFIG_RALINK_MT7621)
#error move RA_ETH_SW_GET_REG from soft_led.c to common part
#else
#error your platform is not supported
#endif

/* там счётчики 16-битные, больше и не надо */
//static u16 counters[4] = {0, 0, 0, 0};
static unsigned bytes = 0;//а, всё целиком

void dru_led_link_changed(unsigned port, unsigned link)
{
	if (link) {
		status |= (1UL << port);

		if (!delayed_work_pending(&led_work))
			schedule_delayed_work(&led_work, HZ / 2);
	} else
		status &= ~(1UL << port);
}

static void led_tick(struct work_struct *work)
{
	int i;
	unsigned x = 0;
	unsigned long n; //избегаем умножения

#define STATUS_PORT_MASK	0xf	//4 порта LAN

	//просуммируем все RX и TX на лан-портах
	for (i = 0, n = 0; i < 4; i++, n += 0x100) {
		//TX, стр. 332
		x += RA_ETH_SW_GET_REG(0x4010 + n) & 0xffff;//good frames count
		//RX, стр. 333
		x += RA_ETH_SW_GET_REG(0x4020 + n) & 0xffff;//good frames count
	}

	if ((status & STATUS_PORT_MASK) == 0) {
		//патчи вынуты, гасим
		lan_led = (0UL << LAN_LED_BIT);
	}
	else if (bytes != x) {
		//инвертируем LAN-led - мигание
		lan_led ^= (1UL << LAN_LED_BIT);
	} else {
		//хоть один lan-порт link on
		//просто пусть горит
		lan_led = (1UL << LAN_LED_BIT);
	}

	/* применим этот костыльный, несохраняющий состояния регистр */
	if (!lan_led_is_locked) //только если разрешено рулить ледами
		apply_led_bit(lan_led);

	bytes = x;

	if (status & STATUS_PORT_MASK) schedule_delayed_work(&led_work, HZ / 2);
}

#ifdef CONFIG_PROC_FS

static ssize_t dru_led_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	char s[8];
	long val;

	if (*offset) return 0;

	if (count > 4) return -EMSGSIZE;

	if (copy_from_user(s, buf, count)) return -EFAULT;

	s[count] = '\0';

	if (strict_strtol(s, 10, &val)) return -EINVAL;

	lan_led_is_locked = val ? 1 : 0;
	val = strlen(s);
	*offset += val;
	return val;
}

static ssize_t dru_led_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	const char *s = lan_led_is_locked ? "1\n" : "0\n";

	if (*offset) return 0;

	if (count < 2) return -ENOBUFS;

	if (copy_to_user(buf, s, 2)) return -EFAULT;

	*offset += 2;
	return 2;
}

static struct proc_dir_entry *dru_led_proc = NULL;

static const struct file_operations dru_led_proc_ops = {
	.owner	 = THIS_MODULE,
	.read	 = dru_led_read,
	.write	 = dru_led_write,
};
#else
#warning procfs is really needed here!
#endif /* CONFIG_PROC_FS */

#endif /* CONFIG_NETLINK_MONITOR_LAN_LED */

__init int dru_led_init(void)
{
#if defined(CONFIG_NETLINK_MONITOR_LAN_LED)
	int i;

#ifdef CONFIG_PROC_FS
	dru_led_proc = proc_create("lan_led_locked", S_IRUSR | S_IWUSR | S_IRGRP, NULL, &dru_led_proc_ops);

	if (!dru_led_proc) {
		printk(KERN_ERR "cannot create /proc/%s\n", "lan_led_locked");
	}

#endif

	RALINK_REG(RALINK_PIO_BASE + PIO_DIR_OFFSET)  |= (1UL << LAN_LED_BIT); // dir output
	apply_led_bit(lan_led); //off

	for (i = 0; i < 4; i++) {
		unsigned long r = RA_ETH_SW_GET_REG(0x3008 + (i * 0x100));//стр. 332-333
		status |= ((r & 0x1) << i);
	}

	if (status)
		schedule_delayed_work(&led_work, HZ / 2); //invert every 0.5 сек => T = 1 sec

#endif /* CONFIG_NETLINK_MONITOR_LAN_LED */

#if defined(CONFIG_NETLINK_MONITOR_SOFT_LED)
	dru_soft_led_init();
#endif
	return 0;
}

__exit void dru_led_fini(void)
{
#if defined(CONFIG_NETLINK_MONITOR_SOFT_LED)
	dru_soft_led_fini();
#endif

#if defined(CONFIG_NETLINK_MONITOR_LAN_LED)

	if (delayed_work_pending(&led_work))
		cancel_delayed_work(&led_work);

	flush_delayed_work(&led_work);

#ifdef CONFIG_PROC_FS

	if (dru_led_proc) proc_remove(dru_led_proc);

#endif
#endif /* CONFIG_NETLINK_MONITOR_LAN_LED */
}
