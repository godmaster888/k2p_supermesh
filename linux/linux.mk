LINUX26_DIR := $(BUILD_DIR)/kernel

# было в коде для broadcom, но использовалось внутри rt2880_app
export LINUXDIR := $(LINUX26_DIR)
LINUX_CONFIG  = $(LINUX26_DIR)/.config
STAGING_AUTOHEADER:= $(BR2_AUTOHEADER_DIR)/kernel_autoconf.h
LINUX_AUTOCONF_FILE = $(shell test -f $(LINUX26_DIR)/include/generated/autoconf.h && echo $(LINUX26_DIR)/include/generated/autoconf.h || echo $(LINUX26_DIR)/include/linux/autoconf.h)
LINUX_VERSION_FILE = $(shell test -f $(LINUX26_DIR)/include/generated/uapi/linux/version.h && echo $(LINUX26_DIR)/include/generated/uapi/linux/version.h || echo $(LINUX26_DIR)/include/linux/version.h)
export LINUX_CONFIG

include linux/addons/addons.mk

PROFILE_CFLAGS := $(call qstrip,$(BR2_PROFILE_CFLAGS))

PROFILE_CFLAGS += -DFW_DEV_ID_$(subst -,_,$(call qstrip,$(DLINK_DEVICEID)))

ifeq ($(BR2_ALPHA_FW),y)
	PROFILE_CFLAGS += -DALPHA_FW
endif

ifeq ($(BR2_WAN_PORT_NUM_0),y)
	PROFILE_CFLAGS += -DWAN_PORT_NUM_0
endif

ifeq ($(BR2_WAN_PORT_NUM_3),y)
	PROFILE_CFLAGS += -DWAN_PORT_NUM_3
endif

ifeq ($(BR2_ONE_LED_2G_5G),y)
	PROFILE_CFLAGS += -DONE_LED_2G_5G
endif

LINUX26_MAKE_FLAGS = \
	HOSTCC="$(HOSTCC)" \
	HOSTCFLAGS="$(HOSTCFLAGS)" \
	ARCH=$(KERNEL_ARCH) \
	INSTALL_MOD_PATH=$(TARGET_DIR) \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CXX=$(TARGET_CROSS)g++ \
	LZMA="$(LZMA)" \
	BUILD_DIR="$(BUILD_DIR)" \
	PROFILE="$(PROFILE)" \
	CUSTOM_CFLAGS="$(PROFILE_CFLAGS) $(BUILDCFLAGS)"

ifeq ($(BR2_DSYSINIT),y)
  BUILDCFLAGS += -DSDK_DSYSINIT
endif

ifeq ($(BR2_PACKAGE_GPIO_MODULE),y)
  BUILDCFLAGS += -DSUPPORT_GPIO_MODULE
  BUILDCFLAGS += -DGPIO_MODULE_PROFILE_$(call qstrip, $(BR2_PACKAGE_GPIO_MODULE_PROFILE))
endif

ifeq ($(BR2_SUPPORT_TRAFFIC_SEGMENTATION),y)
  BUILDCFLAGS += -DSUPPORT_TRAFFIC_SEGMENTATION
endif

ifeq ($(BR2_SUPPORT_PORT_UTILIZATION),y)
  BUILDCFLAGS += -DSUPPORT_PORT_UTILIZATION
endif

ifeq ($(BR2_SUPPORT_WIFI),y)
# New channels for Russia added in 5 GHz
BUILDCFLAGS += -DCHANNEL_EXTENSION_5GHZ
# Autochannel
ifeq ($(BR2_WIFI_DRU_AUTO_CHANNEL),y)
BUILDCFLAGS += -DBRD_WIFI_AUTOCHANNEL
endif
endif

ifeq ($(BR2_rlx),y)
ifeq ($(BR2_DUAL_BOOT_FW),y)
  BUILDCFLAGS += -DCONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE
endif
LINUX26_DEPENDENCIES+=host-cvimg
endif

