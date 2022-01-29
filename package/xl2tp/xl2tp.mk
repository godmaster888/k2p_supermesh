#############################################################
#
# xl2tp
#
#############################################################

XL2TP_SITE = $(DLINK_GIT_STORAGE)/xl2tp
XL2TP_LICENSE = GPL-2.0-or-later
XL2TP_LICENSE_FILES =
XL2TP_DEPENDENCIES = pppd

XL2TP_VERSION = $(call qstrip,$(BR2_PACKAGE_XL2TP_VERSION))
ifeq ($(call qstrip,$(BR2_PACKAGE_XL2TP_VERSION)), )
XL2TP_VERSION = $(call qstrip,$(BR2_DEFAULT_BRANCH))
endif

XL2TP_TARGETS = xl2tpd

ifeq ($(BR2_PACKAGE_XL2TP_CTRL),y)
    XL2TP_TARGETS += xl2tpd-control
endif

ifeq ($(BR2_PACKAGE_XL2TP_PFC),y)
    XL2TP_TARGETS += pfc
    XL2TP_DEPENDENCIES += libpcap
endif

XL2TP_EXTRA_LIBS =

ifeq ($(BR2_DSYSINIT),y)
	XL2TP_DEPENDENCIES += deuteron_framework
	XL2TP_EXTRA_LIBS += -ld_service_notify -ljansson
endif

XL2TP_CFLAGS = \
		-DTRUST_PPPD_TO_DIE \
		-O2 \
		-fno-builtin \
		-Wall \
		-DSANITY \
		-DLINUX \
		-DDEBUG_CLOSE \
		-DIP_ALLOCATION \
		-I$(STAGING_DIR)/usr/include

ifeq ($(BR2_PACKAGE_XL2TP_KERNEL_SUPPORT),y)
XL2TP_CFLAGS += -DUSE_KERNEL
endif

#18758
ifeq ($(BR2_TOOLCHAIN_USES_MUSL),y)
XL2TP_CFLAGS += -DMUSL_LIBC
endif

ifeq ($(BR2_DSYSINIT),y)
XL2TP_CFLAGS += -DDLINK_SOFTWARE
endif

ifeq ($(BR2_PACKAGE_XL2TP_AS_PTY),y)
XL2TP_CFLAGS += -DAS_PTY -DSEND_PTY_READY
endif

XL2TP_LDFLAGS = \
		-L$(STAGING_DIR)/usr/lib \
		$(XL2TP_EXTRA_LIBS)

XL2TP_MAKE_OPTS = \
				  CC="$(TARGET_CC) --sysroot=$(STAGING_DIR)" \
				  LD="$(TARGET_LD)" \
				  STRIP="$(TARGET_STRIP)" \
				  LINUX26_DIR=$(LINUX26_DIR) \
				  CFLAGS="$(XL2TP_CFLAGS)" \
				  LDFLAGS="$(XL2TP_LDFLAGS)" \
				  $(XL2TP_TARGETS)


define XL2TP_BUILD_CMDS
	make -C $(@D) $(XL2TP_MAKE_OPTS)
endef

define XL2TP_INSTALL_TARGET_CMDS
	cp -f $(addprefix $(@D)/,$(XL2TP_TARGETS)) $(TARGET_DIR)/usr/sbin
endef

define XL2TP_CLEAN_CMDS
	make CC=$(TARGET_CC) -C $(@D) clean
endef

ifeq ($(BR2_PACKAGE_XL2TP),y)
$(eval $(call GENTARGETS))
endif #BR2_XL2TP_CLIENT
