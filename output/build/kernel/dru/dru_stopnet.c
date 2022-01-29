
//#include <linux/module.h>
#include <dru_sysctl.h>
#include <net/net_namespace.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#include <linux/reboot.h>

//MODULE_AUTHOR("Alexander farisey Nickulin <anikulin@dlink.ru>");
//MODULE_DESCRIPTION("");
//MODULE_LICENSE("GPL");


int dru_netstop(struct notifier_block *nb, unsigned long code, void *data)
{
	struct net *net;
	struct net_device *dev;

	rtnl_lock();
	for_each_net(net) {
		for_each_netdev(net, dev) {
			printk("..stop iface %s\n", dev->name);
			dev_close(dev);
		}
	}
	rtnl_unlock();
	return 0;
}

static struct notifier_block nb = {
	.notifier_call = dru_netstop
};

int  dru_netstop_init(void)
{
	return register_reboot_notifier(&nb);
}

void dru_netstop_exit(void)
{
	unregister_reboot_notifier(&nb);
}