ifeq ($(BR2_bcm),y)
LINUX26_DEPENDENCIES+=bcm_support bcm_shared
BUILDCFLAGS += -I$(STAGING_DIR)/include/bcm963xx/ -I$(STAGING_DIR)/include/ -I$(STAGING_DIR)/usr/share/bcm_shared/opensource/include/drv/phys/
BUILDCFLAGS += -I$(STAGING_DIR)/usr/share/bcm_shared/opensource/include/drv/mdio/ -I$(STAGING_DIR)/usr/share/bcm_shared/opensource/include/pmc/ -I$(STAGING_DIR)/usr/share/bcm_shared/opensource/include/ -I$(STAGING_DIR)/usr/share/bcm_shared/opensource/include/bcm963xx/ -I$(STAGING_DIR)/usr/share/bcm_shared/broadcom/include/bcm963xx/ -I$(STAGING_DIR)/usr/share/bcm_shared/opensource/flash/
LINUX26_MAKE_FLAGS += BCM_KF=1 BRCMDRIVERS_DIR=$(BUILD_DIR)/bcm_support-master INC_BCMDRIVER_PATH=$(STAGING_DIR)
LINUX26_MAKE_FLAGS += BRCM_CHIP=63178 KERNEL_DIR=$(LINUX26_DIR) BRCM_BOARD=bcm963xx
LINUX26_MAKE_FLAGS += INC_BRCMBOARDPARMS_PATH=$(STAGING_DIR)/usr/share/bcm_shared/opensource/boardparms
LINUX26_MAKE_FLAGS += INC_SPI_PATH=$(STAGING_DIR)/usr/share/bcm_shared/opensource/spi
LINUX26_MAKE_FLAGS += INC_FLASH_PATH=$(STAGING_DIR)/usr/share/bcm_shared/opensource/flash
LINUX26_MAKE_FLAGS += SHARED_DIR=$(STAGING_DIR)/usr/share/bcm_shared
LINUX26_MAKE_FLAGS += INC_UTILS_PATH=$(STAGING_DIR)/usr/share/bcm_shared/opensource/utils
LINUX26_MAKE_FLAGS += BCA_HNDROUTER=1 HNDDRIVER_BASE="main/components/router" DTS_DIR=$(LINUX26_DIR)/arch/arm/boot/bcmdts
LINUX26_MAKE_FLAGS += BRCM_VERSION='5' BRCM_RELEASE='02' ORIG_PROFILE_ARCH=arm  BCA_CPEROUTER=y BUILD_BRCM_HNDROUTER=y INSTALL_DIR=$(TARGET_DIR)

endif

ifeq ($(BR2_ralink),y)
LINUX26_DEPENDENCIES+=host-lzma
ifeq (${BR2_SUPPORT_WIFI},y)
LINUX26_DEPENDENCIES+=mediatek_wifi
endif
BUILDCFLAGS += -I$(LINUX26_DIR)/net/nat/hw_nat
endif

# see comments in rtk_ms_sdk.mk
LINUX26_DEPENDENCIES += $(if $(BR2_rtk_ms),rtk_ms_sdk,)

ifeq ($(BR2_SUPPORT_SLA_AGENT),y)
	BUILDCFLAGS += -DDLINK_SLA_AGENT
endif

ifeq ($(BR2_LINUX_KERNEL_UIMAGE),y)
LINUX26_IMAGE_NAME=uImage
LINUX26_DEPENDENCIES += host-uboot-tools host-lzma
else ifeq ($(BR2_LINUX_KERNEL_BZIMAGE),y)
LINUX26_IMAGE_NAME=bzImage
else ifeq ($(BR2_LINUX_KERNEL_ZIMAGE),y)
LINUX26_IMAGE_NAME=zImage
else ifeq ($(BR2_LINUX_KERNEL_VMLINUX_BIN),y)
LINUX26_IMAGE_NAME=vmlinux.bin
else ifeq ($(BR2_LINUX_KERNEL_LZIMAGE),y)
LINUX26_IMAGE_NAME=vmlinux.lzma
else ifeq ($(BR2_LINUX_KERNEL_VMLINUX),y)
LINUX26_IMAGE_NAME=vmlinux
endif

ifeq ($(BR2_LINUX_KERNEL_WAR_UIMAGE),y)
LINUX26_DEPENDENCIES += host-uboot-tools host-lzma
endif


ifeq ($(DEVELOP)|$(GETRELEASE)|$(KERNEL_COMMIT),y||)
KERNEL_REPO_DEPTH = --depth 10
else
# autobuild is always full to get 'git reset' possible
KERNEL_REPO_DEPTH =
endif

