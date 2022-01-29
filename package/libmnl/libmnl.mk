################################################################################
#
# libmnl
#
################################################################################

LIBMNL_VERSION = master
LIBMNL_SITE_METHOD = git
LIBMNL_SITE = $(DLINK_GIT_STORAGE)/libmnl
LIBMNL_INSTALL_STAGING = YES
LIBMNL_AUTORECONF = YES
HOST_LIBMNL_AUTORECONF = YES
LIBMNL_LICENSE = LGPL-2.1-or-later
LIBMNL_LICENSE_FILES = COPYING

LIBMNL_CONF_OPT += --prefix=/usr

$(eval $(call AUTOTARGETS))
$(eval $(call AUTOTARGETS,host))
