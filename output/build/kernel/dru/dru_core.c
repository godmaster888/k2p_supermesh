#include <dru_sysctl.h>
#include <linux/module.h>

MODULE_AUTHOR("Alexander farisey Nickulin <anikulin@dlink.ru>");
//MODULE_DESCRIPTION("");
//MODULE_LICENSE("");

static int __init dru_init(void)
{
	//without check
	dru_proc_init();
	dru_sysctl_init();
	dru_sysstat_init();
#if defined(CONFIG_DRU_RT_3052_ESW)
	dru_esw_init();
#endif
#if defined(CONFIG_DRU_STOP_NET_ON_REBOOT)
	dru_netstop_init();
#endif
	return 0;
}
/*
 * sysctl has core_initcall() level
 * therefore postcore_initcall() is sufficient.
 * But i'm paranoid :)))
 */
fs_initcall(dru_init);

static void __exit dru_exit(void)
{
	/* will be called never (built-in) */
#if defined(CONFIG_DRU_STOP_NET_ON_REBOOT)
	dru_netstop_exit();
#endif
#if defined(CONFIG_DRU_RT_3052_ESW)
	dru_esw_cleanup();
#endif
	dru_sysstat_cleanup();
#if defined(CONFIG_DRU_RAWIFI)
	dru_wifi_cleanup();
#endif
	dru_sysctl_exit();
	dru_proc_exit();
}
module_exit(dru_exit);
