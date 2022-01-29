#
# Makefile for D-Link Russia R&D stuff
#
# 7 Feb 2014, farisey <anikulin@dlink.ru>
#
ccflags-y := -Idru/include

obj-$(CONFIG_DRU) := dru.o
dru-y := dru_core.o dru_sysctl.o dru_procfs.o
dru-$(CONFIG_DRU_RAWIFI) += dru_wifi.o
dru-y += dru_sysstat.o

dru-$(CONFIG_DRU_RT_3052_ESW) += dru_esw.o
ccflags-$(CONFIG_DRU_RT_3052_ESW) += -Idrivers/net/raeth

dru-$(CONFIG_DRU_STOP_NET_ON_REBOOT) += dru_stopnet.o
