#############################################################
#
# memwatch
#
#############################################################


MEMWATCH_SITE := $(DLINK_GIT_STORAGE)/memwatch
MEMWATCH_VERSION := master
MEMWATCH_LICENSE = GPL-2.0-or-later
MEMWATCH_LICENSE_FILES = gpl.txt
MEMWATCH_MAKE_OPTS += CC=$(TARGET_CC)
MEMWATCH_INSTALL_STAGING = YES
MEMWATCH_OBJ = $(STAGING_DIR)/usr/lib/memwatch.o

define MEMWATCH_BUILD_CMDS
	$(MEMWATCH_MAKE_OPTS) $(MAKE) -C $(@D)
endef

define MEMWATCH_CLEAN_CMDS
	$(MEMWATCH_MAKE_OPTS) $(MAKE) -C $(@D) clean
endef

define MEMWATCH_INSTALL_STAGING_CMDS
	install -d $(STAGING_DIR)/usr/include $(STAGING_DIR)/usr/lib
	install -m 0644 $(@D)/memwatch.h $(STAGING_DIR)/usr/include
	install -m 0644 $(@D)/memwatch.o $(STAGING_DIR)/usr/lib
endef

define MEMWATCH_UNINSTALL_STAGING_CMDS
	rm -f $(STAGING_DIR)/usr/include/memwatch.h $(MEMWATCH_OBJ)
endef

$(eval $(call GENTARGETS))
