#include <dru_sysctl.h>
#include <linux/workqueue.h>
#include <asm/cputime.h>
#include <linux/kernel_stat.h>
#include <linux/module.h>	/* EXPORT_SYMBOL */



struct dru_sysstat dru_sysstat = {
	.period = HZ,		/* once per sec */
	.idle = {
		.last = 0,
		.diff = 0
	}
};
EXPORT_SYMBOL(dru_sysstat);


int  dru_sysstat_init(void);
void dru_sysstat_cleanup(void);

static void calc_sysstat(struct work_struct *work);

static DECLARE_DELAYED_WORK(sysstat_work, calc_sysstat);


//must be in asm/cputime.h
#ifndef arch_idle_time
#define arch_idle_time(cpu) 0
#endif

static void calc_sysstat(struct work_struct *work)
{
	int i;
	cputime64_t idle = 0;//cputime64_zero;

	for_each_possible_cpu(i) {
		//idle = cputime64_add(idle, kstat_cpu(i).cpustat.idle);
		//idle = cputime64_add(idle, arch_idle_time(i));
		idle += kcpustat_cpu(i).cpustat[CPUTIME_IDLE];

		if (cpu_online(i) && !nr_iowait_cpu(i))
			idle += arch_idle_time(i);
	}
	dru_sysstat.idle.diff = cputime64_to_jiffies64(idle) - dru_sysstat.idle.last;
	dru_sysstat.idle.last = cputime64_to_jiffies64(idle);

	if (likely(dru_sysstat.period)) {
		dru_sysstat.idle.percents = 100U * (unsigned)dru_sysstat.idle.diff / dru_sysstat.period;
		schedule_delayed_work(&sysstat_work, dru_sysstat.period);
	} else
		dru_sysstat.idle.percents = 0;
}

int  dru_sysstat_init(void)
{
	if (dru_sysstat.period)
		schedule_delayed_work(&sysstat_work, dru_sysstat.period);

	return 0;
}

void dru_sysstat_cleanup(void)
{
	if (delayed_work_pending(&sysstat_work))
		cancel_delayed_work(&sysstat_work);
}
