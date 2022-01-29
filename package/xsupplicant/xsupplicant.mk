#############################################################
#
# zebra
#
#############################################################
#XSUPPLICANT_VERSION = 2.0.2
#XSUPPLICANT_SOURCE = -$(LIBUSB_VERSION).tar.bz2
XSUPPLICANT_VERSION =$(call qstrip,$(BR2_XSUPPLICANT_VERSION))
ifeq (${XSUPPLICANT_VERSION},)
XSUPPLICANT_VERSION = $(call qstrip,$(BR2_DEFAULT_BRANCH))
endif

XSUPPLICANT_SITE = $(DLINK_GIT_STORAGE)/xsupplicant
XSUPPLICANT_DEPENDENCIES = host-pkg-config
XSUPPLICANT_AUTORECONF = YES
#XSUPPLICANT_INSTALL_STAGING = YES
XSUPPLICANT_INSTALL_TARGET = YES
#XSUPPLICANT_CONF_OPT += enable_timerfd=no
ifeq (${BR2_PACKAGE_XSUPPLICANT},y)
TARGET_CFLAGS += -DSUPPORT_802_1X
endif

$(eval $(call AUTOTARGETS))
