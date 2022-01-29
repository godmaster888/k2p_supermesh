# Модуль настройки туннельных опций в профиле ядра.

# Процедура включения туннельных опций.
define TUN_ENABLE
@$(call MESSAGE,"Enabling kernel tun feature")
@$(call KCONFIG_ENABLE_OPT ,CONFIG_TUN, $(LINUX_CONFIG))
endef

LINUX_ADDONS += $(if $(BR2_PACKAGE_COOVACHILLI)$(BR2_PACKAGE_OPENVPN),TUN_ENABLE,)
