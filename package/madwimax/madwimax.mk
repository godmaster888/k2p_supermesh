#############################################################
#
# madwimax
#
#############################################################

MADWIMAX_VERSION = 0.1.1
MADWIMAX_SITE = ${DLINK_GIT_STORAGE}/madwimax
MADWIMAX_LICENSE = GPL-2.0-or-later
MADWIMAX_LICENSE_FILES = COPYING
MADWIMAX_DEPENDENCIES = libusb
MADWIMAX_CONF_OPT += --without-man-pages --program-transform-name=madwimax
ifeq (${BR2_PACKAGE_MADWIMAX},y)
TARGET_CFLAGS += -DSUPPORT_WIMAX
endif

$(eval $(call AUTOTARGETS))
