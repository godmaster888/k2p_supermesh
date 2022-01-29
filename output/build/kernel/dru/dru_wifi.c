#include <dru_sysctl.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>

void __dru_wifi_throttle(void);
void dru_wifi_cleanup(void);

static void restore_unthrottle(struct work_struct *work);

static DEFINE_SPINLOCK(t_spin);
static DECLARE_DELAYED_WORK(restore_work, restore_unthrottle);


static void restore_unthrottle(struct work_struct *work)
{
	spin_lock(&t_spin);
	dru_sysctl_data.wifi_is_throttled = 0;
	spin_unlock(&t_spin);
}

void __dru_wifi_throttle(void)
{
	if (delayed_work_pending(&restore_work))
		cancel_delayed_work(&restore_work);

	spin_lock(&t_spin);
	dru_sysctl_data.wifi_is_throttled = 1;
	spin_unlock(&t_spin);
	schedule_delayed_work(&restore_work, dru_sysctl_data.wifi_delay_ticks);
}

void dru_wifi_cleanup(void)
{
	if (delayed_work_pending(&restore_work))
		cancel_delayed_work(&restore_work);

	//instead of flush_scheduled_work()
	spin_lock(&t_spin);
	spin_unlock(&t_spin);
}
