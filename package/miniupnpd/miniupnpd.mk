#############################################################
#
# miniupnpd
#
#############################################################

MINIUPNPD_VERSION = 2.0
MINIUPNPD_SITE = $(DLINK_GIT_STORAGE)/miniupnpd
MINIUPNPD_LICENSE = BSD-3-Clause
MINIUPNPD_LICENSE_FILES = LICENSE
MINIUPNPD_DEPENDENCIES = openssl iptables libnfnetlink util-linux
MINIUPNPD_LIBS = -lnfnetlink
MINIUPNPD_CFLAGS = $(TARGET_CFLAGS)

ifeq ($(BR2_DSYSINIT),y)
MINIUPNPD_DEPENDENCIES += jansson deuteron_framework
MINIUPNPD_LDFLAGS = $(TARGET_LDFLAGS) -ljansson -ld_service_notify -Wl,-rpath-link,$(STAGING_DIR)/lib -Wl,-rpath-link,$(STAGING_DIR)/usr/lib
endif

MINIUPNPD_CFLAGS += -I${STAGING_DIR}/include -DENABLE_LEASEFILE

MINIUPNPD_CONFIG_OPTIONS += --vendorcfg

MINIUPNPD_MAKE_ENV = \
		     CC="$(TARGET_CC)" \
		     LD="$(TARGET_LD)" \
		     CONFIG_OPTIONS=${MINIUPNPD_CONFIG_OPTIONS}

MINIUPNPD_MAKE_OPTS = \
		      LIBS="$(MINIUPNPD_LIBS)" \
		      CFLAGS="$(MINIUPNPD_CFLAGS)" \
		      LDFLAGS="$(MINIUPNPD_LDFLAGS)" \
		      STRIP=${TARGET_STRIP} \
		      IPTABLESPATH=${IPTABLES_DIR}


define MINIUPNPD_BUILD_CMDS
	$(MINIUPNPD_MAKE_ENV) $(MAKE) V=99 $(MINIUPNPD_MAKE_OPTS) -f Makefile.linux -C $(@D) miniupnpd
endef

define MINIUPNPD_CLEAN_CMDS
	$(MAKE) -f Makefile.linux -C $(@D) clean
endef

define MINIUPNPD_INSTALL_TARGET_CMDS
	$(INSTALL) $(@D)/miniupnpd $(TARGET_DIR)/sbin
endef

$(eval $(call GENTARGETS))
