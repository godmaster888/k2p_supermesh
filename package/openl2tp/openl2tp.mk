#############################################################
#
# openl2tp
#
#############################################################
OPENL2TP_VERSION = master
#OPENL2TP_SOURCE = openl2tp-$(OPENL2TP_VERSION).tar.gz
OPENL2TP_SITE = $(DLINK_GIT_STORAGE)/openl2tp
OPENL2TP_LICENSE = GPL-2.0-or-later OR PROPRIETARY
OPENL2TP_LICENSE_FILES = LICENSE COPYING
OPENL2TP = $(BUILD_DIR)/openl2tp-$(OPENL2TP_VERSION)
OPENL2TP_DEPENDENCIES = libflex kernel pppd 
OPENL2TP_DIR = $(BUILD_DIR)/openl2tp-$(OPENL2TP_VERSION)

ifeq ($(RALINK_MT7621),y)
# farisey: для новых ядер (тут 3.10.14) нужен дополнительный путь поиска
# заюзать TARGET_ARCH не могу, т.к. там "mipsel", а нужен "mips"
###OPENL2TP_DLINK_EXTRA_CPPFLAGS += -isystem $(LINUX26_DIR)/arch/mips/include
# UPDATE: не надо, сделаю симлинк в uapi в сорцах ядра
# но всё равно тут была бага ниже - закомментаренные строки в MAKE_OPT
# обрезали перенос. В итоге половина переменных проёбывалась.
endif

OPENL2TP_MAKE_OPT = \
	INSTALL_DIR=$(TARGET_DIR) \
	CC="$(TARGET_CC)" \
	MAKE="$(MAKE)" \
	LD="$(TARGET_LD)" \
	STRIP=${TARGET_STRIP} \
	EXTRA_CFLAGS="${TARGET_CFLAGS} -I$(PPPD_DIR)/include/net/ -I$(PPPD_DIR)/include/ -I$(PPPD_DIR) -L$(STAGIN_DIR)/usr/lib -I$(OPENL2TP_DIR) -I$(OPENL2TP_DIR)/usl -I$(OPENL2TP_DIR)/cli -DSYS_LIBDIR=/etc/ppp/plugins " \
	EXTRA_LDFLAGS="${TARGET_LDFLAGS} -lcrypt -L$(TARGET_DIR)/lib" \
	KERNEL_SRCDIR="$(LINUX26_DIR)" \
	KERNEL_BLDDIR="$(LINUX26_DIR)" \
	READLINE_DIR="$(STAGING_DIR)/usr" \
	PPPD_SRCDIR="$(STAGING_DIR)/usr/include" \
	CFLAGS.optimize="$(TARGET_CFLAGS)" \
	CPPFLAGS-y="-I$(STAGING_DIR)/usr/include $(OPENL2TP_DLINK_EXTRA_CPPFLAGS)" \
	L2TP_FEATURE_LOCAL_CONF_FILE=y \
	L2TP_FEATURE_RPC_MANAGEMENT=n \
	CROSS_COMPILE="$(TARGET_CROSS)"

#	#PPPD_VERSION="$(PKG_PPPD_VERSION)" \
#	#PPPD_LIBDIR="$(STAGING_DIR)/usr/lib/pppd" \


ifeq ($(BR2_LINUX_KERNEL_3_10),y)
OPENL2TP_MAKE_OPT += CONFIG_KERNEL_3_10=y
endif

define OPENL2TP_BUILD_CMDS
	${MAKE1} $(OPENL2TP_MAKE_OPT) -C $(@D)
endef

define OPENL2TP_INSTALL_TARGET_CMDS
	install -pm0755 ${@D}/plugins/ppp_unix.so $(TARGET_DIR)/usr/lib/pppd/
	install -pm0755 ${@D}/openl2tpd $(TARGET_DIR)/usr/sbin/openl2tpd
	$(TARGET_STRIP)  $(TARGET_DIR)/usr/sbin/openl2tpd
	$(TARGET_STRIP)  $(TARGET_DIR)/usr/lib/pppd/ppp_unix.so
	install -pm0755  $(TOPDIR)/package/openl2tp/openl2tp.sh $(TARGET_DIR)/usr/openl2tp.sh
endef

define OPENL2TP_CLEAN_CMDS
	${MAKE1} $(OPENL2TP_MAKE_OPT) -C $(@D) clean
endef
$(eval $(call GENTARGETS))
