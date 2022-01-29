FW_BLOCK_SIZE = $(call qstrip,$(BR2_NAND_BLOCK_SIZE))

ifeq ($(FW_BLOCK_SIZE),)
FW_BLOCK_SIZE := 64
endif

ifeq ($(BR2_DRU_BOOTSTRAP), y)

#ifeq ($(call qstrip,$(DLINK_DEVICEID)), DWR_921)
# STAGE[12] пока только для DWR_921
# UPDATE: и DWR_953A1

ifeq ($(BR2_DRU_BOOTSTRAP_STAGE1), y)
#pack_image внутри, всем прочим юзать местный
# я толком не вкурил про FORCE, так что сделаю так.
include target/ralink/bootstrap_dwr921c1.mk
else ifeq ($(BR2_DRU_BOOTSTRAP_STAGE2), y)
# по сути нормальная заварка, ничего особого делать не надо
pack_image: _pack_image
else
# DAP_1620A1_MT7620A
include target/ralink/bootstrap.mk
pack_image: _pack_image
endif

else
# нормальные заварки
pack_image: _pack_image
endif #BR2_DRU_BOOTSTRAP

_pack_image: host-mksign
	dd if=$(BINARIES_DIR)/uImage of=$(BINARIES_DIR)/${FW_IMAGE_NAME} bs=$(FW_BLOCK_SIZE)k conv=sync 2>&1
	dd if=$(BINARIES_DIR)/rootfs.squashfs of=$(BINARIES_DIR)/${FW_IMAGE_NAME} oflag=append conv=notrunc 2>&1
ifneq ($(BR2_PRIVATE_KEY),)
	$(ADD_DATAMODEL_VERSION)
	$(SIGN_DEFAULT_CMD)
endif
ifneq ($(SIGNATURE),)
	@echo "get signature from the bootstrap.mk ..."
	echo -n $(SIGNATURE) >> $(BINARIES_DIR)/${FW_IMAGE_NAME}
else ifeq ($(BR2_DRU_BOOTSTRAP), y)
	$(warning no signature defined for transit/bootstrap firmware)
	@echo no signature defined for transit/bootstrap firmware
endif

# TODO нигде этой инфы нет, но на всех MTK это число одинаковое - Boot(0x30000):Config(0x10000):Factory(0x10000)
define GET_FW_OFFSET
	0x50000
endef
