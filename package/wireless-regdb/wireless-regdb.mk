################################################################################
#
# wireless-regdb
#
################################################################################

WIRELESS_REGDB_VERSION = master
WIRELESS_REGDB_SITE = $(DLINK_GIT_STORAGE)/wireless_regdb
WIRELESS_REGDB_LICENSE = ISC
WIRELESS_REGDB_LICENSE_FILES = LICENSE


ifeq ($(BR2_PACKAGE_WIRELESS_REGDB_INSTALL_CERT),y)
define WIRELESS_REGDB_INSTALL_CERT
	$(INSTALL) -m 644 -D -T $(@D)/builder.key.pub.pem \
		$(TARGET_DIR)/etc/wireless-regdb/pubkeys/builder.key.pub.pem
endef

define WIRELESS_REGDB_UNINSTALL_CERT
	rm -f $(TARGET_DIR)/etc/wireless-regdb/pubkeys/builder.key.pub.pem
endef
endif

define WIRELESS_REGDB_INSTALL_TARGET_CMDS
	$(INSTALL) -m 644 -D -T $(@D)/regulatory.bin \
		$(TARGET_DIR)/usr/lib/crda/regulatory.bin

	$(WIRELESS_REGDB_INSTALL_CERT)
endef

define WIRELESS_REGDB_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/usr/lib/crda/regulatory.bin

	$(WIRELESS_REGDB_UNINSTALL_CERT)
endef

$(eval $(call GENTARGETS))
