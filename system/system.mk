ifeq ($(BR2_USB_SUPPORT),y)
BR2_ROOTFS_STATIC_DEVICE_TABLE += system/device_table_dev_append_usb.txt
endif

ifeq ($(BR2_PACKAGE_LIBVOIP_MANAGER),y)
BR2_ROOTFS_STATIC_DEVICE_TABLE += system/device_table_dev_append_voip.txt
endif

ifeq ($(BR2_SUPPORT_ADSL),y)
BR2_ROOTFS_STATIC_DEVICE_TABLE += system/device_table_dev_append_adsl.txt
endif

ifeq ($(BR2_SUPPORT_VDSL),y)
BR2_ROOTFS_STATIC_DEVICE_TABLE += system/device_table_dev_append_vdsl.txt
endif

ifeq ($(BR2_rtk_luna),y)
BR2_ROOTFS_STATIC_DEVICE_TABLE += system/device_table_dev_append_smux.txt
endif
