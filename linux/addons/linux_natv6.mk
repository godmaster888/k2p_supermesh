
# модуль настройки natv6 опций в профиле ядра.

# возможно вынести в общий код
define KCONFIG_DISABLE_OPT_IF_NOT_ENABLED
       grep -qw "$(1)" $(2) || echo "# $(1) is not set" >> $(2)
endef

#Процедура установки необходимых опций natv6.
define NATV6_ENABLE
@$(call MESSAGE,"Enabling kernel natv6 feature")
@$(call KCONFIG_ENABLE_OPT,CONFIG_NF_NAT_IPV6,$(LINUX_CONFIG))
@$(call KCONFIG_ENABLE_OPT,CONFIG_IP6_NF_TARGET_MASQUERADE,$(LINUX_CONFIG))
@$(call KCONFIG_ENABLE_OPT,CONFIG_NF_NAT_MASQUERADE_IPV6,$(LINUX_CONFIG))
@$(call KCONFIG_DISABLE_OPT_IF_NOT_ENABLED,CONFIG_IP6_NF_TARGET_NPT,$(LINUX_CONFIG))
endef

LINUX_ADDONS += $(if $(BR2_SUPPORT_NATV6),NATV6_ENABLE,)
