#############################################################
#
# usb_modeswitch
#
#############################################################

USB_MODESWITCH_VERSION = master
USB_MODESWITCH_SITE = ${DLINK_GIT_STORAGE}/usb_modeswitch
USB_MODESWITCH_LICENSE = GPL-2.0-or-later
USB_MODESWITCH_LICENSE_FILES =
USB_MODESWITCH_DEPENDENCIES = libusb-compat

define USB_MODESWITCH_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE1) -C $(@D) 
endef

define USB_MODESWITCH_INSTALL_TARGET_CMDS
	install -pm0755 ${@D}/usb_modeswitch $(TARGET_DIR)/usr/sbin/ 
	install -d $(TARGET_DIR)/etc/usb_modeswitch.d
	install --mode=644 -t $(TARGET_DIR)/etc/usb_modeswitch.d ${@D}/usb_modeswitch.d/*
endef

define USB_MODESWITCH_CLEAN_CMDS
	rm -f $(TARGET_DIR)/usr/sbin/usb_modeswitch
	rm -f $(TARGET_DIR)/etc/usb_modeswitch.setup
	rm -f $(TARGET_DIR)/usr/share/man/man1/usb_modeswitch.1
endef

$(eval $(call GENTARGETS))

