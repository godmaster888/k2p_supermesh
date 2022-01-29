######################################################################
#
# gdb
#
######################################################################
GDB_VERSION:=$(call qstrip,$(BR2_GDB_VERSION))

GDB_SOURCE:=gdb-$(GDB_VERSION).tar.bz2
GDB_CAT:=$(BZCAT)

GDB_SITE:=$(DLINK_STORAGE)
GDB_PATCH_DIR:=toolchain/gdb/$(GDB_VERSION)

ifneq ($(BR2_rlx),)
  GDB_PATCH_DIR:=toolchain/gdb/$(GDB_VERSION)_rlx
endif

ifneq ($(BR2_rlx_9607),)
  GDB_PATCH_DIR:=toolchain/gdb/$(GDB_VERSION)_rlx
endif

GDB_DIR:=$(TOOLCHAIN_DIR)/gdb-$(GDB_VERSION)

$(GDB_DIR)/.stamp_downloaded: $(TOOLCHAIN_DIR)/.stamp_downloaded
	@$(call MESSAGE,"gdb $(GDB_VERSION) Downloading")
	$(Q)mkdir -p $(GDB_DIR)
	$(Q)git clone -b $(GDB_VERSION) $(DLINK_GIT_STORAGE)/gdb $(GDB_DIR)
	@touch $@

$(GDB_DIR)/.stamp_extracted: $(GDB_DIR)/.stamp_downloaded
	@$(call MESSAGE,"gdb $(GDB_VERSION) Extracting")
	@$(call CONFIG_UPDATE,$(@D))
	@touch $@


$(GDB_DIR)/.stamp_patched: $(GDB_DIR)/.stamp_extracted
	@$(call MESSAGE,"gdb $(GDB_VERSION) Patching")
ifneq ($(wildcard $(GDB_PATCH_DIR)),)
	$(Q)toolchain/patch-kernel.sh $(GDB_DIR) $(GDB_PATCH_DIR) \*.patch $(GDB_PATCH_EXTRA)
endif
	@touch $@

gdb-dirclean:
	@$(call MESSAGE,"gdb $(GDB_VERSION) Distclean")
	$(Q)rm -rf $(GDB_DIR)

######################################################################
#
# gdb target
#
######################################################################


GDB_TARGET_DIR:=$(BUILD_DIR)/gdb-$(GDB_VERSION)-target

GDB_TARGET_CONFIGURE_VARS:= \
	ac_cv_type_uintptr_t=yes \
	gt_cv_func_gettext_libintl=yes \
	ac_cv_func_dcgettext=yes \
	gdb_cv_func_sigsetjmp=yes \
	bash_cv_func_strcoll_broken=no \
	bash_cv_must_reinstall_sighandlers=no \
	bash_cv_func_sigsetjmp=present \
	bash_cv_have_mbstate_t=yes


$(GDB_TARGET_DIR)/.stamp_configured: $(GDB_DIR)/.stamp_patched
	@$(call MESSAGE,"gdb_target $(GDB_VERSION) Configured")
	$(Q)mkdir -p $(GDB_TARGET_DIR)
	$(Q)(cd $(GDB_TARGET_DIR); \
		gdb_cv_func_sigsetjmp=yes \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS_FOR_TARGET="$(TARGET_CFLAGS) $(TARGET_LDFLAGS) -Wno-error" \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_LDFLAGS) -Wno-error" \
		$(GDB_TARGET_CONFIGURE_VARS) \
		$(GDB_DIR)/configure $(QUIET) \
		--cache-file=/dev/null \
		--build=$(GNU_HOST_NAME) \
		--host=$(REAL_GNU_TARGET_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--prefix=/usr \
		$(DISABLE_NLS) \
		--without-uiout $(DISABLE_GDBMI) \
		--disable-tui --disable-gdbtk --without-x \
		--with-system-gdbinit=/etc/gdbinit \
		--disable-sim --enable-gdbserver \
		--without-included-gettext \
		--disable-werror \
		$(QUIET) \
	)
ifeq ($(BR2_ENABLE_LOCALE),y)
	-$(SED) "s,^INTL *=.*,INTL = -lintl,g;" $(GDB_DIR)/gdb/Makefile
endif
	@touch $@

$(GDB_TARGET_DIR)/.stamp_built: $(GDB_TARGET_DIR)/.stamp_configured
	@$(call MESSAGE,"gdb_target $(GDB_VERSION) Build")
	# force ELF support since it fails due to BFD linking problems
	gdb_cv_var_elf=yes \
	$(TARGET_MAKE_ENV) \
	$(MAKE) CC="$(TARGET_CC)" MT_CFLAGS="$(TARGET_CFLAGS)" \
		-C $(GDB_TARGET_DIR)
	@touch $@

$(GDB_TARGET_DIR)/.stamp_target_installed: $(GDB_TARGET_DIR)/.stamp_built
	@$(call MESSAGE,"gdb_target $(GDB_VERSION) Install")
	$(Q)install -c -D $(GDB_TARGET_DIR)/gdb/gdb $(TARGET_DIR)/usr/bin/gdb
	$(Q)echo "set substitute-path $(BUILD_DIR) /mnt" > $(TARGET_DIR)/etc/gdbinit
	@touch $@

.PHONY: gdb_target
gdb_target: ncurses $(GDB_TARGET_DIR)/.stamp_target_installed

gdb_target-source: $(GDB_DIR)/.stamp_downloaded

gdb_target-clean:
	@$(call MESSAGE,"gdb_target $(GDB_VERSION) Clean")
	-$(MAKE) -C $(GDB_DIR) clean

