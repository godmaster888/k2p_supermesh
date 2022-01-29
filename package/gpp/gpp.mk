#############################################################
#
# gpp
#
#############################################################

GPP_VERSION = master
GPP_SITE_METHOD = git
GPP_SITE = ${DLINK_GIT_STORAGE}/gpp
GPP_LICENSE = LGPL-3.0-or-later
GPP_LICENSE_FILES = COPYING.LESSER
GPP_INSTALL_STAGING = NO
HOST_GPP_AUTORECONF = YES

export GPP=$(HOST_DIR)/usr/bin/gpp


define HOST_GPP_UNINSTALL_STAGING_CMDS
    @$(MAKE) uninstall -C $(@D)
endef

define HOST_GPP_UNINSTALL_TARGET_CMDS
    @echo Complete!
endef

$(eval $(call AUTOTARGETS,host))
