#############################################################
#
# ebtables
#
#############################################################

EBTABLES_VERSION = $(call qstrip,$(BR2_EBTABLES_BRANCH))
ifeq (${EBTABLES_VERSION},)
EBTABLES_VERSION = master
endif
EBTABLES_SITE =  $(DLINK_GIT_STORAGE)/ebtables
EBTABLES_LICENSE = GPL-2.0-or-later
EBTABLES_LICENSE_FILES = COPYING

# farisey: #10463
EBT_HAS_TABLES = broute filter nat
EBT_HAS_MODS = 802_3 among arp arpreply ip6 ip limit log mark mark_m nat nflog pkttype redirect standard stp ulog vlan

ifeq ($(BR2_EBTABLES_CUSTOMIZE),y)
EBT_OPTS += BR2_EBTABLES_CUSTOMIZE=y
endif

EBT_ALL_TABLES += $(foreach x,$(EBT_HAS_TABLES),$(shell echo BR2_EBTABLES_TABLE_$(x) | tr [:lower:] [:upper:])+$(x))
EBT_ALL_MODS += $(foreach x,$(EBT_HAS_MODS),$(shell echo BR2_EBTABLES_MOD_$(x) | tr [:lower:] [:upper:])+$(x))

define EBT_ADD_TABLE
ifeq ($($1),y)
EBT_OPTS += $(1)=y
EBT_EXT_TABLES += $(2)
endif
endef

define EBT_ADD_MOD
ifeq ($($1),y)
EBT_OPTS += $(1)=y
EBT_EXT_MODS += $(2)
endif
endef

$(foreach x,$(EBT_ALL_TABLES),$(eval $(call EBT_ADD_TABLE,$(word 1,$(subst +, ,$(x))),$(word 2,$(subst +, ,$(x))))))
$(foreach x,$(EBT_ALL_MODS),$(eval $(call EBT_ADD_MOD,$(word 1,$(subst +, ,$(x))),$(word 2,$(subst +, ,$(x))))))

define EBTABLES_BUILD_CMDS
	$(EBT_OPTS) EBT_EXT_TABLES="$(EBT_EXT_TABLES)" EBT_EXT_MODS="$(EBT_EXT_MODS)" \
	  $(MAKE1) $(TARGET_CONFIGURE_OPTS) LIBDIR=/lib/ebtables -C $(@D)
endef

define EBTABLES_INSTALL_TARGET_CMDS
	for so in $(@D)/$(EBTABLES_SUBDIR)/*.so \
		$(@D)/$(EBTABLES_SUBDIR)/extensions/*.so; \
		do \
		$(INSTALL) -m 0755 -D $${so} \
			$(TARGET_DIR)/lib/`basename $${so}`; \
	done
	$(INSTALL) -m 0755 -D $(@D)/$(EBTABLES_SUBDIR)/ebtables \
		$(TARGET_DIR)/sbin/ebtables
endef

define EBTABLES_UNINSTALL_TARGET_CMDS
	rm -rf $(TARGET_DIR)/lib/ebtables
	rm -f $(TARGET_DIR)/sbin/ebtables
endef

define EBTABLES_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

$(eval $(call GENTARGETS))
