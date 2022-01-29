/* nlmon.h */
#ifndef _NLMON_H
#define _NLMON_H

/**
 * @file
 * Внешний заголовочный файл для взаимодействия с пользовательским пространством.
 * Для исключения конфликтов имён используется префикс nlmon.
 */

struct nlmon_msg_data {
	int phy;
	int status;
	int sw;
	int phy_type;
};

enum {
	NLMON_PORT_DISABLE,
	NLMON_PORT_ENABLE,
};

#ifdef CONFIG_NETLINK_MONITOR_NEW

/**
 * Название семейства Generic Netlink.
 */
#define NLMON_FAMILY_NAME        "nlmon"
#define NLMON_MCAST_GROUP_NAME   "nlmon_grp"

/**
 * Перечисление возможных атрибутов.
 */
enum {
	NLMON_A_UNSPEC,
	NLMON_A_PHY,
	NLMON_A_STATUS_FLAG,
	NLMON_A_SWITCH_NUMBER,
	NLMON_A_PHY_TYPE,
	__NLMON_A_MAX,
};

#define NLMON_A_MAX (__NLMON_A_MAX - 1)

/**
 * Перечисление возможных команд.
 */
enum {
	NLMON_C_UNSPEC,
	NLMON_C_LINK_REG,
	__NLMON_C_MAX,
};

#ifdef __KERNEL__
#include <linux/types.h>
extern void nlmon_handle_event(struct nlmon_msg_data *msg_data);
#endif


#else /* CONFIG_NETLINK_MONITOR_NEW */
typedef struct nlmonIoctParms {
	int offset;
	int result;
} NLMON_IOCTL_PARMS;

#define NLMON_DRV_MAJOR 202
#define NLMON_DRV_NAME "nlmon"

#define NLMON_IOCTL_MAGIC 'B'

#define NLMON_IOCTL_SET_MONITOR_FD _IOWR (NLMON_IOCTL_MAGIC, 1, NLMON_IOCTL_PARMS)

#define NLMON_IOCTL_WAKEUP_MONITOR_TASK _IOWR (NLMON_IOCTL_MAGIC, 2, NLMON_IOCTL_PARMS)


#endif

#endif /* _NLMON_H */
