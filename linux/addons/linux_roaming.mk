
# модуль настройки roaming опций в профиле ядра.

#Процедура установки необходимых опций roaming.
define ROAMING_ENABLE
@$(call MESSAGE,"Enabling kernel roaming feature")
@$(call KCONFIG_ENABLE_OPT ,CONFIG_SUPPORT_ROAMING, $(LINUX_CONFIG))
endef

#Процедура сброса опций roaming.
define ROAMING_DISABLE
@$(call MESSAGE,"Disabling kernel roaming feature")
@$(call KCONFIG_DISABLE_OPT,CONFIG_SUPPORT_ROAMING, $(LINUX_CONFIG))
endef

LINUX_ADDONS += $(if $(BR2_DEUTERON_APPS_ROAMD),ROAMING_ENABLE,ROAMING_DISABLE)
