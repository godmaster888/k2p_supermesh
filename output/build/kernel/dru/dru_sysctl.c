#include <linux/init.h>
#include <linux/sysctl.h>
#include <dru_sysctl.h>
#include <linux/module.h>



/* storage of dru sysctl data */
struct dru_sysctl_data dru_sysctl_data = {
	.mc_fwd		= CONFIG_DRU_MC_FWD,
#if defined(CONFIG_DRU_UDP_FILTER)
	.udp_filter = {
		.enable		= CONFIG_DRU_UDP_FILTER_ENABLE_DEFVAL,
		.threshold	= CONFIG_DRU_UDP_FILTER_THRESHOLD,
		.cpuload	= CONFIG_DRU_UDP_FILTER_CPULOAD,
		.cpuload_avenrun= 0,	/* recalc it */
		.idle		= CONFIG_DRU_UDP_FILTER_IDLE,
		.pppoe		= CONFIG_DRU_UDP_FILTER_PPPOE,
		.heuristic_utp	= CONFIG_DRU_UDP_FILTER_HEURISTIC_UTP,
		.allow_lan	= CONFIG_DRU_UDP_FILTER_ALLOW_LAN
	},
#endif
#if defined(CONFIG_DRU_LOAD_FILTER)
	.load_filter = {
		.enable		= CONFIG_DRU_LOAD_FILTER_ENABLE_DEFVAL,
		.cpuload_min	= CONFIG_DRU_LOAD_FILTER_CPULOAD_MIN,
		.cpuload_min_avenrun = 0,	/* recalc it */
		.cpuload_max	= CONFIG_DRU_LOAD_FILTER_CPULOAD_MAX,
		.cpuload_max_avenrun = 0	/* recalc it */
	},
#endif
#if defined (CONFIG_DRU_RA_VLANS)
	.ra_vlans = {
		.lan_vid	= {CONFIG_DRU_RA_LAN_VID, 0, 0, 0},
		.lan_vid_be16	= {__constant_htons(CONFIG_DRU_RA_LAN_VID), 0, 0, 0},
		.wan_vid	= CONFIG_DRU_RA_WAN_VID,
		.wan_vid_be16	= __constant_htons(CONFIG_DRU_RA_WAN_VID)
	},
#endif
#if defined(CONFIG_DRU_IDLE_FILTER)
	.idle_filter = {
		.enable		= CONFIG_DRU_IDLE_FILTER_ENABLE_DEFVAL,
		.cpuidle_min	= CONFIG_DRU_IDLE_FILTER_CPUIDLE_MIN,
		.cpuidle_max	= CONFIG_DRU_IDLE_FILTER_CPUIDLE_MAX
	},
#endif
#if defined(CONFIG_DRU_RAWIFI)
	.wifi_task_hi_prio	= CONFIG_DRU_RAWIFI_TASKLET_HIGH,
	.wifi_delay_msec	= CONFIG_DRU_RAWIFI_THROTTLE_DELAY,
	.wifi_delay_ticks	= 0, /* recalc it*//*CONFIG_DRU_RAWIFI_THROTTLE_DELAY * HZ / 1000,*/
	.wifi_is_throttled	= 0,
	.wifi_autochannel_timeo_ms = CONFIG_DRU_RAWIFI_AUTOCHANNEL_TIMEOUT,
#endif
#if defined(CONFIG_DRU_L2TP_FAST_HELLO_REPLY)
	.l2tp_fhr		= CONFIG_DRU_L2TP_FHR_DEFVAL,
#endif
#if defined(CONFIG_DRU_PPPOE_AVOID_PADT_ZOMBIE)
	.avoid_pppoe_zombie	= CONFIG_DRU_PPPOE_AVOID_PADT_ZOMBIE_DEFVAL,
#endif
#if defined(CONFIG_DRU_BRIDGE_NAT_LOOPBACK)
	.br_nat_loopback	= CONFIG_DRU_BRIDGE_NAT_LOOPBACK_DEFVAL,
#endif

};

EXPORT_SYMBOL(dru_sysctl_data);

#ifdef CONFIG_SYSCTL
static int __maybe_unused zero = 0;
static int __maybe_unused one  = 1;
static int __maybe_unused two  = 2;
static int __maybe_unused _100  = 100;
static int __maybe_unused _200  = 200;
static int __maybe_unused _1000  = 1000;