gdb_target-dirclean:
	@$(call MESSAGE,"gdb_target $(GDB_VERSION) Distclean")
	rm -rf $(GDB_DIR)

######################################################################
#
# gdbserver
#
######################################################################

GDB_SERVER_DIR:=$(BUILD_DIR)/gdbserver-$(GDB_VERSION)

$(GDB_SERVER_DIR)/.stamp_configured: $(GDB_DIR)/.stamp_extracted
	@$(call MESSAGE,"gdbserver $(GDB_VERSION) Configured")
	mkdir -p $(GDB_SERVER_DIR)
	(cd $(GDB_SERVER_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		gdb_cv_func_sigsetjmp=yes \
		bash_cv_have_mbstate_t=yes \
		$(GDB_DIR)/gdb/gdbserver/configure $(QUIET) \
		--cache-file=/dev/null \
		--build=$(GNU_HOST_NAME) \
		--host=$(REAL_GNU_TARGET_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		--includedir=$(STAGING_DIR)/usr/include \
		$(DISABLE_NLS) \
		--without-uiout $(DISABLE_GDBMI) \
		--disable-tui --disable-gdbtk --without-x \
		--without-included-gettext \
	)
	touch $@

$(GDB_SERVER_DIR)/gdbserver: $(GDB_SERVER_DIR)/.configured
	@$(call MESSAGE,"gdbserver $(GDB_VERSION) Build")
	$(MAKE) CC="$(TARGET_CC)" MT_CFLAGS="$(TARGET_CFLAGS)" \
		-C $(GDB_SERVER_DIR)

$(TARGET_DIR)/usr/bin/gdbserver: $(GDB_SERVER_DIR)/gdbserver
	@$(call MESSAGE,"gdbserver $(GDB_VERSION) Install")
ifeq ($(BR2_CROSS_TOOLCHAIN_TARGET_UTILS),y)
	mkdir -p $(STAGING_DIR)/usr/$(REAL_GNU_TARGET_NAME)/target_utils
	install -c $(GDB_SERVER_DIR)/gdbserver \
		$(STAGING_DIR)/usr/$(REAL_GNU_TARGET_NAME)/target_utils/gdbserver
endif
	install -c -D $(GDB_SERVER_DIR)/gdbserver $(TARGET_DIR)/usr/bin/gdbserver

gdbserver: $(TARGET_DIR)/usr/bin/gdbserver

gdbserver-source: $(GDB_DIR)/.stamp_downloaded

gdbserver-clean:
	@$(call MESSAGE,"gdbserver $(GDB_VERSION) Clean")
	-$(MAKE) -C $(GDB_SERVER_DIR) clean

gdbserver-dirclean:
	@$(call MESSAGE,"gdbserver $(GDB_VERSION) Distclean")
	rm -rf $(GDB_SERVER_DIR)

######################################################################
#
# gdb on host
#
######################################################################

GDB_HOST_DIR:=$(TOOLCHAIN_DIR)/gdbhost-$(GDB_VERSION)

$(GDB_HOST_DIR)/.configured: $(GDB_DIR)/.extracted
	@$(call MESSAGE,"gdbhost $(GDB_VERSION) Configured")
	mkdir -p $(GDB_HOST_DIR)
	(cd $(GDB_HOST_DIR); \
		gdb_cv_func_sigsetjmp=yes \
		bash_cv_have_mbstate_t=yes \
		$(GDB_DIR)/configure $(QUIET) \
		$(HOST_CONFIGURE_OPTS) \
		--cache-file=/dev/null \
		--prefix=$(STAGING_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		$(DISABLE_NLS) \
		--without-uiout $(DISABLE_GDBMI) \
		--disable-tui --disable-gdbtk --without-x \
		--without-included-gettext \
		--enable-threads \
		--disable-werror \
	)
	touch $@

$(GDB_HOST_DIR)/gdb/gdb: $(GDB_HOST_DIR)/.configured
	@$(call MESSAGE,"gdbhost $(GDB_VERSION) Build")
	# force ELF support since it fails due to BFD linking problems
	gdb_cv_var_elf=yes \
	$(MAKE) -C $(GDB_HOST_DIR)
	strip $(GDB_HOST_DIR)/gdb/gdb

$(TARGET_CROSS)gdb: $(GDB_HOST_DIR)/gdb/gdb
	@$(call MESSAGE,"gdbhost $(GDB_VERSION) Install")
	install -c $(GDB_HOST_DIR)/gdb/gdb $(TARGET_CROSS)gdb
#	ln -snf ../../bin/$(REAL_GNU_TARGET_NAME)-gdb \
#		$(STAGING_DIR)/usr/$(REAL_GNU_TARGET_NAME)/bin/gdb
	ln -snf $(REAL_GNU_TARGET_NAME)-gdb \
		$(HOST_DIR)/usr/bin/$(GNU_TARGET_NAME)-gdb

gdbhost: host-expat $(TARGET_CROSS)gdb

gdbhost-source: $(GDB_DIR)/.stamp_downloaded

gdbhost-clean:
	@$(call MESSAGE,"gdbhost $(GDB_VERSION) Clean")
	-$(MAKE) -C $(GDB_HOST_DIR) clean

gdbhost-dirclean:
	@$(call MESSAGE,"gdbhost $(GDB_VERSION) Distclean")
	rm -rf $(GDB_HOST_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(BR2_PACKAGE_GDB),y)
TARGETS+=gdb_target
endif

ifeq ($(BR2_PACKAGE_GDB_SERVER),y)
TARGETS+=gdbserver
endif

ifeq ($(BR2_PACKAGE_GDB_HOST),y)
TARGETS+=gdbhost
endif
