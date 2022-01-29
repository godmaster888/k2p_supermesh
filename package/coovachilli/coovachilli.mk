################################################################################
#
# coovachilli
#
################################################################################
COOVACHILLI_VERSION = master
COOVACHILLI_SITE = $(DLINK_GIT_STORAGE)/coovachilli
COOVACHILLI_SITE_METHOD = git
COOVACHILLI_LICENSE = GPL-2.0-or-later
COOVACHILLI_LICENSE_FILES = COPYING
COOVACHILLI_INSTALL_STAGING = NO
COOVACHILLI_INSTALL_TARGET = YES
COOVACHILLI_DEPENDENCIES += host-gengetopt

# Линковка с d_service_notify и вытекающие из-за этого хаки CONF_ENV
COOVACHILLI_CONF_ENV += CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/include -I$(STAGING_DIR)/usr/include -g"
COOVACHILLI_CONF_ENV += LIBS="-ld_service_notify -ld_msgpack -lz -ljansson"

# Перехватывать DNS-запросы
COOVACHILLI_CONF_OPT += --enable-redirdnsreq

# Пока вообще не понятно, как эту опцию использовать
COOVACHILLI_CONF_OPT += --disable-ieee8021q
# Поддержка RADIUS Change of Authorization - пока не нужно
COOVACHILLI_CONF_OPT += --disable-coa
# Поддержка RADIUS proxy - не нужно
COOVACHILLI_CONF_OPT += --disable-radproxy
# Встроенный web-интерфейс, не нужен
COOVACHILLI_CONF_OPT += --disable-uamuiport
# У нас всегда используется dhcp-сервер, опция не нужна
COOVACHILLI_CONF_OPT += --disable-uamanyip
# Потенциально полезная штука (позволит бриджевать chilli интерфейс), но пока не поддерживается у нас
COOVACHILLI_CONF_OPT += --disable-tap


# На будущее, если решим использовать ядерные модули
ifeq ($(BR2_PACKAGE_COOVACHILLI_NETFILTER),y)
COOVACHILLI_CONF_OPT += --with-nfcoova
# Нужно явно засунуть их в MAKE_ENV, чтобы опции долетели до вложенного src/linux/Makefile
COOVACHILLI_MAKE_ENV += CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/include" LDFLAGS="-L$(STAGING_DIR)/usr/lib" KERNEL_DIR=$(LINUX26_DIR)
endif

define COOVACHILLI_BOOTSTRAP
	cd $(@D) && ./bootstrap
endef

COOVACHILLI_PRE_CONFIGURE_HOOKS += COOVACHILLI_BOOTSTRAP

define COOVACHILLI_REMOVE_ETC
	rm -rfv $(TARGET_DIR)/etc/chilli
endef

COOVACHILLI_POST_INSTALL_TARGET_HOOKS += COOVACHILLI_REMOVE_ETC

# Иногда происходит гонка и патч 'cmdline.patch' накладывается несколько раз
COOVACHILLI_MAKE = $(MAKE1)

$(eval $(call AUTOTARGETS))
