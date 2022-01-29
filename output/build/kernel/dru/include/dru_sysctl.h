#ifndef DRU_SYSCTL_H
#define DRU_SYSCTL_H

#include <linux/sched.h>	/* avenrun */


struct dru_filter_stat {
	unsigned long long omitted;
	unsigned long long accepted;
	unsigned long long dropped;
};

struct dru_sysstat {
	unsigned period;
	struct {
		u64 last;//internal
		u64 diff;
		unsigned percents;
	} idle;
};
extern struct dru_sysstat dru_sysstat;

#if defined(CONFIG_DRU_UDP_FILTER_STAT)
extern struct dru_filter_stat udp_filter_stat;
#endif

#if defined(CONFIG_DRU_LOAD_FILTER_STAT)
extern struct dru_filter_stat load_filter_stat;
#endif

#if defined(CONFIG_DRU_IDLE_FILTER_STAT)
extern struct dru_filter_stat idle_filter_stat;
#endif

struct dru_sysctl_data {
	unsigned int mc_fwd;
#if defined(CONFIG_DRU_UDP_FILTER)
	struct {
		unsigned int enable;
		unsigned int threshold;
		unsigned int cpuload;	/*percents */
		unsigned long cpuload_avenrun;	/* in avenrun */
		unsigned int idle;
		unsigned int pppoe;
		unsigned int heuristic_utp;
		unsigned int allow_lan;
	} udp_filter;
#endif
#if defined(CONFIG_DRU_LOAD_FILTER)
	struct {
		unsigned int  enable;
		unsigned int  cpuload_min;		/* percents */
		unsigned long cpuload_min_avenrun;	/* in avenrun */
		unsigned int  cpuload_max;		/* percents */
		unsigned long cpuload_max_avenrun;	/* in avenrun */
	} load_filter;
#endif
#if defined (CONFIG_DRU_RA_VLANS)
	struct {
		unsigned int	lan_vid[4];
		__be16		lan_vid_be16[4];
		unsigned int	wan_vid;
		__be16		wan_vid_be16;
	} ra_vlans;
#endif
#if defined(CONFIG_DRU_IDLE_FILTER)
	struct {
		unsigned int  enable;
		unsigned int  cpuidle_min;		/* percents */
		unsigned int  cpuidle_max;		/* percents */
	} idle_filter;
#endif
#if defined(CONFIG_DRU_RAWIFI)
	unsigned int wifi_task_hi_prio;
	unsigned int wifi_delay_msec;
	unsigned int wifi_delay_ticks;	/* recalc from msec */
	int wifi_is_throttled;		/* read-only for external !! */
	unsigned int wifi_autochannel_timeo_ms;
#endif
#if defined(CONFIG_DRU_L2TP_FAST_HELLO_REPLY)
	unsigned l2tp_fhr;
#endif
#if defined(CONFIG_DRU_PPPOE_AVOID_PADT_ZOMBIE)
	unsigned avoid_pppoe_zombie;
#endif
#if defined(CONFIG_DRU_BRIDGE_NAT_LOOPBACK)
	unsigned br_nat_loopback;
#endif
};

extern struct dru_sysctl_data dru_sysctl_data;

#if defined(CONFIG_DRU_RAWIFI)
extern void __dru_wifi_throttle(void);
static inline int dru_wifi_throttle(void)
{
	if (dru_sysctl_data.wifi_delay_ticks) {
		__dru_wifi_throttle();
		return 1;
	}

	return 0;
}
extern void dru_wifi_cleanup(void);
#endif

extern int  dru_sysctl_init(void);
extern void dru_sysctl_exit(void);

extern int  dru_proc_init(void);
extern void dru_proc_exit(void);

extern int  dru_sysstat_init(void);
extern void dru_sysstat_cleanup(void);

#if defined(CONFIG_DRU_RT_3052_ESW)
extern int  dru_esw_init(void);
extern void dru_esw_cleanup(void);
#endif

#if defined(CONFIG_DRU_STOP_NET_ON_REBOOT)
extern int  dru_netstop_init(void);
extern void dru_netstop_exit(void);
#endif

#endif