#if defined(CONFIG_DRU_UDP_FILTER) || defined(CONFIG_DRU_LOAD_FILTER)
static unsigned long calc_avenrun(unsigned int percents)
{
	unsigned long av = 0;
	if (percents) {
		av = (percents / 100) << FSHIFT;//integer part
		av |= ((percents % 100) << FSHIFT) / 100; //frac part
	}
	printk(KERN_INFO "dru filter: %u%% -> 0x%08x avenrun\n", percents, (unsigned)av);
	return av;
}
static int aven_dointvec_minmax(struct ctl_table *table, int write,
				void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int res = proc_dointvec_minmax(table, write, buffer, lenp, ppos);
	if (!res && write) {
		unsigned int *x;
		unsigned long *y;
#if defined(CONFIG_DRU_UDP_FILTER)
		if (table->data == &dru_sysctl_data.udp_filter.cpuload) {
			x = &dru_sysctl_data.udp_filter.cpuload;
			y = &dru_sysctl_data.udp_filter.cpuload_avenrun;
		}
		else
#endif
#if defined(CONFIG_DRU_LOAD_FILTER)
		if (table->data == &dru_sysctl_data.load_filter.cpuload_min) {
			x = &dru_sysctl_data.load_filter.cpuload_min;
			y = &dru_sysctl_data.load_filter.cpuload_min_avenrun;
		}
		else
		if (table->data == &dru_sysctl_data.load_filter.cpuload_max) {
			x = &dru_sysctl_data.load_filter.cpuload_max;
			y = &dru_sysctl_data.load_filter.cpuload_max_avenrun;
		}
		else
#endif
		{
			printk(KERN_CRIT "dru filter: unhandled ctl table %s!\n", table->procname);
			return -ENOSYS;
		}
		*y = calc_avenrun(*x);
	}
	return res;
}
#endif


#if defined(CONFIG_DRU_RA_VLANS)

static int __maybe_unused _4095  = 4095;

static int ra_vlans_dointvec_minmax(struct ctl_table *table, int write,
				    void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int res = proc_dointvec_minmax(table, write, buffer, lenp, ppos);
	if (!res && write) {
		unsigned int *x;
		__be16 *y;
		size_t lan = 0;
		if (table->data == dru_sysctl_data.ra_vlans.lan_vid) {
			lan = 1;
			x = dru_sysctl_data.ra_vlans.lan_vid;
			y = dru_sysctl_data.ra_vlans.lan_vid_be16;
		}
		else
		if (table->data == &dru_sysctl_data.ra_vlans.wan_vid) {
			x = &dru_sysctl_data.ra_vlans.wan_vid;
			y = &dru_sysctl_data.ra_vlans.wan_vid_be16;
		}
		else
		{
			printk(KERN_CRIT "dru filter: unhandled ctl table %s!\n", table->procname);
			return -ENOSYS;
		}
		if (lan) {
			int nz = 0;
			for (lan = 0; lan < ARRAY_SIZE(dru_sysctl_data.ra_vlans.lan_vid); lan++) {
				if (*x) nz++;
				*y++ = htons((unsigned short)(*x++));
			}
			if (!nz) {
				/* 'all zeroes' array passed */
				dru_sysctl_data.ra_vlans.lan_vid[0] = CONFIG_DRU_RA_LAN_VID;
				dru_sysctl_data.ra_vlans.lan_vid_be16[0] = __constant_htons(CONFIG_DRU_RA_LAN_VID);
			}
		} else {
			/* wan_vid -> wan_vid_be16 */
			*y++ = htons((unsigned short)(*x++));
		}
	}
	return res;
}

/* /proc/sys/dru/ra_vlans/ content */
static ctl_table ra_vlans_table[] = {
	{
		.procname	= "lan_vid",
		.data		= dru_sysctl_data.ra_vlans.lan_vid,
		.maxlen		= sizeof(dru_sysctl_data.ra_vlans.lan_vid),
		.mode		= 0644,
		.proc_handler	= ra_vlans_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &_4095,
	},
	{
		.procname	= "wan_vid",
		.data		= &dru_sysctl_data.ra_vlans.wan_vid,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= ra_vlans_dointvec_minmax,
		.extra1		= &one,
		.extra2		= &_4095,
	},
	{}
};
#endif

