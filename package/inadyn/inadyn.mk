#############################################################
#
# inadyn
#
#############################################################
INADYN_VERSION = 2.7
INADYN_SITE = $(DLINK_GIT_STORAGE)/inadyn
INADYN_LICENSE = GPL-2.0-or-later
INADYN_LICENSE_FILES = COPYING
INADYN_AUTORECONF = YES
INADYN_DEPENDENCIES = host-pkg-config libconfuse libite jansson deuteron_framework

# Needed for autoreconf to work properly, see ./autogen.sh
define INADYN_FIXUP_M4_DIR
	mkdir $(@D)/m4
endef

INADYN_POST_EXTRACT_HOOKS += INADYN_FIXUP_M4_DIR

ifeq ($(BR2_PACKAGE_INADYN_SSL),y)
INADYN_CONF_OPT += --enable-openssl
INADYN_DEPENDENCIES += openssl
else
INADYN_CONF_OPT += --disable-ssl
endif

# DLINK_SOFTWARE d_service_notify linking
INADYN_LDFLAGS += -ljansson -ld_service_notify -L$(STAGING_DIR)/usr/lib -Wl,-rpath-link,$(STAGING_DIR)/lib -Wl,-rpath-link,$(STAGING_DIR)/usr/lib

INADYN_CONF_ENV += LIBS="$(INADYN_LDFLAGS)"

$(eval $(call AUTOTARGETS))
