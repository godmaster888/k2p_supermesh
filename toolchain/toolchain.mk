TOOLCHAIN_VERSION = $(call qstrip,$(BR2_TOOLCHAIN_EXTERNAL_VERSION))
ifeq ($(TOOLCHAIN_VERSION),)
TOOLCHAIN_VERSION = master
endif

TOOLCHAIN_DIR=$(BASE_DIR)/toolchains

.PHONY: toolchain toolchain-source toolchain-extract

#18758
ifeq ($(BR2_TOOLCHAIN_USES_MUSL),y)
TARGET_CFLAGS += -DMUSL_LIBC
endif

#
# Definitions of the list of libraries that should be copied to the target.
#

TOOLCHAIN_EXTERNAL_LIBS += ld*.so.* libgcc_s.so.* libatomic.so.*
ifeq ($(BR2_TOOLCHAIN_USES_GLIBC)$(BR2_TOOLCHAIN_USES_UCLIBC),y)
TOOLCHAIN_EXTERNAL_LIBS += libc.so.* libcrypt.so.* libdl.so.* libm.so.* libnsl.so.* libresolv.so.* librt.so.* libutil.so.*
ifeq ($(BR2_TOOLCHAIN_HAS_THREADS),y)
TOOLCHAIN_EXTERNAL_LIBS += libpthread.so.*
endif # ! no threads
endif

ifeq ($(BR2_TOOLCHAIN_USES_UCLIBC),y)
TOOLCHAIN_EXTERNAL_LIBS += libubacktrace.so.*
endif

ifeq ($(BR2_TOOLCHAIN_USES_GLIBC),y)
TOOLCHAIN_EXTERNAL_LIBS += libnss_files.so.* libnss_dns.so.* libmvec.so.* libanl.so.*
endif

ifeq ($(BR2_TOOLCHAIN_USES_MUSL),y)
TOOLCHAIN_EXTERNAL_LIBS += libc.so
endif

ifeq ($(BR2_INSTALL_LIBSTDCPP),y)
TOOLCHAIN_EXTERNAL_LIBS += libstdc++.so.*
endif

TOOLCHAIN_EXTERNAL_LIBS += $(addsuffix .so*,$(call qstrip,$(BR2_TOOLCHAIN_EXTRA_LIBS)))

toolchain: $(TOOLCHAIN_DIR)/.stamp_installed
toolchain-source: $(TOOLCHAIN_DIR)/.stamp_downloaded
toolchain-extract: $(TOOLCHAIN_DIR)/.stamp_extracted

toolchain-clean-for-extract:
	@rm -f $(TOOLCHAIN_DIR)/.stamp_extracted

$(TOOLCHAIN_DIR)/.stamp_downloaded:
	@$(call MESSAGE,"Downloading toolchain")
	$(Q)git clone --depth 1 -b $(TOOLCHAIN_VERSION) git@rd:sdk_toolchains/$(BR2_TOOLCHAIN_EXTERNAL_LOCATION) $(BASE_DIR)/toolchains
	$(Q)touch $@

$(TOOLCHAIN_DIR)/.stamp_extracted: $(TOOLCHAIN_DIR)/.stamp_downloaded
	@$(call MESSAGE,"Extracting toolchain")
	$(Q)cp -aR $(TOOLCHAIN_DIR)/${BR2_USRINC_PATH}/* ${STAGING_DIR}/usr/include/
	$(Q)cp -aR $(TOOLCHAIN_DIR)/${BR2_LIB_PATH}/* ${STAGING_DIR}/lib/
ifneq ($(call qstrip,$(BR2_USRLIB_PATH)),)
	$(Q)cp -aR $(TOOLCHAIN_DIR)/${BR2_USRLIB_PATH}/* ${STAGING_DIR}/usr/lib/
endif
	$(Q)chmod -R +w ${STAGING_DIR}/usr/include
	$(Q)touch $@


$(TOOLCHAIN_DIR)/.stamp_installed: $(TOOLCHAIN_DIR)/.stamp_extracted
	$(Q)$(call MESSAGE,"Copying external toolchain libraries to target...")
	$(Q)for libpattern in $(TOOLCHAIN_EXTERNAL_LIBS); do \
		$(call copy_toolchain_lib_root,$$libpattern); \
	done
ifeq ($(BR2_TOOLCHAIN_USES_UCLIBC),y)
	# uClibc-ng dynamic loader is called ld-uClibc.so.1, but gcc is not
	# patched specifically for uClibc-ng, so it continues to generate
	# binaries that expect the dynamic loader to be named ld-uClibc.so.0,
	# like with the original uClibc. Therefore, we create an additional
	# symbolic link to make uClibc-ng systems work properly.
	$(Q)if test -e $(TARGET_DIR)/lib/ld-uClibc.so.1; then \
		ln -sf ld-uClibc.so.1 $(TARGET_DIR)/lib/ld-uClibc.so.0 ; \
	fi
	$(Q)if test -e $(TARGET_DIR)/lib/ld64-uClibc.so.1; then \
		ln -sf ld64-uClibc.so.1 $(TARGET_DIR)/lib/ld64-uClibc.so.0 ; \
	fi
endif
	$(Q)touch $@

include toolchain/helpers.mk
include toolchain/gdb/gdb.mk
