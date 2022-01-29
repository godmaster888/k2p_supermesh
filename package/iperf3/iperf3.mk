IPERF3_SITE = $(DLINK_GIT_STORAGE)/iperf3
IPERF3_VERSION = 3.7
IPERF3_LICENSE = BSD-3-Clause-LBNL AND BSD-3-Clause AND MIT
IPERF3_LICENSE_FILES = LICENSE
IPERF3_CONF_OPT = --disable-shared
IPERF3_MAKE_OPT = noinst_PROGRAMS=

ifeq ($(BR2_DSYSINIT),y)
IPERF3_DEPENDENCIES += deuteron_framework
IPERF3_LDFLAGS += -ld_service_notify -Wl,-rpath-link,$(STAGING_DIR)/lib -Wl,-rpath-link,$(STAGING_DIR)/usr/lib
endif

IPERF3_MAKE_OPT += CC="$(TARGET_CC)" LD="$(TARGET_LD)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS) $(IPERF3_LDFLAGS)"

define IPERF3_INSTALL_TARGET_CMDS
	install -pm0755 $(@D)/src/iperf3 $(TARGET_DIR)/usr/sbin/
	$(TARGET_STRIP) $(TARGET_DIR)/usr/sbin/iperf3
endef

$(eval $(call AUTOTARGETS))
