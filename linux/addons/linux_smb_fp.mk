# модуль включения samba fast path в профиле ядра.

define SMB_FP_ENABLE
@$(call MESSAGE,"Enabling kernel samba fast path feature")
@$(call KCONFIG_ENABLE_OPT,CONFIG_NETFILTER_FP_SMB,$(LINUX_CONFIG))
endef

define SMB_FP_DISABLE
@$(call MESSAGE,"Disabling kernel samba fast path feature")
@$(call KCONFIG_DISABLE_OPT,CONFIG_NETFILTER_FP_SMB,$(LINUX_CONFIG))
endef

LINUX_ADDONS += $(if $(BR2_PACKAGE_SAMBA_FASTPATH),SMB_FP_ENABLE,SMB_FP_DISABLE)