#if defined(CONFIG_DRU_LOAD_FILTER)
/* /proc/sys/dru/load_filter/ content */
static ctl_table load_filter_table[] = {
	{
		.procname	= "enable",
		.data		= &dru_sysctl_data.load_filter.enable,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &one,
	},
	{
		.procname	= "cpuload_min",
		.data		= &dru_sysctl_data.load_filter.cpuload_min,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= aven_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &_1000,
	},
	{
		.procname	= "cpuload_max",
		.data		= &dru_sysctl_data.load_filter.cpuload_max,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= aven_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &_1000,
	},
	{}
};
#endif

#if defined(CONFIG_DRU_IDLE_FILTER)
/* /proc/sys/dru/idle_filter/ content */
static ctl_table idle_filter_table[] = {
	{
		.procname	= "enable",
		.data		= &dru_sysctl_data.idle_filter.enable,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &one,
	},
	{
		.procname	= "cpuidle_min",
		.data		= &dru_sysctl_data.idle_filter.cpuidle_min,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &_100,
	},
	{
		.procname	= "cpuidle_max",
		.data		= &dru_sysctl_data.idle_filter.cpuidle_max,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &_100,
	},
	{}
};
#endif


#if defined(CONFIG_DRU_UDP_FILTER_STAT)
struct dru_filter_stat udp_filter_stat = {
	.omitted	= 0,
	.accepted	= 0,
	.dropped	= 0
};
#endif
#if defined(CONFIG_DRU_LOAD_FILTER_STAT)
struct dru_filter_stat load_filter_stat = {
	.omitted	= 0,
	.accepted	= 0,
	.dropped	= 0
};
#endif
#if defined(CONFIG_DRU_IDLE_FILTER_STAT)
struct dru_filter_stat idle_filter_stat = {
	.omitted	= 0,
	.accepted	= 0,
	.dropped	= 0
};
EXPORT_SYMBOL(idle_filter_stat);
#endif


#if defined(CONFIG_DRU_UDP_FILTER)
static int _1480 = 1480;

/* /proc/sys/dru/udp_filter/ content */
static ctl_table udp_filter_table[] = {
	{
		.procname	= "enable",
		.data		= &dru_sysctl_data.udp_filter.enable,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &one,
	},
	{
		.procname	= "threshold",
		.data		= &dru_sysctl_data.udp_filter.threshold,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &one,
		.extra2		= &_1480,
	},
	{
		.procname	= "cpuload",
		.data		= &dru_sysctl_data.udp_filter.cpuload,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= aven_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &_200,
	},
	{
		.procname	= "idle",
		.data		= &dru_sysctl_data.udp_filter.idle,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &_100,
	},
	{
		.procname	= "pppoe",
		.data		= &dru_sysctl_data.udp_filter.pppoe,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &one,
	},
	{
		.procname	= "heuristic_utp",
		.data		= &dru_sysctl_data.udp_filter.heuristic_utp,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &one,
	},
	{
		.procname	= "allow_lan",
		.data		= &dru_sysctl_data.udp_filter.allow_lan,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &one,
	},
	{}
};
#endif

#if defined(CONFIG_DRU_RAWIFI)

static int _10000 = 10000;
static int _3000  = 3000;

static int wifi_delay_dointvec_minmax(ctl_table *table, int write,
				      void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int res = proc_dointvec_minmax(table, write, buffer, lenp, ppos);
	if (!res && write) {
		dru_sysctl_data.wifi_delay_ticks =
		msecs_to_jiffies(dru_sysctl_data.wifi_delay_msec);
		printk(KERN_INFO "dru rawifi throttle: %u msecs -> %u jiffies\n",
		       dru_sysctl_data.wifi_delay_msec, dru_sysctl_data.wifi_delay_ticks);
	}
	return res;
}
#endif

