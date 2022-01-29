SOFTFLOWD_VERSION = master
SOFTFLOWD_SITE = $(DLINK_GIT_STORAGE)/softflowd
SOFTFLOWD_LICENSE = BSD-2-Clause AND BSD-3-Clause AND ISC
SOFTFLOWD_LICENSE_FILES = LICENSE
SOFTFLOWD_DEPENDENCIES = libpcap

SOFTFLOWD_CONF_OPT += --with-chrootdir=/var/run/softflowd

define SOFTFLOWD_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/softflowd $(TARGET_DIR)/usr/sbin/softflowd
	$(TARGET_STRIP) $(TARGET_DIR)/usr/sbin/softflowd
endef

$(eval $(call AUTOTARGETS))
