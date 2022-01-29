TARGETS += pack_image

ifeq ($(DEVELOP),y)
	TARGETS += link_image
endif

ifneq ($(strip $(BR2_PARTS_FILE)),"")
    TARGETS += pack_dlinkhdr_image
endif

# usage: CHECK_FLASH_SIZE,<firmware_offset>
define CHECK_FLASH_SIZE
	@{ \
	flash_fw_offset=$$(printf "%d\n" "$(1)"); \
	current_fw_size=$$(stat -L -c %s $(BINARIES_DIR)/$(FW_IMAGE_NAME)); \
	flash_fw_diff=$$(echo "($$flash_fw_offset + $$current_fw_size) - $(BR2_FLASH_SIZE)" | bc); \
	if [ "$$flash_fw_diff" -gt "0" ]; then \
		$(call MESSAGE,"Firmware size is too large: $$(echo "$$flash_fw_diff/1024+1" | bc)Kb"); \
		exit 1; \
	else \
		$(call MESSAGE,"Firmware size is correct. Reserve: $$(echo "$$flash_fw_diff/-1024" | bc)Kb"); \
	fi; \
	}
endef

SIGN_PROGRAM := $(HOST_DIR)/usr/bin/mksign
SIGN_FILE    := $(if $(ROLLBACK),$(ROLLBACK_FILE),$(PROFILE_FILE))
SIGN_DEFAULT_CMD = $(SIGN_PROGRAM) sign $(BINARIES_DIR)/$(FW_IMAGE_NAME) $(SIGN_FILE)

ADD_DATAMODEL_VERSION = $(TOPDIR)/scripts/add_version.py $(STAGING_DIR)/etc/dlink/dm_version.dm $(BINARIES_DIR)/$(FW_IMAGE_NAME)

TARGET_ARCH_DIR:=

ifneq ($(BR2_ar7240),)
    TARGET_ARCH_DIR = target/ar7240
else ifneq ($(BR2_qca),)
    TARGET_ARCH_DIR = target/qca
else ifneq ($(BR2_octeon),)
    TARGET_ARCH_DIR = target/octeon
else ifneq ($(BR2_ralink),)
    TARGET_ARCH_DIR = target/ralink
else ifneq ($(BR2_rlx),)
    TARGET_ARCH_DIR = target/rlx
else ifneq ($(BR2_rlx_9607),)
    TARGET_ARCH_DIR = target/rlx_9607
else ifeq ($(BR2_i386)$(BR2_x86_64),y)
    TARGET_ARCH_DIR = target/x86
else ifeq ($(BR2_rtk_ms),y)
    TARGET_ARCH_DIR = target/rtk_ms
else ifeq ($(BR2_rtk_luna),y)
    TARGET_ARCH_DIR = target/rtk_luna
else ifneq ($(BR2_bcm),)
    TARGET_ARCH_DIR = target/bcm
else
$(error define your target arch dir) 
endif

PACK_MK:=pack.mk

ifeq ($(RLX_819X),y)
    PACK_MK = rlx_819x_pack.mk
endif

include $(TARGET_ARCH_DIR)/$(PACK_MK)

# Если переменная FWDIR пуста, то мы задаем дефолтный путь
# Иначе используем эту переменную и включаем перемещение
ifeq ($(FWDIR),)
MOVE_DIR = /var/www/$(PROFILE)
else
MOVE_DIR = $(FWDIR)
NEED_MOVE = y
endif

# Если мы на билдсервере и варим не DEVELOP прошивку то переместим её
ifeq ($(DEVELOP)$(HOSTNAME),rd)
NEED_MOVE = y
endif

ifeq ($(NEED_MOVE),y)
TARGETS += move_image
endif

TARGETS += report

move_image: report
	mkdir -p $(MOVE_DIR)
	mv $(BINARIES_DIR)/$(FW_IMAGE_NAME) $(MOVE_DIR)/$(FW_IMAGE_NAME)

link_image:
	ln -sf $(FW_IMAGE_NAME) $(BINARIES_DIR)/current_fw.bin

check_fw_size:
ifeq ($(BR2_CHECK_FLASH_SIZE),y)
	$(call CHECK_FLASH_SIZE,$(call GET_FW_OFFSET))
endif

report: check_fw_size
	@$(call MESSAGE,"Done: firmware $(FW_IMAGE_NAME)")


PACK_PY_ENV = \
    TARGET_ARCH_DIR="$(TARGET_ARCH_DIR)" \
    BINARIES_DIR="$(BINARIES_DIR)" \
    PROFILE_DIR="$(PROFILE_DIR)" \
    HOST_DIR="$(HOST_DIR)" \
    PROFILE_FILE="$(PROFILE_FILE)" \
    PARTS_FILE="$(PROFILE_DIR)/$(BR2_PARTS_FILE)" \
    LINUX_CONFIG="$(LINUX_CONFIG)" \
    LINUX26_IMAGE_NAME="$(LINUX26_IMAGE_NAME)" \
    FW_IMAGE_NAME="$(FW_IMAGE_NAME)" \
    SIGN_PROGRAM="$(SIGN_PROGRAM)" \
    SIGN_FILE="$(SIGN_FILE)"

pack_dlinkhdr_image: host-mksign host-dlinkhdr
	$(PACK_PY_ENV) target/pack.py

ifeq ($(BR2_OBFUSCATE_FW),y)
OBF_IMAGE := $(HOST_DIR)/usr/bin/obf_image

TARGETS += obfuscate_image
obfuscate_image: host-obf_image pack_image
	$(OBF_IMAGE) $(BINARIES_DIR)/$(FW_IMAGE_NAME) $(BINARIES_DIR)/$(FW_IMAGE_NAME).hex $(BR2_FW_UUID)

ifeq ($(NEED_MOVE),y)
move_image: move_obfuscated_image

move_obfuscated_image: obfuscate_image
	mv $(BINARIES_DIR)/$(FW_IMAGE_NAME).hex $(MOVE_DIR)/
endif

endif
