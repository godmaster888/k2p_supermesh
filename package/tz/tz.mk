#############################################################
#
# tz
#
#############################################################

TZ_SITE=$(DLINK_GIT_STORAGE)/tz
TZ_VERSION = master
TZ_LICENSE = BSD-3-Clause AND Unlicense
TZ_LICENSE_FILES = LICENSE
TZ_INSTALL_STAGING = YES
TZ_DEPENDENCIES = host-deuteron_apps

tz_script := $(shell pwd)/$(dir $(lastword $(MAKEFILE_LIST)))/gen4dm.py

define TZ_BUILD_CMDS
	cc="$(TARGET_CC)" CFLAGS=-Os $(MAKE) -C $(@D) zic tzdata.zi
	cd $(@D); $(tz_script) tzdata.zi > __tz.json
	cd $(@D); $(HOST_DIR)/usr/sbin/dmsdm msgpack  __tz.json __tz.data
endef

define TZ_INSTALL_STAGING_CMDS
	mkdir -p $(STAGING_DIR)/usr/include
	$(INSTALL) -m 0644 $(@D)/tzfile.h $(STAGING_DIR)/usr/include
endef

define TZ_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/etc
	$(INSTALL) $(@D)/__tz.data $(TARGET_DIR)/etc/tz.data
	mkdir -p $(TARGET_DIR)/usr/share/zoneinfo
	$(INSTALL) $(@D)/tzdata.zi $(TARGET_DIR)/usr/share/zoneinfo
	mkdir -p $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 0755 $(@D)/zic $(TARGET_DIR)/usr/sbin
endef

define TZ_CLEAN_CMDS
	rm -f $(TARGET_DIR)/etc/tz.data
	cd $(@D); rm -f __tz.json __tz.data
	$(MAKE) -C $(@D) clean
	rm -f $(@D)/version
	rm -f $(STAGING_DIR)/usr/include/tzfile.h
	rm -f $(TARGET_DIR)/usr/share/zoneinfo/tzdata.zi $(TARGET_DIR)/usr/sbin/zic
endef


$(eval $(call GENTARGETS))
