################################################################################
#
# iptables
#
################################################################################
IPTABLES_VERSION =$(call qstrip,$(BR2_IPTABLES_VERSION))

ifeq (${IPTABLES_VERSION},)
IPTABLES_VERSION = 1.6.1
endif

IPTABLES_SITE = $(DLINK_GIT_STORAGE)/iptables
IPTABLES_INSTALL_STAGING = YES
IPTABLES_DEPENDENCIES = host-pkg-config kernel\
	$(if $(BR2_PACKAGE_LIBNETFILTER_CONNTRACK),libnetfilter_conntrack)
IPTABLES_LICENSE = GPL-2.0-or-later
IPTABLES_LICENSE_FILES = COPYING
IPTABLES_AUTORECONF = YES
IPTABLES_CFLAGS = $(TARGET_CFLAGS)

# D-Link: we need this because of our broken toolchains
IPTABLES_CONF_ENV = CC="$(TARGET_CC) --sysroot=$(STAGING_DIR)" CFLAGS="$(IPTABLES_CFLAGS)"

ifeq ($(BR2_PACKAGE_IPTABLES_SKB_MARK2_SUPPORT),y)
IPTABLES_CFLAGS += -DCONFIG_RTK_SKB_MARK2 
IPTABLES_CONF_ENV += CONFIG_RTK_SKB_MARK2=y
endif

# Building static causes ugly warnings on some plugins
IPTABLES_CONF_OPT = --libexecdir=/usr/lib --with-kernel=$(STAGING_DIR)/usr \
	$(if $(BR2_STATIC_LIBS),,--disable-static)

# For connlabel match
ifeq ($(BR2_PACKAGE_LIBNETFILTER_CONNTRACK),y)
IPTABLES_DEPENDENCIES += libnetfilter_conntrack
endif

# For nfnl_osf
ifeq ($(BR2_PACKAGE_LIBNFNETLINK),y)
IPTABLES_DEPENDENCIES += libnfnetlink
endif

# For iptables-compat tools
ifeq ($(BR2_PACKAGE_IPTABLES_NFTABLES),y)
IPTABLES_CONF_OPT += --enable-nftables
IPTABLES_DEPENDENCIES += host-bison host-flex libmnl libnftnl
else
IPTABLES_CONF_OPT += --disable-nftables
endif

# bpf compiler support and nfsynproxy tool
ifeq ($(BR2_PACKAGE_IPTABLES_BPF_NFSYNPROXY),y)
# libpcap is tricky for static-only builds and needs help
ifeq ($(BR2_STATIC_LIBS),y)
IPTABLES_LIBS_FOR_STATIC_LINK += `$(STAGING_DIR)/usr/bin/pcap-config --static --additional-libs`
IPTABLES_CONF_OPT += LIBS="$(IPTABLES_LIBS_FOR_STATIC_LINK)"
endif
IPTABLES_CONF_OPT += --enable-bpf-compiler --enable-nfsynproxy
IPTABLES_DEPENDENCIES += libpcap
else
IPTABLES_CONF_OPT += --disable-bpf-compiler --disable-nfsynproxy
endif

# D-Link: fix for some broken toolchains
IPTABLES_CONF_OPT += --disable-largefile

# D-Link: disable ipv6 support if not needed
ifneq ($(BR2_INET_IPV6),y)
IPTABLES_CONF_OPT += --disable-ipv6
endif


