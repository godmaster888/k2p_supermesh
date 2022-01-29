#############################################################
#
# accel-pptp
#
#############################################################

ACCEL_PPTP_VERSION = $(call qstrip,$(BR2_ACCEL_PPTP_VERSION))
ifeq (${UDHCP_VERSION},)
ACCEL_PPTP_VERSION = $(call qstrip,$(BR2_ACCEL_PPTP_VERSION))
endif
ACCEL_PPTP_SITE:=$(DLINK_GIT_STORAGE)/accel-pptp
ACCEL_PPTP_LICENSE = GPL-2.0-or-later
ACCEL_PPTP_LICENSE_FILES = COPYING

ifeq ($(ACCEL_PPTP_VERSION), 0.8.5.1)

define ACCEL_PPTP_CONFIGURE_CMDS
	( cd $(@D); ./configure --host=${BR2_TOOLCHAIN_EXTERNAL_PATH} CC="$(TARGET_CC)" KDIR=${LINUX26_DIR} PPPDVER="2.4.5" DESTDIR=${TARGET_DIR}/usr libdir="/usr/lib/pppd" )
endef

define ACCEL_PPTP_CMDS
	$(MAKE) CC="$(TARGET_CC)" COPTS="-I${@D}/include $(TARGET_CFLAGS) -I${LINUX26_DIR}/include" \
		-C $(@D) 
endef

$(eval $(call AUTOTARGETS))
endif

ifeq ($(ACCEL_PPTP_VERSION), ralink_4.0.1.0)

ACCEL_PPTP_MAKE_OPTS = \
	CFLAGS="$(TARGET_CFLAGS)  -DHAVE_CONFIG_H -DSBINDIR='\"$(TARGET_DIR)/sbin/\"' -I$(LINUX26_DIR)/include -DEMBED  -Dlinux -D__linux__ -Dunix "\
	CC="$(TARGET_CC)" \
	LD="$(TARGET_CC)" \
	PREFIX="$(TARGET_DIR)" \
	LDFLAGS="$(TARGET_LDFLAGS) -L$(TARGET_DIR)/lib/" \
	CROSS_COMPILE="$(TARGET_CROSS)" 

define ACCEL_PPTP_BUILD_CMDS
	$(MAKE) $(ACCEL_PPTP_MAKE_OPTS) -C $(@D) all
endef

$(eval $(call GENTARGETS))

endif
