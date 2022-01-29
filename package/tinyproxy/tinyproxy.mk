#############################################################
#
# tinyproxy
#
#############################################################
TINYPROXY_SITE = $(DLINK_GIT_STORAGE)/tinyproxy
TINYPROXY_VERSION = master
TINYPROXY_LICENSE = GPL-2.0-or-later
TINYPROXY_LICENSE_FILES = COPYING
TINYPROXY_CONF_OPT = ac_cv_path_A2X=yes ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes \
					--sysconfdir=/tmp --enable-filter --enable-transparent --disable-regexcheck --disable-xtinyproxy --disable-upstream

ifeq ($(BR2_SUPPORT_NSG),y)
TINYPROXY_MAKE_OPT += CFLAGS="$(TARGET_CFLAGS) -DSUPPORT_NSG"
endif

define TINYPROXY_INSTALL_TARGET_CMDS
	install -pm0755 $(@D)/src/tinyproxy $(TARGET_DIR)/usr/sbin/tinyproxy
	$(TARGET_STRIP) $(TARGET_DIR)/usr/sbin/tinyproxy
endef

$(eval $(call AUTOTARGETS))
