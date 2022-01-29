#############################################################
#
# jansson
#
#############################################################

JANSSON_VERSION = 2.12
JANSSON_SITE_METHOD = git
JANSSON_SITE = $(DLINK_GIT_STORAGE)/jansson
JANSSON_LICENSE = MIT
JANSSON_LICENSE_FILES = LICENSE
JANSSON_INSTALL_STAGING = YES
JANSSON_SUPPORTS_IN_SOURCE_BUILD = NO

ifeq ($(BR2_USER_DEBUG), y)
  JANSSON_CONF_OPT += -DCMAKE_BUILD_TYPE=Debug
endif

HOST_JANSSON_CONF_OPT += -DCMAKE_BUILD_TYPE=Debug

$(eval $(call CMAKETARGETS))
$(eval $(call CMAKETARGETS,host))
