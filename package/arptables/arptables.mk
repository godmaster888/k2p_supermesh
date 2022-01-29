#############################################################
#
# arptables
#
#############################################################
ARPTABLES_VERSION = master
ARPTABLES_SITE = $(DLINK_GIT_STORAGE)/arptables
ARPTABLES_LICENSE = GPL-2.0-or-later
ARPTABLES_LICENSE_FILES = debian/copyright
ARPTABLES_MAKE_OPTS = \
	CC="$(TARGET_CC)" \
	LD="$(TARGET_LD)" \
	PREFIX="$(TARGET_DIR)" \
	STRIP=${TARGET_STRIP} \
	INSTALL_DIR=$(TARGET_DIR) 

ifeq ($(BR2_LINUX_KERNEL_3_10),y)
ARPTABLES_MAKE_OPTS += CONFIG_KERNEL_3_10=y
endif

define ARPTABLES_BUILD_CMDS
	$(MAKE) ${ARPTABLES_MAKE_OPTS} -C $(@D) install
endef

define ARPTABLES_CLEAN_CMDS
	$(MAKE) -C ${@D} clean
endef

$(eval $(call GENTARGETS))
