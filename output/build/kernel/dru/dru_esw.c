#include <dru_sysctl.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <asm/atomic.h>

#include <asm/rt2880/surfboardint.h>
#include "ra2882ethreg.h"	/* drivers/net/raeth */

#if defined(CONFIG_NETLINK_MONITOR)
#include "net/nlmon/netlinkmon.h"
#endif

#ifndef RALINK_REG
#define RALINK_REG(x)	(*((volatile u32 *)(x)))
#endif

int  dru_esw_init(void);
void dru_esw_cleanup(void);

static irqreturn_t esw_irqh(int irq, void *null_dev);
static int req_irq_err = 0;

#if   defined (CONFIG_RALINK_RT3052) || \
      defined (CONFIG_RALINK_RT3352) || \
      defined (CONFIG_RALINK_RT5350) || \
      defined (CONFIG_RALINK_MT7628)
static unsigned long stat_val = 0;// let all be off
#endif



static irqreturn_t esw_irqh(int irq, void *null_dev)
{
	/*
	 * WARNING нету лочки вокруг &(ei_local->page_lock) из dev_id
	 */
	unsigned long isr, i;

#if !defined(CONFIG_RALINK_MT7621)
	isr = RALINK_REG(ESW_ISR);//стр. 338
#else
	mii_mgr_read(31, 0x700c, &isr);
#endif

	/* проанализируем данные */
#if   defined (CONFIG_RALINK_RT3052) || \
      defined (CONFIG_RALINK_RT3352) || \
      defined (CONFIG_RALINK_RT5350) || \
      defined (CONFIG_RALINK_MT7628)

	if (isr & PORT_ST_CHG) {
		unsigned long r = RALINK_REG(RALINK_ETH_SW_BASE + 0x80), mask;

		printk("ESW: Link Status Changed\n");

		for (i = 25, mask = (1 << 25); i <= 30; i++, mask <<= 1)
			if ( (stat_val & mask) != (r & mask) ) {
				struct nlmon_msg_data nlmon_msg = {
					.sw = 0,
					.phy = (i - 25),
					.status = ((r & mask) >> i) // (r & mask) ? 1 : 0
				};
#ifdef CONFIG_NETLINK_MONITOR_NEW
				nlmon_handle_event(&nlmon_msg);
#else
				kerSysSendtoNlMonTask(MSG_NETLINK_LINK_STATUS_CHANGED, (char *) &nlmon_msg, sizeof(nlmon_msg));
#endif
			}

		stat_val = r;
	}

#elif defined (CONFIG_RALINK_RT6855) || defined (CONFIG_RALINK_RT6855A) || \
      defined (CONFIG_RALINK_MT7620) || \
      defined (CONFIG_RALINK_MT7621)

	/*
	 * влом делать копипасту из P0_LINK_CH, P1_LINK_CH, ...
	 * как это сделать макросами ... наверное, никак без расширений
	 * в ra2882ethreg.h,
	 * но т.к. макросы одинаковые, то сделаю циклом
	 * Кстати, для 7621 заявлены 0-5, но юзаются 0-4 (и маска 0x1f)
	 */
#if defined (CONFIG_RALINK_MT7621)

	for (i = 0; i < 5; i++) {
		unsigned long r;
		mii_mgr_read(31, (0x3008 + (i * 0x100)), &r);
#else

	for (i = 0; i < 6; i++) {
		unsigned long r = RALINK_REG(RALINK_ETH_SW_BASE + 0x3008 + (i * 0x100));//стр. 332-333
#endif

		if (isr & BIT(i)) {
			struct nlmon_msg_data nlmon_msg = {
				.sw = 0,
				.phy = i,
				.status = (r & 0x1)
			};
			printk("ESW: Link Status Changed - Port%d Link %s\n", (int)i, (r & 0x1) ? "Up" : "Down");
#ifdef CONFIG_NETLINK_MONITOR_NEW
			nlmon_handle_event(&nlmon_msg);
#else
			kerSysSendtoNlMonTask(MSG_NETLINK_LINK_STATUS_CHANGED, (char *) &nlmon_msg, sizeof(nlmon_msg));
#endif
		}
	}

#else
#error define your arch!
#endif
	//reset back status bits
#if !defined(CONFIG_RALINK_MT7621)
	RALINK_REG(ESW_ISR) = isr;//sysRegWrite()/KSEG1ADDR ?
#else
	mii_mgr_write(31, 0x700c, 0x1f); //ack switch link change
#endif

	return IRQ_HANDLED;
}



int  dru_esw_init(void)
{
	/*
	 * Kconfig гарантирует, что не будет
	 * конфликта с Extended Switch в raeth.o
	 */
	printk(KERN_INFO "DRU: init ESW\n");
#if   defined (CONFIG_RALINK_RT3052) || \
      defined (CONFIG_RALINK_RT3352) || \
      defined (CONFIG_RALINK_RT5350) || \
      defined (CONFIG_RALINK_MT7628)
	stat_val = RALINK_REG(RALINK_ETH_SW_BASE + 0x80);
#endif
	req_irq_err = request_irq(SURFBOARDINT_ESW, esw_irqh, IRQF_DISABLED, "Ralink_DRU_ESW", NULL);

	if (req_irq_err)
		return req_irq_err;

	/*
	 * понавключаем все эти прерывания
	 */
#if defined (CONFIG_RALINK_MT7621)
	// на это доки нет, берём as-is из raether.c
	mii_mgr_write(31, 0x7008, 0x1f); //enable switch link change intr
#else
	RALINK_REG(RALINK_INTCL_BASE + 0x34) |= (1 << 17); //стр. 56, ESW бит
	RALINK_REG(ESW_IMR) &= ~(P0_LINK_CH | P1_LINK_CH | P2_LINK_CH | P3_LINK_CH | P4_LINK_CH | P5_LINK_CH);//~(ESW_INT_ALL); // стр. 338
	//к чёрту ESW_P{N}_IntMn
#endif

	return 0;
}
void dru_esw_cleanup(void)
{
	printk(KERN_WARNING "DRU: ESW: we do not return registers to the original state\n");

	if (!req_irq_err) free_irq(SURFBOARDINT_ESW, NULL);
}