# In-tree path to the kernel config
KERNEL_CONFIG_PATH =

KERNEL_CONFIG_DEST = $(LINUX26_DIR)/.config

LINUX26_IMAGE_PATH=$(LINUX26_DIR)/arch/$(KERNEL_ARCH)/boot/$(LINUX26_IMAGE_NAME)

LINUX_KERNEL_CUSTOM_BRANCH := $(call qstrip,$(BR2_LINUX_KERNEL_CUSTOM_BRANCH))
LINUX_KERNEL_CUSTOM_REPO := git@rd:$(call qstrip,$(BR2_LINUX_KERNEL_CUSTOM_REPO))

ifeq ($(BR2_BUILD_GPL),y)
define DISABLE_LOCALVERSION
  @$(call KCONFIG_SET_OPT,LOCALVERSION,,$(LINUX_CONFIG))
  @$(call KCONFIG_DISABLE_OPT,CONFIG_LOCALVERSION_AUTO,$(LINUX_CONFIG))
  @touch $(LINUX26_DIR)/.scmversion
endef
LINUX_ADDONS += DISABLE_LOCALVERSION
endif

# Download
$(LINUX26_DIR)/.stamp_downloaded:
	@$(call MESSAGE,"Downloading kernel")
	$(Q)git clone $(KERNEL_REPO_DEPTH) -b $(LINUX_KERNEL_CUSTOM_BRANCH) $(LINUX_KERNEL_CUSTOM_REPO) $(LINUX26_DIR)
	@if ! grep -q "patch = " $(LINUX26_DIR)/.git/config ; then \
		echo -e "[alias]\\n\\tpatch = \"!sh $(TOPDIR)/scripts/patch.sh\"" >> $(LINUX26_DIR)/.git/config; \
	fi
ifeq (${GETRELEASE},)
	@echo "using current kernel HEAD ..."
else ifneq ("$(KERNEL_COMMIT)",)
	@echo reset to commit $(KERNEL_COMMIT)
	cd $(LINUX26_DIR); git reset --hard $(KERNEL_COMMIT);
else
	@echo "Error! GETRELEASE is set but neither KERNEL_COMMIT is specified"
	@false
endif
	$(Q)touch $@

# Configuration
$(LINUX26_DIR)/.stamp_configured: $(LINUX26_DIR)/.stamp_downloaded
	@$(call MESSAGE,"Configuring kernel")
	$(Q)if [ -f $(PROFILE_DIR)/$(BR2_LINUX_KERNEL_CUSTOM_CONFIG_FILE) ]; then                                 \
		cp -v $(PROFILE_DIR)/$(BR2_LINUX_KERNEL_CUSTOM_CONFIG_FILE)                   $(KERNEL_CONFIG_DEST);  \
	elif [ -f $(LINUX26_DIR)/$(KERNEL_CONFIG_PATH)/config_$(PROFILE) ]; then                                  \
		cp -v $(LINUX26_DIR)/$(KERNEL_CONFIG_PATH)/config_$(PROFILE)                   $(KERNEL_CONFIG_DEST); \
	elif [ -f $(LINUX26_DIR)/$(KERNEL_CONFIG_PATH)/config_$(BR2_DLINK_GENERIC_PROFILE) ]; then                \
		cp -v $(LINUX26_DIR)/$(KERNEL_CONFIG_PATH)/config_$(BR2_DLINK_GENERIC_PROFILE) $(KERNEL_CONFIG_DEST); \
	else                                                                                                      \
		$(call MESSAGE, "WARNING: using default in-tree kernel .config");                                     \
	fi

ifeq ($(BR2_rlx),y)
	@ln -sf bsp_${RTL_CHIP} $(LINUX26_DIR)/arch/$(KERNEL_ARCH)/bsp
ifeq ($(BR2_RTK_SDK_RETAIL),y)
	@ln -snf rtl8192cd_3414b $(LINUX26_DIR)/drivers/net/wireless/rtl8192cd
endif

ifeq ($(RLX_86XX),y)
	cp -v $(LINUX26_DIR)/appendix_RTL8685PB.h $(LINUX26_DIR)/appendix.h;
