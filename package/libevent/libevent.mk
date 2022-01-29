#############################################################
#
# libevent
#
#############################################################
LIBEVENT_VERSION = 2.0.22
LIBEVENT_SITE = $(DLINK_GIT_STORAGE)/libevent
LIBEVENT_INSTALL_STAGING = YES
LIBEVENT_LICENSE = BSD-3-Clause AND ISC
LIBEVENT_LICENSE_FILES = LICENSE
# disable building test programs and samples --> need autoreconf
LIBEVENT_AUTORECONF = YES

define LIBEVENT_REMOVE_PYSCRIPT
	rm $(TARGET_DIR)/usr/bin/event_rpcgen.py
endef

define LIBEVENT_MKLIBS_HOOK
	cp $(STAGING_DIR)/usr/lib/libevent.a $(STAGING_DIR)/lib/libevent-2.0_pic.a
endef

# libevent installs a python script to target - get rid of it
LIBEVENT_POST_INSTALL_TARGET_HOOKS += LIBEVENT_REMOVE_PYSCRIPT

# disable additional libraries
ifeq ($(BR2_PACKAGE_SSLSPLIT),y)
LIBEVENT_DEPENDENCIES += openssl
LIBEVENT_CONF_OPT += --disable-core-extra
else
LIBEVENT_CONF_OPT += --disable-openssl --disable-thread-support --disable-core-extra
endif

# disable debug mode
LIBEVENT_CONF_OPT += --disable-debug-mode

# disable epoll
LIBEVENT_CONF_OPT += ac_cv_func_epoll_ctl=no ac_cv_header_sys_epoll_h=no
LIBEVENT_CONF_OPT += --disable-static



$(eval $(call AUTOTARGETS))
