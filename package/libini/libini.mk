#############################################################
#
# LIBINI
#
#############################################################
LIBINI_VERSION = master
LIBINI_SITE = $(DLINK_GIT_STORAGE)/libini
LIBINI_LICENSE = BSD-3-Clause
LIBINI_LICENSE_FILES =

LIBINI_AUTORECONF = NO
LIBINI_INSTALL_STAGING = YES
LIBINI_INSTALL_TARGET = YES

LIBINI_CFLAGS  += $(TARGET_CFLAGS) -I. -Wall -s -O2 -I$(STAGING_DIR)/usr/include -fPIC
LIBINI_LDFLAGS += -L. -L$(STAGING_DIR)/lib
LIBINI_MAKE_ENV = $(TARGET_MAKE_ENV) CC="$(TARGET_CC)" LD="$(TARGET_LD)" CFLAGS="$(LIBINI_CFLAGS)" LDFLAGS="$(LIBINI_LDFLAGS)"


define LIBINI_CONFIGURE_CMDS
	@echo "No need to configure"
endef

define LIBINI_CLEAN_CMDS
	$(LIBINI_MAKE_ENV) $(MAKE) -C ${@D} clean
endef

define LIBINI_INSTALL_STAGING_CMDS
	$(LIBINI_MAKE_ENV) $(MAKE) -C ${@D} install INSTALLDIR=${STAGING_DIR}
	$(LIBINI_MAKE_ENV) $(MAKE) -C ${@D} install-headers INSTALLDIR=${STAGING_DIR}
endef

define LIBINI_INSTALL_TARGET_CMDS
	$(LIBINI_MAKE_ENV) $(MAKE) -C ${@D} install INSTALLDIR=${TARGET_DIR}
endef


$(eval $(call AUTOTARGETS,package,libini))
