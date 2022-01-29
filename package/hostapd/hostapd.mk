#############################################################
#
# hostapd
#
#############################################################

HOSTAPD_VERSION = w1
ifeq ($(BR2_PACKAGE_QCA_WIFI),y)
HOSTAPD_VERSION = qca-10.4
endif
ifeq ($(BR2_bcm),y)
HOSTAPD_LIBS += -lnl
endif

HOSTAPD_SITE = $(DLINK_GIT_STORAGE)/hostapd
HOSTAPD_DEPENDENCIES = libnl-tiny
# headers from qca_wifi
ifneq ($(BR2_bcm),)
HOSTAPD_DEPENDENCIES += libnl
endif
HOSTAPD_DEPENDENCIES += $(if $(BR2_PACKAGE_QCA_WIFI),qca_wifi)

HOSTAPD_SUBDIR = hostapd
HOSTAPD_CONFIG = $(HOSTAPD_DIR)/$(HOSTAPD_SUBDIR)/.config
HOSTAPD_LICENSE = BSD-3-Clause
HOSTAPD_LICENSE_FILES = README COPYING
HOSTAPD_CONFIG_SET =

HOSTAPD_CFLAGS += $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnl-tiny
HOSTAPD_LDFLAGS=$(TARGET_LDFLAGS)

ifeq ($(BR2_DSYSINIT),y)
HOSTAPD_DEPENDENCIES += deuteron_framework
HOSTAPD_LIBS += -ld_service_notify -Wl,-rpath-link $(STAGING_DIR)/usr/lib
endif

HOSTAPD_CONFIG_ENABLE = \
	CONFIG_FULL_DYNAMIC_VLAN \
	CONFIG_HS20 \
	CONFIG_IEEE80211AC \
	CONFIG_IEEE80211N \
	CONFIG_IEEE80211R \
	CONFIG_INTERNAL_LIBTOMMATH \
	CONFIG_INTERWORKING
#	 \
#	CONFIG_LIBNL32
# \
#	CONFIG_VLAN_NETLINK

WPASUPP_SUBDIR = wpa_supplicant
WPASUPP_CONFIG = $(HOSTAPD_DIR)/$(WPASUPP_SUBDIR)/.config
WPASUPP_CONFIG_DISABLE =

WPASUPP_CONFIG_SET =

# тупо взял от hostapd, хотел дописать
# ещё и CONFIG_IEEE80211R_AP, но там тараканы с
# CONFIG_ETH_P_OUI, который только в hostapd..
# короче, не стал дописывать :)
# да хз что это
WPASUPP_CONFIG_ENABLE = \
	CONFIG_HS20 \
	CONFIG_IEEE80211AC \
	CONFIG_IEEE80211N \
	CONFIG_IEEE80211R \
	CONFIG_INTERNAL_LIBTOMMATH \
	CONFIG_INTERWORKING
#	CONFIG_AP ?? или в опции?

# а вот это мои довески
WPASUPP_CONFIG_ENABLE += \
	CONFIG_ELOOP_EPOLL

WPASUPP_CONFIG_DISABLE = \
	CONFIG_DRIVER_WIRED \
	CONFIG_PKCS12 \
	CONFIG_SMARTCARD

WPASUPP_CONFIG_EDITS =

