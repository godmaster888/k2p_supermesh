#############################################################
#
# pure-ftpd
#
#############################################################
PURE_FTPD_SITE = $(DLINK_GIT_STORAGE)/pure-ftpd
PURE_FTPD_VERSION = master
PURE_FTPD_LICENSE = ISC AND BSD-3-Clause
PURE_FTPD_LICENSE_FILES = COPYING
PURE_FTPD_CONF_OPT = --with-minimal --sysconfdir=/tmp
PURE_FTPD_DEPENDENCIES = deuteron_framework
PURE_FTPD_CONF_ENV = LDFLAGS="$(TARGET_LDFLAGS) -ljansson -ld_service_notify -Wl,-rpath-link,$(STAGING_DIR)/lib -Wl,-rpath-link,$(STAGING_DIR)/usr/lib"

define PURE_FTPD_INSTALL_TARGET_CMDS
	install -pm0755 $(@D)/src/pure-ftpd $(TARGET_DIR)/usr/sbin
	$(TARGET_STRIP) $(TARGET_DIR)/usr/sbin/pure-ftpd
endef

$(eval $(call AUTOTARGETS))
