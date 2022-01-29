################################################################################
#
# openvpn
#
################################################################################

OPENVPN_VERSION = dsysinit
OPENVPN_SITE = $(DLINK_GIT_STORAGE)/openvpn
OPENVPN_DEPENDENCIES = openssl
OPENVPN_LICENSE = GPL-2.0-only WITH openvpn-openssl-exception WITH openvpn-lzo-exception AND Zlib AND OpenSSL
OPENVPN_LICENSE_FILES = COPYING COPYRIGHT.GPL
OPENVPN_CONF_OPT = \
	--enable-iproute2 \
	--with-crypto-library=openssl \
	$(if $(BR2_STATIC_LIBS),--disable-plugins)
OPENVPN_CONF_ENV = IFCONFIG=/sbin/ifconfig \
	NETSTAT=/bin/netstat \
	ROUTE=/sbin/route

ifeq ($(BR2_PACKAGE_OPENVPN_SMALL),y)
OPENVPN_CONF_OPT += \
	--enable-small \
	--disable-plugins \
	--disable-eurephia
endif
OPENVPN_AUTORECONF = YES

# BusyBox 1.21+ places the ip applet in the "correct" place
# but previous versions didn't.
ifeq ($(BR2_PACKAGE_IPROUTE2),y)
	OPENVPN_CONF_ENV += IPROUTE=/sbin/ip
else ifeq ($(BR2_BUSYBOX_VERSION_1_19_X)$(BR2_BUSYBOX_VERSION_1_20_X),y)
	OPENVPN_CONF_ENV += IPROUTE=/bin/ip
else
	OPENVPN_CONF_ENV += IPROUTE=/sbin/ip
endif

ifeq ($(BR2_PACKAGE_OPENVPN_LZO),y)
	OPENVPN_DEPENDENCIES += lzo
else
	OPENVPN_CONF_OPT += --disable-lzo
endif

define OPENVPN_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 $(@D)/src/openvpn/openvpn \
		$(TARGET_DIR)/usr/sbin/openvpn
endef

define OPENVPN_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/usr/sbin/openvpn
endef

OPENVPN_LDFLAGS = $(TARGET_LDFLAGS)

ifeq ($(BR2_DSYSINIT),y)
	OPENVPN_DEPENDENCIES += deuteron_framework
	OPENVPN_LDFLAGS += -ljansson -ld_service_notify
endif

OPENVPN_MAKE_OPT += CFLAGS="$(TARGET_CFLAGS) -std=gnu99" LDFLAGS="$(OPENVPN_LDFLAGS) -Wl,-rpath-link,$(STAGING_DIR)/lib -Wl,-rpath-link,$(STAGING_DIR)/usr/lib"

$(eval $(AUTOTARGETS))