ifeq ($(BR2_RTL8192CD_VER_405),y)
	@ln -snf rtl8192cd_405 $(LINUX26_DIR)/drivers/net/wireless/rtl8192cd
else ifeq ($(BR2_RTL8192CD_VER_406),y)
	@ln -snf rtl8192cd_406 $(LINUX26_DIR)/drivers/net/wireless/rtl8192cd
endif # BR2_RTL8192CD_VER_XXX
endif # RLX_86XX
endif # BR2_rlx
	$(Q)$(foreach addon,$(LINUX_ADDONS),$(call $(addon))$(sep))
	$(Q)touch $@

# Compilation. We make sure the kernel gets rebuilt when the
# configuration has changed.

$(LINUX26_DIR)/.stamp_compiled: $(LINUX26_DIR)/.stamp_configured $(LINUX26_DIR)/.config
	@$(call MESSAGE,"Compiling kernel")
	$(TARGET_MAKE_ENV) TARGET_DIR=${TARGET_DIR} $(MAKE) $(LINUX26_MAKE_FLAGS) -C $(@D) $(LINUX26_IMAGE_NAME) V=99


ifeq ($(BR2_rlx),y)
ifneq ($(RTL8685PB),)
	${MAKE1} -C $(LINUX26_DIR) vmimg CROSS_COMPILE=$(TARGET_CROSS)
ifeq ($(RTL8685PB)$(BR2_NAND),yy)
	cp $(LINUX26_DIR)/uImage $(LINUX26_IMAGE_PATH)
else
	cp $(LINUX26_DIR)/vmlinux.lzma $(LINUX26_IMAGE_PATH)
endif

ifneq ($(RTL8685PB),)
	cp $(LINUX26_DIR)/genhead $(BINARIES_DIR)
endif
else
ifeq ($(BR2_ALPHA_FW),y)
# для BR2_ALPHA_FW:
# для 816,879 - загрузчик сам распаковывае lzma с ядром
# для 850,822 - используется стандартный риалтековский распаковщик
ifeq ($(PROFILE),$(filter $(PROFILE),DIR_816LB1A_ALPHA DIR_879A1_ALPHA))
#   жмем ядро
	lzma -zfv $(LINUX26_IMAGE_PATH)
	mv $(LINUX26_IMAGE_PATH).lzma $(LINUX26_IMAGE_PATH)
else
ifeq ($(PROFILE),$(filter $(PROFILE),DIR_850L_ALPHA DIR_822C1A_ALPHA DIR_809A1A_ALPHA))
	CROSS_COMPILE=$(TARGET_CROSS) HOST_DIR=$(HOST_DIR) ${MAKE1} -C $(LINUX26_DIR)/rtkload ARCH=$(KERNEL_ARCH)
else
	$(error Pls, check image format!!!)
endif
endif
else
	CROSS_COMPILE=$(TARGET_CROSS) HOST_DIR=$(HOST_DIR) ${MAKE1} -C $(LINUX26_DIR)/rtkload ARCH=$(KERNEL_ARCH)
endif
endif
endif

	@if [ $(shell grep -c "CONFIG_MODULES=y" $(LINUX26_DIR)/.config) != 0 ] ; then 	\
		$(TARGET_MAKE_ENV) $(MAKE) $(LINUX26_MAKE_FLAGS) -C $(@D) modules ;	\
	fi
ifeq ($(BR2_bcm),y)
	@$(call MESSAGE,"Compiling dtc")
	$(TARGET_MAKE_ENV) TARGET_DIR=${TARGET_DIR} $(MAKE) $(LINUX26_MAKE_FLAGS) -C $(@D) boot=$(LINUX26_DIR)/arch/arm/boot/bcmdts dtbs V=99
endif

	$(Q)touch $@

# Генерим общий файл депмода
# TODO: заполнять этот список только теми
# дирами, в которых собираются сторонние модули,
# чтобы не искать во всём output/build (долго)
LINUX26_SYMVERS_DIRS += $(O)/build/kernel
LINUX26_SYMVERS_ALL  = $(O)/Module.symvers.all

