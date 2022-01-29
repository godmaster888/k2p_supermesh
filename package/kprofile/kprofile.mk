#############################################################
#
# kprofile
#
#############################################################
KPROFILE_VERSION = master
KPROFILE_SITE = ${DLINK_GIT_STORAGE}/kprofile
KPROFILE_LICENSE = GPL-2.0-or-later
KPROFILE_LICENSE_FILES =
#KPROFILE_DEPENDENCIES =

#$(MAKE) V=99 CC="$(TARGET_CC)" LD="$(TARGET_LD)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)" -C $(@D)

SDK_TARGETS =
ifeq ($(BR2_KPROFILE_READPROFILE),y)
SDK_TARGETS += readprofile
endif
ifeq ($(BR2_KPROFILE_KERNELTOP),y)
SDK_TARGETS += kerneltop
endif

define KPROFILE_BUILD_CMDS
	$(MAKE) -C $(@D) CC="$(TARGET_CC)" SDK_TARGETS="$(SDK_TARGETS)"
endef

define KPROFILE_INSTALL_TARGET_CMDS
	for i in $(SDK_TARGETS); do \
		$(INSTALL) -D $(@D)/$$i $(TARGET_DIR)/sbin; \
		$(TARGET_STRIP) $(TARGET_DIR)/sbin/$$i; \
	done
endef


$(eval $(call GENTARGETS))
