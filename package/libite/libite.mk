################################################################################
#
# libite
#
################################################################################

LIBITE_VERSION = 1.8.2
LIBITE_SOURCE = libite-$(LIBITE_VERSION).tar.xz
LIBITE_SITE = $(DLINK_STORAGE)
LIBITE_LICENSE = MIT AND X11 AND ISC AND BSD-2-Clause
LIBITE_LICENSE_FILES = README.md LICENSE chomp.c pidfile.c
LIBITE_INSTALL_STAGING = YES
LIBITE_AUTORECONF = YES

$(eval $(call AUTOTARGETS))
