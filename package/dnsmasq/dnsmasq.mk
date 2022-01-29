#############################################################
#
# dnsmasq
#
#############################################################

DNSMASQ_VERSION = dsysinit

DNSMASQ_SITE = $(DLINK_GIT_STORAGE)/dnsmasq

DNSMASQ_LICENSE = GPL-2.0-only OR GPL-3.0-only
DNSMASQ_LICENSE_FILES = COPYING COPYING-v3

DNSMASQ_MAKE_ENV = CC="$(TARGET_CC)"

ifeq ($(BR2_DSYSINIT),y)
	DNSMASQ_DEPENDENCIES += jansson deuteron_framework
	DNSMASQ_LIBS += -ljansson -ld_service_notify
endif

DNSMASQ_MAKE_OPT = COPTS="$(DNSMASQ_COPTS)" PREFIX=/usr \
		   CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS) $(DNSMASQ_LIBS) -Wl,-rpath-link,$(STAGING_DIR)/lib -Wl,-rpath-link,$(STAGING_DIR)/usr/lib" \
		   DESTDIR=$(TARGET_DIR)

DNSMASQ_COPTS += -DNO_INOTIFY

ifeq ($(BR2_PACKAGE_DNSMASQ_CONNTRACK),y)
	DNSMASQ_COPTS += -DHAVE_CONNTRACK
	DNSMASQ_MAKE_OPT += LDFLAGS+="-lmnl"
endif

ifneq ($(BR2_INET_IPV6),y)
	DNSMASQ_COPTS += -DNO_IPV6
endif

ifneq ($(BR2_PACKAGE_DNSMASQ_DHCP),y)
	DNSMASQ_COPTS += -DNO_DHCP
endif

ifneq ($(BR2_PACKAGE_DNSMASQ_TFTP),y)
	DNSMASQ_COPTS += -DNO_TFTP
endif

ifeq ($(BR2_PACKAGE_DNSMASQ_IDN),y)
	DNSMASQ_MAKE_OPT += all-i18n
	DNSMASQ_DEPENDENCIES += libidn libintl
	DNSMASQ_MAKE_ENV += LDFLAGS+="-lintl"
endif

ifneq ($(BR2_LARGEFILE),y)
	DNSMASQ_COPTS += -DNO_LARGEFILE
endif

ifeq ($(BR2_SUPPORT_TWIN_IP),y)
	DNSMASQ_COPTS += -DSUPPORT_SUPER_DMZ
endif

ifeq ($(BR2_SUPPORT_SAFEDNS),y)
	DNSMASQ_MAKE_ENV += SAFEDNS=y
	DNSMASQ_COPTS += -DSUPPORT_SAFEDNS
endif

ifeq ($(BR2_PACKAGE_DBUS),y)
	DNSMASQ_DEPENDENCIES += host-pkg-config dbus
endif

ifeq ($(BR2_PACKAGE_DBUS),y)
define DNSMASQ_ENABLE_DBUS
	$(SED) 's^.*#define HAVE_DBUS.*^#define HAVE_DBUS^' \
		$(DNSMASQ_DIR)/src/config.h
	$(SED) 's^PKG_CONFIG = pkg-config^PKG_CONFIG = $(PKG_CONFIG_HOST_BINARY)^' \
		$(DNSMASQ_DIR)/Makefile
	$(SED) 's^--cflags dbus-1^--cflags dbus-1 \| sed s\\\#-I/\\\#-I$(STAGING_DIR)/\\\#g^' \
		$(DNSMASQ_DIR)/Makefile
endef
else
define DNSMASQ_ENABLE_DBUS
	$(SED) 's^.*#define HAVE_DBUS.*^/* #define HAVE_DBUS */^' \
		$(DNSMASQ_DIR)/src/config.h
endef
endif

ifeq ($(BR2_PACKAGE_DNSMASQ_BIND_DNS_WAN_IFACE),y)
	DNSMASQ_COPTS += -DBIND_DNS_WAN_IFACE
endif

define DNSMASQ_BUILD_CMDS
	$(DNSMASQ_ENABLE_DBUS)
	$(DNSMASQ_MAKE_ENV) $(MAKE) -C $(@D) $(DNSMASQ_MAKE_OPT)
endef

define DNSMASQ_INSTALL_TARGET_CMDS
	$(DNSMASQ_MAKE_ENV) $(MAKE) -C $(@D) $(DNSMASQ_MAKE_OPT) install
endef

define DNSMASQ_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define DNSMASQ_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/usr/sbin/dnsmasq
	rm -f $(TARGET_DIR)/usr/share/man/man8/dnsmasq.8
endef

$(eval $(call GENTARGETS))
