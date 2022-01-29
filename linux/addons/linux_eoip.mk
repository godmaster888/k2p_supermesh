# модуль настройки поддержки EoIP опций в профиле ядра.

#Процедура установки необходимых опций EoIP.
define EOIP_ENABLE
	@$(call MESSAGE,"Enabling kernel EoIP support")
	@$(call KCONFIG_ENABLE_OPT,CONFIG_NET_EOIP,$(LINUX_CONFIG))
endef

define EOIP_DISABLE
	@$(call MESSAGE,"Disabling kernel EoIP support")
	@$(call KCONFIG_DISABLE_OPT,CONFIG_NET_EOIP,$(LINUX_CONFIG))
endef

LINUX_ADDONS += $(if $(BR2_SUPPORT_EOIP),EOIP_ENABLE,EOIP_DISABLE)
