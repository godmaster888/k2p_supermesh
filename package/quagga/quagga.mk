#############################################################
#
# quagga
#
#############################################################

QUAGGA_VERSION = $(call qstrip,$(BR2_QUAGGA_VERSION))
QUAGGA_LICENSE = GPL-2.0-or-later AND LGPL-2.0-or-later
QUAGGA_LICENSE_FILES = COPYING COPYING.LIB

ifeq ($(QUAGGA_VERSION),)
QUAGGA_VERSION = 1.1.1
endif

ifneq ($(QUAGGA_VERSION),1.1.1)
QUAGGA_SITE = $(DLINK_GIT_STORAGE)/quagga
QUAGGA_AUTORECONF = YES
QUAGGA_SUPPORTS_IN_SOURCE_BUILD = NO

# Эта версия не знает про:
# --enable-netlink
# --disable-opaque-lsa
# --disable-ospf-te
# --disable-pie
# --disable-ipv6
# Скорее всего, тарболл 1.1.1 тоже.
QUAGGA_CONF_OPT += --disable-nhrpd
else
QUAGGA_SITE = $(DLINK_STORAGE)
endif

QUAGGA_DEPENDENCIES = host-gawk
QUAGGA_CONF_OPT += --program-transform-name='' \
				  --sysconfdir=/tmp \
				  --enable-shared \
				  --disable-static \
				  --disable-doc \
				  --disable-watchquagga \
				  --disable-pimd \
				  --enable-netlink \
				  --disable-tcp-zebra \
				  --disable-opaque-lsa \
				  --enable-user=$(BR2_QUAGGA_USER) \
				  --enable-group=system \
				  --disable-rtadv \
				  --disable-vtysh \
				  --disable-pie

QUAGGA_CONF_ENV = LIBS="-ld_service_notify -ljansson -Wl,-rpath-link,$(STAGING_DIR)/lib -Wl,-rpath-link,$(STAGING_DIR)/usr/lib "
QUAGGA_DEPENDENCIES += deuteron_framework jansson

QUAGGA_CONF_OPT += $(if $(BR2_PACKAGE_QUAGGA_ZEBRA),--enable-zebra,--disable-zebra)
QUAGGA_CONF_OPT += $(if $(BR2_PACKAGE_QUAGGA_BGPD),--enable-bgpd,--disable-bgpd --disable-bgp-announce)
QUAGGA_CONF_OPT += $(if $(BR2_PACKAGE_QUAGGA_RIPD),--enable-ripd,--disable-ripd)
QUAGGA_CONF_OPT += $(if $(BR2_PACKAGE_QUAGGA_RIPNGD),--enable-ripngd,--disable-ripngd)
QUAGGA_CONF_OPT += $(if $(BR2_PACKAGE_QUAGGA_OSPFD),--enable-ospfd,--disable-ospfd --disable-ospfapi --disable-ospf-te --disable-ospf-ri)
QUAGGA_CONF_OPT += $(if $(BR2_PACKAGE_QUAGGA_OSPF6D),--enable-ospf6d,--disable-ospf6d)
QUAGGA_CONF_OPT += $(if $(BR2_PACKAGE_QUAGGA_ISISD),--enable-isisd,--disable-isisd)

ifneq ($(BR2_INET_IPV6),y)
QUAGGA_CONF_OPT+=--disable-ipv6
endif

ifeq ($(BR2_PACKAGE_QUAGGA_LDPD),y)
QUAGGA_CONF_OPT += --enable-mpls
QUAGGA_CONF_OPT += --enable-mpls-ip-encap-table=$(BR2_PACKAGE_QUAGGA_LDPD_MPLS_IP_ENCAP_TABLE)
QUAGGA_CONF_OPT += --enable-ldpd
else
QUAGGA_CONF_OPT += --disable-mpls
QUAGGA_CONF_OPT += --disable-ldpd
endif

$(eval $(call AUTOTARGETS))
