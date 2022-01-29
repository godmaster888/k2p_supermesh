#############################################################
#
# pppd
#
#############################################################
PPPD_VERSION = $(call qstrip,$(BR2_PPPD_VERSION))
ifeq ($(call qstrip,$(BR2_PPPD_VERSION)), )
PPPD_VERSION = $(call qstrip,$(BR2_DEFAULT_BRANCH))
endif

PPPD_LICENSE = BSD-3-Clause AND LGPL-2.0-or-later AND GPL-2.0-or-later
PPPD_LICENSE_FILES = README

PPPD_DEPENDENCIES = deuteron_framework

PPPD_CFLAGS = -DDLINK_SOFTWARE -isystem $(STAGING_DIR)/usr/include -L$(STAGING_DIR)/usr/lib

ifneq ($(PPPD_VERSION),rtl)
PPPD_CFLAGS += -DMPPE_OPTIONAL
endif

PPPD_SITE = $(DLINK_GIT_STORAGE)/ppp
PPPD_MAKE_OPT = INSTALL_DIR=$(TARGET_DIR) STRIP=${TARGET_STRIP} CC="$(TARGET_CC) --sysroot=$(STAGING_DIR)" STAGING_DIR=$(STAGING_DIR)  DLINK_SOFTWARE=y

ifeq ($(BR2_INET_IPV6),y)
	PPPD_MAKE_OPT += HAVE_INET6=y
endif

ifeq ($(BR2_PPPD_FILTER),y)
	PPPD_DEPENDENCIES += libpcap
	PPPD_MAKE_OPT += FILTER=y
endif

ifeq ($(BR2_PPPD_DROP_OLD_SESSION),y)
	PPPD_MAKE_OPT += SEND_PADT_ON_START=y
endif

#18758
ifeq ($(BR2_TOOLCHAIN_USES_MUSL),y)
	PPPD_MAKE_OPT += MUSL_LIBC=y
endif

ifeq ($(BR2_i386)$(BR2_x86_64),y)
	PPPD_MAKE_OPT += BR2_i386=y
endif

CONFIGURE := ./configure --target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--sysconfdir=/tmp \
		$(DISABLE_DOCUMENTATION) \
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
		$(DISABLE_IPV6) \
		$(QUIET)

ifeq ($(BR2_PACKAGE_PPPD_PPTP),y)
	PLUGINS += pptp
endif

ifeq ($(BR2_PACKAGE_PPPD_PPPOE),y)
	PLUGINS += rp-pppoe
endif

ifeq ($(BR2_PACKAGE_PPPD_PPPOA),y)
	PLUGINS += pppoatm
endif

ifeq ($(BR2_PACKAGE_PPPD_L2TP),y)
	PLUGINS += mediatek_openl2tp
endif

define PPPD_CONFIGURE_CMDS
	cd ${@D} && ${CONFIGURE} UNAME_S="Linux" UNAME_M="$(ARCH)" UNAME_R="2.6.21"
endef

define PPPD_BUILD_CMDS
	${MAKE} CC="$(TARGET_CC)" LD="$(TARGET_LD)" EXTRA_LIBS="-ljansson -ld_service_notify" EXTRA_CFLAGS="$(PPPD_CFLAGS)" -C $(@D) SUBDIRS="$(PLUGINS)" $(PPPD_MAKE_OPT)
endef

define PPPD_INSTALL_TARGET_CMDS
	${MAKE} -C ${@D} CC="$(TARGET_CC)" LD="$(TARGET_LD)" install INSTROOT=$(TARGET_DIR) DESTDIR=$(TARGET_DIR) STRIP=${TARGET_STRIP} SUBDIRS="$(PLUGINS)"
endef

define PPPD_CLEAN_CMDS
        $(PPPD_MAKE_OPT) ${MAKE} -C $(@D) clean
endef

$(eval $(call GENTARGETS))
