#############################################################
#
# iproute2
#
#############################################################

# $(info $$TARGET_MAKE_ENV is [${TARGET_MAKE_ENV}])

IPROUTE2_VERSION = $(call qstrip,$(BR2_IPROUTE2_VERSION))
IPROUTE2_SOURCE = iproute2-$(BR2_IPROUTE2_VERSION).tar.gz
# позже переделать на локальный git-репозиторий
# IPROUTE2_SITE = https://mirrors.edge.kernel.org/pub/linux/utils/net/iproute2/
IPROUTE2_SITE = $(DLINK_STORAGE)
IPROUTE2_LICENSE = GPL-2.0-or-later
IPROUTE2_LICENSE_FILES = COPYING
IPROUTE2_DEPENDENCIES = host-bison\
	                    host-pkg-config \
	                    $(if $(BR2_PACKAGE_LIBMNL),libmnl)

IPROUTE2_TARGET_SBINS = ctstat genl ifstat ip lnstat nstat routef routel rtacct rtmon rtpr rtstat ss tc

ifeq ($(BR2_PACKAGE_IPTABLES)x$(BR2_STATIC_LIBS),yx)
IPROUTE2_DEPENDENCIES += iptables
else
define IPROUTE2_DISABLE_IPTABLES
	# m_xt.so is built unconditionally
	echo "TC_CONFIG_XT:=n" >>$(@D)/config.mk
endef
endif

ifeq ($(BR2_PACKAGE_BERKELEYDB_COMPAT185),y)
IPROUTE2_DEPENDENCIES += berkeleydb
endif

IPROUTE2_MANDIR=$(STAGING_DIR)/usr/share/man
IPROUTE2_DOCDIR=$(STAGING_DIR)/usr/share/doc/iproute2-$(IPROUTE2_VERSION)
IPROUTE2_SBIN_DIR = /sbin
IPROUTE2_SUBDIRS = lib

# Т.к. после iproute2 может собираться после busybox'a, то при удалении нужно проверять,
# что удаляем корректные бинари от iproute2, a не simlink'и busybox'a
define IPROUTE2_SMART_DELETE
	test -L $(TARGET_DIR)$(IPROUTE2_SBIN_DIR)/$(1) || $(RM) -v $(TARGET_DIR)$(IPROUTE2_SBIN_DIR)/$(1)
endef

ifeq ($(BR2_IPROUTE2_MODULE_IP),y)
IPROUTE2_SUBDIRS += ip
else
# Выключем модули ip: ip link, ip addr, ip neighbor, ip rule, ip route, ip tunnel, ip maddr, ip mroute, ip monitor
define IPROUTE2_MODULE_IP_REMOVE
	$(call IPROUTE2_SMART_DELETE,ipaddr)
	$(call IPROUTE2_SMART_DELETE,iplink)
	$(call IPROUTE2_SMART_DELETE,ipneigh)
	$(call IPROUTE2_SMART_DELETE,iproute)
	$(call IPROUTE2_SMART_DELETE,iprule)
	$(call IPROUTE2_SMART_DELETE,iptunnel)
	$(call IPROUTE2_SMART_DELETE,ip)
endef
IPROUTE2_POST_INSTALL_TARGET_HOOKS += IPROUTE2_MODULE_IP_REMOVE
endif

ifeq ($(BR2_IPROUTE2_MODULE_BRIDGE),y)
IPROUTE2_SUBDIRS += bridge
else
define IPROUTE2_MODULE_BRIDGE_REMOVE
	$(call IPROUTE2_SMART_DELETE,bridge)
endef
IPROUTE2_POST_INSTALL_TARGET_HOOKS += IPROUTE2_MODULE_BRIDGE_REMOVE
endif

ifeq ($(BR2_IPROUTE2_MODULE_GENL),y)
IPROUTE2_SUBDIRS += genl
else
define IPROUTE2_MODULE_GENL_REMOVE
	$(call IPROUTE2_SMART_DELETE,genl)
endef
IPROUTE2_POST_INSTALL_TARGET_HOOKS += IPROUTE2_MODULE_GENL_REMOVE
endif

ifeq ($(BR2_IPROUTE2_MODULE_IFCFG),y)
IPROUTE2_SUBDIRS += ifcfg
else
define IPROUTE2_MODULE_IFCFG_REMOVE
	$(call IPROUTE2_SMART_DELETE,ifcfg)
endef
IPROUTE2_POST_INSTALL_TARGET_HOOKS += IPROUTE2_MODULE_IFCFG_REMOVE
endif

ifeq ($(BR2_IPROUTE2_MODULE_IFSTAT),y)
IPROUTE2_SUBDIRS += ifstat
else
define IPROUTE2_MODULE_IFSTAT_REMOVE
	$(call IPROUTE2_SMART_DELETE,ifstat)
endef
IPROUTE2_POST_INSTALL_TARGET_HOOKS += IPROUTE2_MODULE_IFSTAT_REMOVE
endif