# Определяем назначение установки модулей ядра
# Теперь есть несколько способов
# - "стандарт"   - установка дерева в /lib/modules/$(uname -r)
# - "упрощенный" - установка дерева в /lib/modules/
# - "классический костыль" - установка всех кучи модулей в /lib/modules/ как есть
#
# Первый способ используется стоковыми ядрами (точнее, host-kmod).
# В папке /lib/modules появляется директория с именем версии ядра, в которой
# располагаются еще 3 директории (kernel, build, source) и дополнительные файлы
# (modules.dep, modules.builtin, modules.order, ...).
#
# Второй способ работает как первый, только пропускается имя версии ядра.
# Для встраиваемых систем нет нужды в проверке версии ядра, так как оно там
# фактически всегда одно, и набор модулей всегда соответствует загруженному ядру.
# Ну и это полезно, когда идет постоянная разработка и частые коммиты в ядро.
#
# Третий способ - ядра, используемые совместно со старым SDK - там каждое ядро
# было специально хакнуто, чтобы все модули устанавливались в /lib/modules,
# причем все модули лежат в корне директории. В таком случае modprobe может быть
# закостылен с использованием скрипта-обертки над insmod.
#
# Если в SDK добавляется новое ядро, то нужно использовать способ 1 или 2.
# Если ядро перекочевало из старого SDK - то скорее всего придется использовать
# способ 3.
#
LINUX26_MODULE_DIRTYPE = flat
LINUX_MODULE_DIR     = $(TARGET_DIR)/lib/modules/$(LINUX26_RELEASE)/extra/
LINUX26_MODULE_DIR   = $(LINUX_MODULE_DIR)
LINUX26_DEPENDENCIES += host-kmod

ifeq ($(BR2_LINUX_KERNEL_MODULES_TREE),y)
LINUX26_MODULE_DIRTYPE = tree
endif

LINUX26_RELEASE        = $(shell $(MAKE) $(LINUX26_MAKE_FLAGS) -C $(LINUX26_DIR) --no-print-directory -s kernelrelease 2>/dev/null)