/* /proc/sys/dru/ content */
static ctl_table dru_table[] = {
	{
		.procname	= "mc_fwd",
		.data		= &dru_sysctl_data.mc_fwd,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &one,
	},
#if defined(CONFIG_DRU_UDP_FILTER)
	{
		.procname	= "udp_filter",
		.mode		= 0555,
		.child		= udp_filter_table,
	},
#endif
#if defined(CONFIG_DRU_LOAD_FILTER)
	{
		.procname	= "load_filter",
		.mode		= 0555,
		.child		= load_filter_table,
	},
#endif
#if defined(CONFIG_DRU_RA_VLANS)
	{
		.procname	= "ra_vlans",
		.mode		= 0555,
		.child		= ra_vlans_table,
	},
#endif
#if defined(CONFIG_DRU_IDLE_FILTER)
	{
		.procname	= "idle_filter",
		.mode		= 0555,
		.child		= idle_filter_table,
	},
#endif
#if defined(CONFIG_DRU_RAWIFI)
	{
		.procname	= "wifi_task_hi_prio",
		.data		= &dru_sysctl_data.wifi_task_hi_prio,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &one,
	},
	{
		.procname	= "wifi_delay_msec",
		.data		= &dru_sysctl_data.wifi_delay_msec,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= wifi_delay_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &_10000,
	},
	{
		.procname	= "wifi_is_throttled",
		.data		= &dru_sysctl_data.wifi_is_throttled,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0444,
		.proc_handler	= proc_dointvec_minmax,
		/*
		.extra1		= &zero,
		.extra2		= &_10000,
		*/
	},
	{
		.procname	= "rawifi_autochannel_timeout",
		.data		= &dru_sysctl_data.wifi_autochannel_timeo_ms,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= wifi_delay_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &_3000,
	},
#endif
#if defined(CONFIG_DRU_L2TP_FAST_HELLO_REPLY)
	{
		.procname	= "l2tp_fhr",
		.data		= &dru_sysctl_data.l2tp_fhr,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &two,
	},
#endif
#if defined(CONFIG_DRU_PPPOE_AVOID_PADT_ZOMBIE)
	{
		.procname	= "avoid_pppoe_zombie",
		.data		= &dru_sysctl_data.avoid_pppoe_zombie,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &one,
	},
#endif
#if defined(CONFIG_DRU_BRIDGE_NAT_LOOPBACK)
	{
		.procname	= "br_nat_loopback",
		.data		= &dru_sysctl_data.br_nat_loopback,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &zero,
		.extra2		= &one,
	},
#endif
	{}
};

/* /proc/sys/dru subdir item */
static ctl_table dru_root_table[] = {
	{
		.procname	= "dru",
		.mode		= 0555,
		.child		= dru_table,
	},
	{}
};

static struct ctl_table_header *dru_sysctl_header = NULL;

static int dru_sysctl_table_register(void)
{
#if defined(CONFIG_DRU_UDP_FILTER)
	dru_sysctl_data.udp_filter.cpuload_avenrun = calc_avenrun(dru_sysctl_data.udp_filter.cpuload);
#endif
#if defined(CONFIG_DRU_LOAD_FILTER)
	dru_sysctl_data.load_filter.cpuload_min_avenrun = calc_avenrun(dru_sysctl_data.load_filter.cpuload_min);
	dru_sysctl_data.load_filter.cpuload_max_avenrun = calc_avenrun(dru_sysctl_data.load_filter.cpuload_max);
#endif
#if defined(CONFIG_DRU_RAWIFI)
	dru_sysctl_data.wifi_delay_ticks = msecs_to_jiffies(dru_sysctl_data.wifi_delay_msec);
#endif
	//не проверяем, что вызвано более одного раза
	//просто убьём того, кто это сделает
	dru_sysctl_header = register_sysctl_table(dru_root_table);
	if (unlikely(!dru_sysctl_header)) {
		printk(KERN_ERR "Cannot register /proc/sys/dru\n");
		return -1;// ENOMEM ?
	}
	printk(KERN_INFO "Registered /proc/sys/dru\n");
	return 0;
}
static void dru_sysctl_table_unregister(void)
{
	if (likely(dru_sysctl_header))
		unregister_sysctl_table(dru_sysctl_header);
}

#else /* CONFIG_SYSCTL */
static inline int dru_sysctl_table_register(void) {return 0;}
static inline void dru_sysctl_table_unregister(void) {}
#endif /* CONFIG_SYSCTL */


int dru_sysctl_init(void)
{
	dru_sysctl_table_register();//without check
	return 0;
}

void dru_sysctl_exit(void)
{
	/* will be called never (built-in) */
	dru_sysctl_table_unregister();
}
