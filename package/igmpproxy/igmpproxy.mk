#############################################################
#
# igmpproxy
#
#
#############################################################

IGMPPROXY_VERSION = $(call qstrip,$(BR2_PACKAGE_IGMPPROXY_BRANCH))
ifeq (${IGMPPROXY_VERSION},)
IGMPPROXY_VERSION = $(call qstrip,$(BR2_DEFAULT_BRANCH))
endif

IGMPPROXY_SITE = $(DLINK_GIT_STORAGE)/igmpproxy
IGMPPROXY_LICENSE = GPL-2.0-or-later
IGMPPROXY_LICENSE_FILES = COPYING

#ifeq (${BR2_PACKAGE_IGMPPROXY},y)
#TARGET_CFLAGS += -DSUPPORT_IGMP
#endif

ifeq (${IGMPPROXY_VERSION},$(filter ${IGMPPROXY_VERSION},mediatek rlx))
IGMPPROXY_DEPENDENCIES := kernel 

IGMPPROXY_OPT= \
	INSTALL_DIR=$(TARGET_DIR) \
	CC="$(TARGET_CC)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	MAKE="$(MAKE)" \
	LD="$(TARGET_LD)" \
	STRIP=${TARGET_STRIP} \

define IGMPPROXY_BUILD_CMDS
	${MAKE1} $(IGMPPROXY_OPT) -C $(@D) all
endef

define IGMPPROXY_CLEAN_CMDS
	${MAKE} -C $(@D) clean
endef

ifeq (${IGMPPROXY_VERSION}, mediatek)
define IGMPPROXY_INSTALL_TARGET_CMDS
	install -pm0755 ${@D}/src/igmpproxy $(TARGET_DIR)/usr/bin/igmpproxy
endef
endif

ifeq (${IGMPPROXY_VERSION}, rlx)
define IGMPPROXY_INSTALL_TARGET_CMDS
	install -pm0755 ${@D}/igmpproxy $(TARGET_DIR)/bin/igmpproxy
endef
endif

$(eval $(call GENTARGETS))
else
$(eval $(call AUTOTARGETS))
endif

