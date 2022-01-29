#############################################################
#
# USBTEST
#
#############################################################

USBTEST_VERSION = master
USBTEST_SITE = $(DLINK_GIT_STORAGE)/usbtest
USBTEST_LICENSE = GPL-2.0-only AND LGPL-2.0-only
USBTEST_LICENSE_FILES = COPYING
USBTEST_INSTALL_STAGING = YES

$(eval $(call CMAKETARGETS))
