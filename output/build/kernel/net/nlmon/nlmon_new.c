/* nlmon.c */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <net/genetlink.h>
#include <linux/netlink.h>
#include <net/nlmon/nlmon.h>
#include <linux/hardirq.h>

#if defined(CONFIG_NETLINK_MONITOR_LED_CONTROL)
//может всё из того хидера перетащить в nlmon.h
//и снести его, как у Светки?
//Вернее оставить там один include nlmon.h
//для обратной совместимости со всяким софтом.
//
//asafonov: пока оставим как есть, внесем минимум изменений.
//Возможно, в будущем проведем рефакторинг.
#include <net/nlmon/netlinkmon.h>
#endif

/*
#if defined(CONFIG_DRU_RT_3052_ESW)
atomic_t nlmon_is_active = ATOMIC_INIT(0);
#endif
*/

/** Определение группы семейства */
static struct genl_multicast_group nlmon_mcgrp = {
	.name = NLMON_MCAST_GROUP_NAME
};

/** Определение семейства  genetlink*/
static struct genl_family nlmon_family = {
	.id = GENL_ID_GENERATE,
	.hdrsize = 0,
	.name = NLMON_FAMILY_NAME,
	.version = 1,
	.maxattr = NLMON_A_MAX,
};


/**
 * Обработчик события кабель вставлен/вынут.
 *
 * @param phy		номер порта
 * @param status 	статус порта
 */
void nlmon_handle_event(struct nlmon_msg_data *msg_data)
{
	struct sk_buff *msg;
	int rc;
	void *hdr;
	gfp_t flags;

#if defined(CONFIG_NETLINK_MONITOR_LAN_LED)
	dru_led_link_changed(msg_data->phy, msg_data->status);
#endif

	if (in_irq() || in_atomic())
		flags = GFP_ATOMIC;
	else
		flags = GFP_KERNEL;

	/* формирование сообщения*/
	msg = genlmsg_new(NLMSG_GOODSIZE, flags);

	if (msg == NULL)
		goto out;

	hdr = genlmsg_put(msg, 0, 0, &nlmon_family, 0, NLMON_C_LINK_REG);

	if (!hdr)
		goto out_free;

	rc = nla_put_u8(msg, NLMON_A_PHY, msg_data->phy);

	if (rc != 0)
		goto out_free;

	rc = nla_put_u8(msg, NLMON_A_STATUS_FLAG, msg_data->status);

	if (rc != 0)
		goto out_free;

	rc = nla_put_u8(msg, NLMON_A_SWITCH_NUMBER, msg_data->sw);

	if (rc != 0)
		goto out_free;

	rc = nla_put_u8(msg, NLMON_A_PHY_TYPE, msg_data->phy_type);

	if (rc != 0)
		goto out_free;

	/* закончили формирование сообщения */
	genlmsg_end(msg, hdr);

	/* отправка сообщения подписчикам группы*/
	genlmsg_multicast(msg, 0, nlmon_mcgrp.id, flags);
	goto out;

out_free:
	nlmsg_free(msg);
out:
	return;
}

/**
 * Инициализация модуля.
 *
 * @return 0 при успешном завершении.
 */
int __init nlmon_init(void)
{
	int ret;
	printk(KERN_INFO "nlmon: module starting ...");

#if defined(CONFIG_NETLINK_MONITOR_LED_CONTROL)
	dru_led_init();
#endif
	ret = genl_register_family(&nlmon_family);

	if (ret) {
		printk("nlmon: error in registering generic netlink family (%d).", ret);
		goto out;
	}

	ret = genl_register_mc_group(&nlmon_family, &nlmon_mcgrp);

	if (ret) {
		printk("nlmon: error in registering multicast group for generic netlink family (%d).", ret);
		goto out_unreg_fam;
	}

	return 0;

out_unreg_fam:
	genl_unregister_family(&nlmon_family);
out:
#if defined(CONFIG_NETLINK_MONITOR_LAN_LED)
	dru_led_fini();
#endif
	return ret;
}

/**
 * Выгрузка модуля.
 */
void __exit nlmon_exit(void)
{
	printk(KERN_INFO "nlmon: module exiting ...");

	genl_unregister_family(&nlmon_family);

#if defined(CONFIG_NETLINK_MONITOR_LAN_LED)
	dru_led_fini();
#endif
}

module_init(nlmon_init);
module_exit(nlmon_exit);
EXPORT_SYMBOL(nlmon_handle_event);
