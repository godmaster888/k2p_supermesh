# модуль настройки поддержки timerfd опций в профиле ядра.

#Процедура установки необходимых опций timerfd.
define TIMERFD_ENABLE
	@$(call MESSAGE,"Enabling kernel timerfd feature")
	@$(call KCONFIG_ENABLE_OPT ,CONFIG_TIMERFD, $(LINUX_CONFIG))
endef

# Включаем всегда, так как он почти ничего не стоит, но может быть полезен
# многим пакетам.
LINUX_ADDONS += TIMERFD_ENABLE
