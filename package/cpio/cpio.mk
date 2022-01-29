################################################################################
#
# cpio
#
################################################################################

CPIO_VERSION = 2.12
CPIO_SOURCE = cpio-$(CPIO_VERSION).tar.gz
# CPIO_SITE = $(BR2_GNU_MIRROR)/cpio
CPIO_SITE = $(DLINK_STORAGE)
CPIO_CONF_OPT = --bindir=/bin
CPIO_LICENSE = GPL-3.0+
CPIO_LICENSE_FILES = COPYING

# cpio uses argp.h which is not provided by uclibc or musl by default.
# Use the argp-standalone package to provide this.
ifeq ($(BR2_PACKAGE_ARGP_STANDALONE),y)
CPIO_DEPENDENCIES += argp-standalone
endif

$(eval $(call AUTOTARGETS,host))
