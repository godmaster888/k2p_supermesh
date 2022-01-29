#############################################################
#
# zebra
#
#############################################################
ZEBRA_VERSION = 0.95a
ZEBRA_SITE:=$(DLINK_GIT_STORAGE)/zebra
ZEBRA_LICENSE = GPL-2.0-or-later
ZEBRA_LICENSE_FILES = COPYING

ZEBRA_CONF_OPT = --prefix=/usr --sysconfdir=/tmp --enable-group=root

# zebra password
ifneq ($(BR2_QUAGGA_ZEBRA_PASSWORD),)
  ZEBRA_CONF_OPT += --enable-user=$(call qstrip,$(BR2_QUAGGA_ZEBRA_PASSWORD))
else
  ZEBRA_CONF_OPT += --enable-user=admin
endif

ifeq ($(BR2_PACKAGE_ZEBRA_BGPD),y)
ZEBRA_CONF_OPT+=--enable-bgpd
else
ZEBRA_CONF_OPT+=--disable-bgpd
endif

ifeq ($(BR2_PACKAGE_ZEBRA_OSPFD),y)
ZEBRA_CONF_OPT+=--enable-ospfd
else
ZEBRA_CONF_OPT+=--disable-ospfd
endif

ifeq ($(BR2_PACKAGE_ZEBRA_OSPF6D),y)
ZEBRA_CONF_OPT+=--enable-ospf6d
else
ZEBRA_CONF_OPT+=--disable-ospf6d
endif

ifeq ($(BR2_PACKAGE_ZEBRA_RIPNGD),y)
ZEBRA_CONF_OPT+=--enable-ripngd
else
ZEBRA_CONF_OPT+=--disable-ripngd
endif

$(eval $(call AUTOTARGETS))