ifeq ($(BR2_IPROUTE2_MODULE_NSTAT),y)
IPROUTE2_SUBDIRS += nstat
else
define IPROUTE2_MODULE_NSTAT_REMOVE
	$(call IPROUTE2_SMART_DELETE,nstat)
endef
IPROUTE2_POST_INSTALL_TARGET_HOOKS += IPROUTE2_MODULE_NSTAT_REMOVE
endif

ifeq ($(BR2_IPROUTE2_MODULE_ROUT_TABLE),y)
IPROUTE2_SUBDIRS += routel routef
else
define IPROUTE2_MODULE_ROUTE_TABLE_REMOVE
	$(call IPROUTE2_SMART_DELETE,routel)
	$(call IPROUTE2_SMART_DELETE,routef)
endef
IPROUTE2_POST_INSTALL_TARGET_HOOKS += IPROUTE2_MODULE_ROUTE_TABLE_REMOVE
endif

ifeq ($(BR2_IPROUTE2_MODULE_RT),y)
IPROUTE2_SUBDIRS += rtacct rtmon
else
define IPROUTE2_MODULE_RT_REMOVE
	$(call IPROUTE2_SMART_DELETE,rtacct)
	$(call IPROUTE2_SMART_DELETE,rtmon)
endef
IPROUTE2_POST_INSTALL_TARGET_HOOKS += IPROUTE2_MODULE_RT_REMOVE
endif

ifeq ($(BR2_IPROUTE2_MODULE_LNSTAT),y)
IPROUTE2_SUBDIRS += ctstat rtstat lnstat
else
define IPROUTE2_MODULE_LNSTAT_REMOVE
	$(call IPROUTE2_SMART_DELETE,ctstat)
	$(call IPROUTE2_SMART_DELETE,rtstat)
	$(call IPROUTE2_SMART_DELETE,lnstat)
endef
IPROUTE2_POST_INSTALL_TARGET_HOOKS += IPROUTE2_MODULE_LNSTAT_REMOVE
endif

ifeq ($(BR2_IPROUTE2_MODULE_SS),y)
IPROUTE2_SUBDIRS += ss
else
define IPROUTE2_MODULE_SS_REMOVE
	$(call IPROUTE2_SMART_DELETE,ss)
endef
IPROUTE2_POST_INSTALL_TARGET_HOOKS += IPROUTE2_MODULE_SS_REMOVE
endif

ifeq ($(BR2_IPROUTE2_MODULE_TC),y)
IPROUTE2_SUBDIRS += tc
else
# Выключем модуль tc (TODO: сделать выключения отдельных модулей)
define IPROUTE2_MODULE_TC_REMOVE
	$(call IPROUTE2_SMART_DELETE,tc)
endef
IPROUTE2_POST_INSTALL_TARGET_HOOKS += IPROUTE2_MODULE_TC_REMOVE
endif


define IPROUTE2_CLEAN_CMDS
	$(TARGET_MAKE_ENV) DESTDIR="$(TARGET_DIR)" $(MAKE) -C $(@D) clean
endef


define IPROUTE2_CONFIGURE_CMDS
	# disabled arpd (needs berkeleydb)
	sed -i '/^TARGETS/s@arpd@@g' $(IPROUTE2_DIR)/misc/Makefile
	sed -i /ARPD/d $(IPROUTE2_DIR)/Makefile
	# disabled generate .conf files
	sed -i '/CONFDIR/d' $(IPROUTE2_DIR)/Makefile
	cd $(@D) && $(TARGET_CONFIGURE_OPTS) MANDIR="$(IPROUTE2_MANDIR)" DOCDIR="$(IPROUTE2_DOCDIR)"\
	SBINDIR="$(IPROUTE2_SBIN_DIR)" ./configure
	$(IPROUTE2_DISABLE_IPTABLES)
endef


define IPROUTE2_BUILD_CMDS
	$(TARGET_MAKE_ENV) LDFLAGS="$(TARGET_LDFLAGS)" \
		CFLAGS="$(TARGET_CFLAGS) -DXT_LIB_DIR=\\\"/usr/lib/xtables\\\"" \
		CBUILD_CFLAGS="$(HOST_CFLAGS)" $(MAKE) V=1 LIBDB_LIBS=-lpthread \
		DBM_INCLUDE="$(STAGING_DIR)/usr/include" \
		SUBDIRS="$(IPROUTE2_SUBDIRS)" \
		SHARED_LIBS="$(if $(BR2_STATIC_LIBS),n,y)" -C $(@D)
endef


define IPROUTE2_INSTALL_TARGET_CMDS
	$(info IPROUTE2_INSTALL_TARGET_CMDS is [${TARGET_MAKE_ENV}])
	$(TARGET_MAKE_ENV) DESTDIR="$(TARGET_DIR)" SUBDIRS="$(IPROUTE2_SUBDIRS)" $(MAKE) -C $(@D) install
endef

$(eval $(call GENTARGETS))
