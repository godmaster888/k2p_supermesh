#############################################################
#
# p910nd
#
#############################################################
P910ND_VERSION = 0.97
P910ND_SITE=${DLINK_GIT_STORAGE}/p910nd
P910ND_LICENSE = GPL-2.0-only
P910ND_LICENSE = COPYING
P910ND_LIBS =

ifeq ($(BR2_DSYSINIT), y)
	P910ND_LIBS += -L$(STAGING_DIR)/usr/lib -ld_service_notify -ljansson -Wl,-rpath-link,$(STAGING_DIR)/usr/lib
endif

P910ND_MAKE_OPTS = \
	CFLAGS="$(TARGET_CFLAGS) -DVERSION=0.9.8 -O2  -Wall " \
	CC="$(TARGET_CC)" \
	LD="$(TARGET_CC)" \
	DESTDIR="$(TARGET_DIR)" \
	EXTRA_LDFLAGS="$(TARGET_LDFLAGS)" \
	EXTRA_LIBS="$(P910ND_LIBS)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	STRIP="$(TARGET_STRIP)"

define P910ND_BUILD_CMDS
	$(MAKE) $(P910ND_MAKE_OPTS) -C $(@D) install
endef

define P910ND_INSTALL_TARGET_CMDS
	$(MAKE) $(P910ND_MAKE_OPTS) -C $(@D) install
endef

define P910ND_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

$(eval $(call GENTARGETS))
