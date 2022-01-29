# Kernel-specific options for Realtek 9607C and 8198D

ifeq ($(BR2_rtk_luna),y)

# host speedup
define KERNEL_HOST_SPEEDUP_ENABLE
@$(call MESSAGE,"Enable host speedup");
@$(call KCONFIG_ENABLE_OPT,CONFIG_RTK_HOST_SPEEDUP, $(LINUX_CONFIG));
endef

ifeq ($(BR2_DLINK_TR069CLIENT_KERNEL_SPEEDUP),y)
LINUX_ADDONS += KERNEL_HOST_SPEEDUP_ENABLE
endif

endif # BR2_rtk_luna