IPTABLES_CLEANUP_FILES =                                \
	$(TARGET_DIR)/usr/lib/xtables/libebt_802_3.so       \
	$(TARGET_DIR)/usr/lib/xtables/libip6t_DNPT.so       \
	$(TARGET_DIR)/usr/lib/xtables/libip6t_NETMAP.so     \
	$(TARGET_DIR)/usr/lib/xtables/libip6t_SNPT.so       \
	$(TARGET_DIR)/usr/lib/xtables/libip6t_eui64.so      \
	$(TARGET_DIR)/usr/lib/xtables/libip6t_frag.so       \
	$(TARGET_DIR)/usr/lib/xtables/libip6t_hbh.so        \
	$(TARGET_DIR)/usr/lib/xtables/libip6t_mh.so         \
	$(TARGET_DIR)/usr/lib/xtables/libip6t_rt.so         \
	$(TARGET_DIR)/usr/lib/xtables/libipt_CLUSTERIP.so   \
	$(TARGET_DIR)/usr/lib/xtables/libipt_ECN.so         \
	$(TARGET_DIR)/usr/lib/xtables/libipt_realm.so       \
	$(TARGET_DIR)/usr/lib/xtables/libxt_AUDIT.so        \
	$(TARGET_DIR)/usr/lib/xtables/libxt_CHECKSUM.so     \
	$(TARGET_DIR)/usr/lib/xtables/libxt_CLASSIFY.so     \
	$(TARGET_DIR)/usr/lib/xtables/libxt_CONNSECMARK.so  \
	$(TARGET_DIR)/usr/lib/xtables/libxt_DSCP.so         \
	$(TARGET_DIR)/usr/lib/xtables/libxt_HMARK.so        \
	$(TARGET_DIR)/usr/lib/xtables/libxt_IDLETIMER.so    \
	$(TARGET_DIR)/usr/lib/xtables/libxt_LED.so          \
	$(TARGET_DIR)/usr/lib/xtables/libxt_RATEEST.so      \
	$(TARGET_DIR)/usr/lib/xtables/libxt_SECMARK.so      \
	$(TARGET_DIR)/usr/lib/xtables/libxt_TCPOPTSTRIP.so  \
	$(TARGET_DIR)/usr/lib/xtables/libxt_TEE.so          \
	$(TARGET_DIR)/usr/lib/xtables/libxt_TPROXY.so       \
	$(TARGET_DIR)/usr/lib/xtables/libxt_TRACE.so        \
	$(TARGET_DIR)/usr/lib/xtables/libxt_addrtype.so     \
	$(TARGET_DIR)/usr/lib/xtables/libxt_bpf.so          \
	$(TARGET_DIR)/usr/lib/xtables/libxt_cgroup.so       \
	$(TARGET_DIR)/usr/lib/xtables/libxt_cluster.so      \
	$(TARGET_DIR)/usr/lib/xtables/libxt_comment.so      \
	$(TARGET_DIR)/usr/lib/xtables/libxt_cpu.so          \
	$(TARGET_DIR)/usr/lib/xtables/libxt_dscp.so         \
	$(TARGET_DIR)/usr/lib/xtables/libxt_ecn.so          \
	$(TARGET_DIR)/usr/lib/xtables/libxt_hashlimit.so    \
	$(TARGET_DIR)/usr/lib/xtables/libxt_ipcomp.so       \
	$(TARGET_DIR)/usr/lib/xtables/libxt_ipvs.so         \
	$(TARGET_DIR)/usr/lib/xtables/libxt_nfacct.so       \
	$(TARGET_DIR)/usr/lib/xtables/libxt_osf.so          \
	$(TARGET_DIR)/usr/lib/xtables/libxt_owner.so        \
	$(TARGET_DIR)/usr/lib/xtables/libxt_physdev.so      \
	$(TARGET_DIR)/usr/lib/xtables/libxt_pkttype.so      \
	$(TARGET_DIR)/usr/lib/xtables/libxt_quota.so        \
	$(TARGET_DIR)/usr/lib/xtables/libxt_rateest.so      \
	$(TARGET_DIR)/usr/lib/xtables/libxt_rpfilter.so     \
	$(TARGET_DIR)/usr/lib/xtables/libxt_sctp.so         \
	$(TARGET_DIR)/usr/lib/xtables/libxt_string.so       \
	$(TARGET_DIR)/usr/lib/xtables/libxt_time.so         \
	$(TARGET_DIR)/usr/lib/xtables/libxt_u32.so

ifneq ($(BR2_SUPPORT_LOAD_BALANCING),y)
IPTABLES_CLEANUP_FILES += $(TARGET_DIR)/usr/lib/xtables/libxt_CONNMARK.so
IPTABLES_CLEANUP_FILES += $(TARGET_DIR)/usr/lib/xtables/libxt_statistic.so
endif

ifneq ($(BR2_PACKAGE_IPTABLES_TTL_IPV4),y)
IPTABLES_CLEANUP_FILES += $(TARGET_DIR)/usr/lib/xtables/libipt_TTL.so
IPTABLES_CLEANUP_FILES += $(TARGET_DIR)/usr/lib/xtables/libipt_ttl.so
endif

ifneq ($(BR2_PACKAGE_IPTABLES_TTL_IPV6),y)
IPTABLES_CLEANUP_FILES += $(TARGET_DIR)/usr/lib/xtables/libip6t_HL.so
IPTABLES_CLEANUP_FILES += $(TARGET_DIR)/usr/lib/xtables/libip6t_hl.so
endif

ifneq ($(BR2_PACKAGE_IPTABLES_SKB_MARK2_SUPPORT),y)
IPTABLES_CLEANUP_FILES += $(TARGET_DIR)/usr/lib/xtables/libxt_mark2.so
IPTABLES_CLEANUP_FILES += $(TARGET_DIR)/usr/lib/xtables/libxt_MARK2.so
IPTABLES_CLEANUP_FILES += $(TARGET_DIR)/usr/lib/xtables/libxt_connmark2.so
IPTABLES_CLEANUP_FILES += $(TARGET_DIR)/usr/lib/xtables/libxt_CONNMARK2.so
endif

define IPTABLES_CLEANUP
	rm -vf $(IPTABLES_CLEANUP_FILES)
endef

IPTABLES_POST_INSTALL_TARGET_HOOKS += IPTABLES_CLEANUP

$(eval $(call AUTOTARGETS))
