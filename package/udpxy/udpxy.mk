#############################################################
#
# udpxy
#
#############################################################
UDPXY_VERSION = master
UDPXY_SITE = $(DLINK_GIT_STORAGE)/udpxy
UDPXY_LICENSE = GPL-3.0-or-later
UDPXY_LICENSE_FILES = README
UDPXY_INSTALL_TARGET = YES
UDPXY_DEPENDENCIES = jansson deuteron_framework

ifeq ($(BR2_rlx),y)
  UDPXY_CFLAGS += -DRLX
endif

ifeq ($(BR2_UDPXY_KERNEL_MULTICAST),y)
  UDPXY_CFLAGS += -DUDPXY_KERNEL_MULTICAST
endif

UDPXY_MAKE_ENV = CFLAGS="$(TARGET_CFLAGS) $(UDPXY_CFLAGS)"
UDPXY_MAKE_OPTS = \
        CC="$(TARGET_CC) --sysroot=$(STAGING_DIR)" \
        LD="$(TARGET_LD)" \
        LDFLAGS=" -L$(STAGING_DIR)/usr/lib -ljansson -ld_service_notify"

define UDPXY_BUILD_CMDS
        $(UDPXY_MAKE_ENV) $(MAKE) $(UDPXY_MAKE_OPTS) -C $(@D) release
endef

define  UDPXY_INSTALL_TARGET_CMDS
	$(INSTALL) -pm0755 $(@D)/udpxy $(TARGET_DIR)/usr/bin/
	$(INSTALL) -pm0755 $(@D)/udpxrec $(TARGET_DIR)/usr/bin/

endef
define UDPXY_CLEAN_CMDS
        $(MAKE) -C $(@D) clean
       rm -f $(TARGET_DIR)/usr/bin/udpxy
       rm -f $(TARGET_DIR)/usr/bin/udpxrec


endef





$(eval $(call GENTARGETS))

