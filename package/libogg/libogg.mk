#############################################################
#
# libogg
#
#############################################################
LIBOGG_VERSION = master
LIBOGG_SITE = $(DLINK_GIT_STORAGE)/libogg
LIBOGG_LICENSE = BSD-3-Clause
LIBOGG_LICENSE_FILES = COPYING
LIBOGG_INSTALL_STAGING = YES
LIBOGG_INSTALL_TARGET = YES
LIBOGG_AUTORECONF = YES

LIBOGG_CONF_ENV = CFLAGS="-I$(STAGING_DIR)/include"

$(eval $(call AUTOTARGETS))
