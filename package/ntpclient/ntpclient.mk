#############################################################
#
# ntpclient
#
#############################################################

NTPCLIENT_VERSION = 2010_365
NTPCLIENT_SITE = ${DLINK_GIT_STORAGE}/ntpclient
NTPCLIENT_LICENSE = GPL-2.0-only
NTPCLIENT_LICENSE_FILES = README
NTPCLIENT_MAKE_OPTS = \
	CFLAGS="$(TARGET_CFLAGS)" \
	CC="$(TARGET_CC)" \
	LD="$(TARGET_LD)" \
	PREFIX="$(TARGET_DIR)" \
	EXTRA_LDFLAGS="$(TARGET_LDFLAGS)" \
	CROSS_COMPILE="$(TARGET_CROSS)" 

define NTPCLIENT_BUILD_CMDS
	$(MAKE) $(NTPCLIENT_MAKE_OPTS) -C $(@D) all
endef

define NTPCLIENT_INSTALL_TARGET_CMDS
	install -pm0755 $(@D)/ntpclient $(TARGET_DIR)/usr/bin/
endef
$(eval $(call GENTARGETS))
