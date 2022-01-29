# Makefile from Netlink monitor driver

obj-$(CONFIG_NETLINK_MONITOR) := nlmon_mod.o
ifeq ($(CONFIG_NETLINK_MONITOR_NEW),y)
nlmon_mod-objs := nlmon_new.o
else
nlmon_mod-objs := nlmon.o
endif
nlmon_mod-$(CONFIG_NETLINK_MONITOR_LED_CONTROL) += led.o



ifneq ($(CONFIG_NETLINK_MONITOR_SOFT_LED),)
nlmon_mod-y += soft_led.o
nlmon_mod-$(CONFIG_PROC_FS) += soft_led_procfs.o
endif