ifeq ($(BR2_PACKAGE_HOSTAPD),y)
$(warning #18758: TODO: hostapd is not being build with VLAN_NETLINK support now)
endif

#18758: для libnl-tiny
HOSTAPD_CONFIG_DISABLE += CONFIG_LIBNL32
HOSTAPD_CONFIG_SET += CONFIG_LIBNL_TINY CONFIG_LIBNL20
WPASUPP_CONFIG_SET += CONFIG_LIBNL_TINY CONFIG_LIBNL20

ifneq ($(BR2_PACKAGE_QCA_WIFI),)
HOSTAPD_CONFIG_SET += CONFIG_DRIVER_ATHEROS ATH_GCM_SUPPORT
# qca-10.4: driver atheros
# для athr_get_freq () требует
HOSTAPD_CONFIG_ENABLE += CONFIG_WNM
# который для линковки с atheros_set_wps_ie() хочет
HOSTAPD_CONFIG_ENABLE += CONFIG_WPS

#WPASUPP_CONFIG_SET += CONFIG_DRIVER_ATHEROS ATH_GCM_SUPPORT
# qca-10.4 требует драйвер "athr" для клиентского коннекта
WPASUPP_CONFIG_SET += CONFIG_DRIVER_ATHR ATH_GCM_SUPPORT
# и wpa_s->ap_iface
WPASUPP_CONFIG_ENABLE += CONFIG_AP

# без nl80211 не собирается (undefined reference)
#HOSTAPD_CONFIG_DISABLE += CONFIG_DRIVER_NL80211
#WPASUPP_CONFIG_DISABLE += CONFIG_DRIVER_NL80211
endif

#18758, Qualcomm vendor extensions
# TODO: is qca_wifi affected?
HOSTAPD_CONFIG_ENABLE += $(if $(BR2_qca),CONFIG_DRIVER_NL80211_QCA)
WPASUPP_CONFIG_ENABLE += $(if $(BR2_qca),CONFIG_DRIVER_NL80211_QCA)

HOSTAPD_CFLAGS += $(if $(BR2_TOOLCHAIN_USES_MUSL)$(BR2_TOOLCHAIN_USES_UCLIBC),-D_GNU_SOURCE)
#HOSTAPD_CFLAGS += -DMUSL_LIBC

# libnl-3 needs -lm (for rint) and -lpthread if linking statically
# And library order matters hence stick -lnl-3 first since it's appended
# in the hostapd Makefiles as in LIBS+=-lnl-3 ... thus failing
HOSTAPD_LIBS += $(if $(BR2_STATIC_LIBS),-lnl-3 -lm -lpthread)

# Try to use openssl if it's already available
ifeq ($(BR2_PACKAGE_OPENSSL),y)
HOSTAPD_DEPENDENCIES += openssl
HOSTAPD_LIBS += $(if $(BR2_STATIC_LIBS),-lcrypto -lz)
HOSTAPD_CONFIG_EDITS += 's/\#\(CONFIG_TLS=openssl\)/\1/'
WPASUPP_CONFIG_EDITS += 's/\#\(CONFIG_TLS=openssl\)/\1/'
else
# точно не тестировалось для wpa_supplicant,
# для hostapd - не помню.
HOSTAPD_CONFIG_DISABLE += CONFIG_EAP_PWD
HOSTAPD_CONFIG_EDITS += 's/\#\(CONFIG_TLS=\).*/\1internal/'
  ifeq ($(BR2_PACKAGE_HOSTAPD_WPASUPP),y)
    $(warning wpa_supplicant wan not tested or even built with internal TLS!)
    WPASUPP_CONFIG_DISABLE += CONFIG_EAP_PWD
    WPASUPP_CONFIG_EDITS += 's/\#\(CONFIG_TLS=\).*/\1internal/'
  endif
endif

HOSTAPD_CONFIG_ENABLE += $(if $(BR2_PACKAGE_HOSTAPD_ACS),CONFIG_ACS)
# без nl80211 не собирается ACS для wpa_supplicant
WPASUPP_CONFIG_ENABLE += $(if $(BR2_PACKAGE_HOSTAPD_ACS),CONFIG_ACS)
WPASUPP_CONFIG_ENABLE += $(if $(BR2_PACKAGE_HOSTAPD_WPASUPP_MESH),CONFIG_MESH CONFIG_IEEE80211W)
WPASUPP_CONFIG_ENABLE += $(if $(BR2_PACKAGE_HOSTAPD_WPASUPP_P2P),CONFIG_P2P)

ifeq ($(BR2_PACKAGE_HOSTAPD_EAP),y)
HOSTAPD_CONFIG_ENABLE += \
	CONFIG_EAP \
	CONFIG_RADIUS_SERVER

# Enable both TLS v1.1 (CONFIG_TLSV11) and v1.2 (CONFIG_TLSV12)
HOSTAPD_CONFIG_ENABLE += CONFIG_TLSV1
# в wpa_supplicant всё повключено по дефолту
else
HOSTAPD_CONFIG_DISABLE += CONFIG_EAP
HOSTAPD_CONFIG_ENABLE += \
	CONFIG_NO_ACCOUNTING \
	CONFIG_NO_RADIUS

WPASUPP_CONFIG_DISABLE += CONFIG_IEEE8021X_EAPOL
# и всех кодлой закомментарить
WPASUPP_CONFIG_EDITS += 's/^\(CONFIG_EAP_\).*/\#\1/'
endif

HOSTAPD_CONFIG_ENABLE += $(if $(BR2_PACKAGE_HOSTAPD_WPS),CONFIG_WPS)
WPASUPP_CONFIG_ENABLE += $(if $(BR2_PACKAGE_HOSTAPD_WPS),CONFIG_WPS)

define HOSTAPD_CONFIGURE_CMDS
	cp $(@D)/$(HOSTAPD_SUBDIR)/defconfig $(HOSTAPD_CONFIG)
	sed -i $(patsubst %,-e 's/^#\(%=\)/\1/',$(HOSTAPD_CONFIG_ENABLE)) \
		$(patsubst %,-e 's/^\(%=\)/#\1/',$(HOSTAPD_CONFIG_DISABLE)) \
		$(patsubst %,-e '1i%=y',$(HOSTAPD_CONFIG_SET)) \
		$(patsubst %,-e %,$(HOSTAPD_CONFIG_EDITS)) \
		$(HOSTAPD_CONFIG)
	cp $(@D)/$(WPASUPP_SUBDIR)/defconfig $(WPASUPP_CONFIG)
	sed -i $(patsubst %,-e 's/^#\(%=\)/\1/',$(WPASUPP_CONFIG_ENABLE)) \
		$(patsubst %,-e 's/^\(%=\)/#\1/',$(WPASUPP_CONFIG_DISABLE)) \
		$(patsubst %,-e '1i%=y',$(WPASUPP_CONFIG_SET)) \
		$(patsubst %,-e %,$(WPASUPP_CONFIG_EDITS)) \
		$(WPASUPP_CONFIG)
endef

# объектники пересекаются у hostapd и wpa_supplicant.
# или переделывать сборку на разные сборочные диры
# или вот так - стирать их перед сборкой подпакета.
define hspd_cln_objs
	$(if $(BR2_PACKAGE_HOSTAPD_WPASUPP), find $(@D)/ -name '*\.[oa]' -exec rm {} \;)
endef

define HOSTAPD_BUILD_CMDS
	$(call hspd_cln_objs)
	$(TARGET_MAKE_ENV) CFLAGS="$(HOSTAPD_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" LIBS="$(HOSTAPD_LIBS)" \
		$(MAKE) CC="$(TARGET_CC)" -C $(@D)/$(HOSTAPD_SUBDIR)
	$(call hspd_cln_objs)
	$(TARGET_MAKE_ENV) CFLAGS="$(HOSTAPD_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" LIBS="$(HOSTAPD_LIBS)" \
		$(MAKE) CC="$(TARGET_CC)" -C $(@D)/$(WPASUPP_SUBDIR)
endef

ifeq ($(BR2_PACKAGE_HOSTAPD_INSTALL_CLI),y)
define hspd_inst_cli
	$(INSTALL) -m 0755 -D $(@D)/$(HOSTAPD_SUBDIR)/hostapd_cli \
		$(TARGET_DIR)/usr/bin/hostapd_cli
endef
else
define hspd_inst_cli
	@echo "hostapd_cli is skipped to install"
endef
endif

ifeq ($(BR2_PACKAGE_HOSTAPD_INSTALL_CONF),y)
define hspd_inst_conf
	$(INSTALL) -m 0644 -D $(@D)/$(HOSTAPD_SUBDIR)/hostapd.conf \
		$(TARGET_DIR)/etc/hostapd.conf
endef
else
define hspd_inst_conf
	@echo "hostapd.conf is skipped to install"
endef
endif

ifeq ($(BR2_PACKAGE_HOSTAPD_WPASUPP),y)
define wpasupp_inst
	$(INSTALL) -m 0755 -D $(@D)/$(WPASUPP_SUBDIR)/wpa_supplicant \
		$(TARGET_DIR)/usr/sbin/wpa_supplicant
endef
else
define wpasupp_inst
	@echo "wpa_supplicant is skipped to install"
endef
endif

define HOSTAPD_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/$(HOSTAPD_SUBDIR)/hostapd \
		$(TARGET_DIR)/usr/sbin/hostapd
	$(call hspd_inst_cli)
	$(call hspd_inst_conf)
	$(call wpasupp_inst)
endef

$(eval $(call GENTARGETS))