ENTRY = $(shell LC_ALL=C readelf -h $(LINUX26_DIR)/vmlinux|grep "Entry"|cut -d":" -f 2)
LDADDR = $(shell readelf -a $(LINUX26_DIR)/vmlinux|grep "\[ 1\]"|cut -d" " -f 26)
ifneq ($(subst ",,$(DLINK_DEVICEID)),)
DEVICE_ID = $(DLINK_DEVICEID)
else
DEVICE_ID = $(PROFILE)
endif

depmod: host-kmod
	$(Q)DEPMOD="$(DEPMOD)" STRIP="$(KSTRIPCMD)" $(TOPDIR)/scripts/depmod.sh $(LINUX26_DIR) $(TARGET_DIR) $(LINUX26_RELEASE)

# will be added to TARGETS in sdk/Makefile
target-finalize: depmod

# Installation
$(LINUX26_DIR)/.stamp_installed: $(LINUX26_DIR)/.stamp_compiled
	@$(call MESSAGE,"Installing kernel")
	cp $(LINUX26_IMAGE_PATH) $(BINARIES_DIR)
ifeq ($(BR2_LINUX_KERNEL_WAR_UIMAGE),y)
	$(LZMA) e $(BINARIES_DIR)/$(LINUX26_IMAGE_NAME) $(BINARIES_DIR)/$(LINUX26_IMAGE_NAME).lzma 2>&1
	$(MKIMAGE) -A mips -O linux -T kernel -C lzma -a 0x$(LDADDR) -e $(ENTRY) -n ${DEVICE_ID}    \
	-d $(BINARIES_DIR)/$(LINUX26_IMAGE_NAME).lzma $(BINARIES_DIR)/uImage
endif
# Install modules and remove symbolic links pointing to build
# directories, not relevant on the target
	@if [ $(shell grep -c "CONFIG_MODULES=y" $(LINUX26_DIR)/.config) != 0 ] ; then 	\
		echo "Installing modules ..."; \
		$(TARGET_MAKE_ENV) $(MAKE1) $(LINUX26_MAKE_FLAGS) -C $(@D) 		\
			INSTALL_MOD_PATH=$(TARGET_DIR) DEPMOD=$(TOPDIR)/scripts/depmod.pl modules_install ;		\
		rm -rf $(TARGET_DIR)/lib/modules/../build ;	\
		rm -rf $(TARGET_DIR)/lib/modules/../source ;	\
	fi
ifeq ($(BR2_LINUX_KERNEL_INSTALL_SYSTEM_MAP),y)
	@echo "Installing System.map ..."
	@cp -f $(@D)/System.map $(TARGET_DIR)
endif

# Придется здесь явно инсталлить хедеры netlink monitor, потому что все равно
# у нас не вызывается цель headers_install.
# Пропишем здесь установку хедеров вне зависимости, есть они или нет.
	@cp -r $(LINUX26_DIR)/include/net/nlmon/ $(STAGING_DIR)/usr/include/ 2> /dev/null || true
	$(Q)touch $@

# Установка автохедеров. Необходимо для d-link_libs.
	@mkdir -p $(BR2_AUTOHEADER_DIR)
	@echo -e "#ifndef BR2_KERNEL_AUTOHEADER_H" > $(STAGING_AUTOHEADER)
	@echo -e "#define BR2_KERNEL_AUTOHEADER_H" >> $(STAGING_AUTOHEADER)
	@cat $(LINUX_AUTOCONF_FILE) | sed 's/CONFIG_/KERNEL_CONFIG_/g' >> $(STAGING_AUTOHEADER)
	@cat $(LINUX_VERSION_FILE)  >> $(STAGING_AUTOHEADER)
	@echo >> $(STAGING_AUTOHEADER)
	@echo -e "#endif /* end of include guard: BR2_KERNEL_AUTOHEADER_H */" >> $(STAGING_AUTOHEADER)

# Support for rebuilding the kernel after the cpio archive has
# been generated in $(BINARIES_DIR)/rootfs.cpio.
$(LINUX26_DIR)/.stamp_initramfs_rebuilt: $(LINUX26_DIR)/.stamp_target_installed $(LINUX26_DIR)/.stamp_images_installed $(BINARIES_DIR)/rootfs.cpio
	@$(call MESSAGE,"Rebuilding kernel with initramfs")
	# Build the kernel.
	$(TARGET_MAKE_ENV) $(MAKE) $(LINUX26_MAKE_FLAGS) -C $(@D) $(LINUX26_IMAGE_NAME)
	# Copy the kernel image to its final destination
	cp $(LINUX26_IMAGE_PATH) $(BINARIES_DIR)
	$(Q)touch $@

# The initramfs building code must make sure this target gets called
# after it generated the initramfs list of files.
linux-rebuild-with-initramfs linux26-rebuild-with-initramfs: $(LINUX26_DIR)/.stamp_initramfs_rebuilt

all: kernel

kernel-download: $(LINUX26_DIR)/.stamp_downloaded

# download alias
kernel-source:   kernel-download
linux26-source:  kernel-download

kernel-configure: $(LINUX26_DIR)/.stamp_configured

kernel-clean-for-reconfigure: kernel-clean-for-rebuild
	rm -f $(LINUX26_DIR)/.stamp_configured

kernel-reconfigure: kernel-clean-for-reconfigure kernel-configure

kernel-menuconfig: kernel-configure
	$(MAKE1) $(LINUX26_MAKE_FLAGS) -C $(LINUX26_DIR) menuconfig
	rm -f $(LINUX26_DIR)/.stamp_{compiled,installed}

kernel-oldconfig: kernel-configure
	$(MAKE1) $(LINUX26_MAKE_FLAGS) -C $(LINUX26_DIR) oldconfig
	rm -f $(LINUX26_DIR)/.stamp_{compiled,installed}

kernel-build: $(LINUX26_DIR)/.stamp_compiled

kernel-clean-for-rebuild:
	rm -f $(LINUX26_DIR)/.stamp_{compiled,installed}

kernel-rebuild: kernel-clean-for-rebuild kernel

kernel-install: $(LINUX26_DIR)/.stamp_installed

kernel-clean: kernel-configure
	@$(call MESSAGE,"Cleaning kernel")
	$(MAKE1) $(LINUX26_MAKE_FLAGS) -C $(LINUX26_DIR) clean
	rm -rf $(LINUX26_DIR)/.stamp_{compiled,installed}

kernel: $(LINUX26_DEPENDENCIES) kernel-install

linux26: kernel

ifeq ($(BR2_LINUX_KERNEL),y)
TARGETS += linux26
endif
