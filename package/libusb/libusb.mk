#############################################################
#
# libusb
#
#############################################################
LIBUSB_VERSION = 1.0.8
LIBUSB_SITE = ${DLINK_GIT_STORAGE}/libusb
LIBUSB_LICENSE = LGPL-2.1-or-later
LIBUSB_LICENSE_FILES = COPYING
LIBUSB_DEPENDENCIES = host-pkg-config
LIBUSB_INSTALL_STAGING = YES
LIBUSB_INSTALL_TARGET = YES
LIBUSB_CONF_OPT += enable_timerfd=no
$(eval $(call AUTOTARGETS))
