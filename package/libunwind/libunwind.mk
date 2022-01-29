################################################################################
#
# libunwind
#
################################################################################

LIBUNWIND_VERSION = 1.2.1
LIBUNWIND_SOURCE = libunwind-$(LIBUNWIND_VERSION).tar.gz
LIBUNWIND_SITE = $(DLINK_STORAGE)
LIBUNWIND_INSTALL_STAGING = YES
LIBUNWIND_LICENSE_FILES = COPYING
LIBUNWIND_LICENSE = MIT
LIBUNWIND_AUTORECONF = YES

LIBUNWIND_CONF_OPT += --disable-tests
LIBUNWIND_CONF_OPT += --prefix=/usr

ifeq ($(BR2_PACKAGE_LIBATOMIC_OPS),y)
LIBUNWIND_DEPENDENCIES = libatomic_ops
endif

ifeq ($(BR2_PACKAGE_LIBUNWIND),y)
TARGET_CFLAGS += -funwind-tables -rdynamic
endif

$(eval $(call AUTOTARGETS))
$(eval $(call AUTOTARGETS,host))
