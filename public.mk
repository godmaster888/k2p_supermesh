
ifneq ($(IS_PUBLIC),y)
$(error do not include this file, if not PUBLIC)
endif

export LD_LIBRARY_PATH=$(HOST_DIR)/usr/lib

FAKE_TARGETS :=

ifeq ($(BR2_ralink)$(BR2_SUPPORT_WIFI),yy)
FAKE_TARGETS += mediatek_wifi
endif

ifeq ($(BR2_rlx),y)
FAKE_TARGETS += host-cvimg rtl_wifi
ifeq ($(BR2_modems),y)
FAKE_TARGETS += adslctrl rtl_ethctl
endif
endif

 # busybox
FAKE_TARGETS += deuteron_framework

# tz
FAKE_TARGETS += host-deuteron_apps

# host-dapps
FAKE_TARGETS += host-deuteron_modules

# host-dlibs
FAKE_TARGETS += host-libmtd host-hwdata

FAKE_TARGETS += host-mksign

$(FAKE_TARGETS):
	$(Q)echo $@ is prebuilt


HOST_DEUTERON_FRAMEWORK_DEPENDENCIES := host-jansson host-libmnl host-zlib host-libmd5-rfc host-libcrc
# fake: HOST_DEUTERON_FRAMEWORK_DEPENDENCIES := host-libmtd host-hwdata
HOST_DEUTERON_FRAMEWORK_DEPENDENCIES += $(if $(BR2_WITH_LUA_HOST), host-lua host-lua-minify,)
HOST_DEUTERON_FRAMEWORK_DEPENDENCIES += host-openssl

host-deuteron_framework: $(HOST_DEUTERON_FRAMEWORK_DEPENDENCIES)
host-deuteron_apps: host-jansson host-deuteron_framework

# dlibs
LOST_TARGETS += libmd5-rfc

# anweb
LOST_TARGETS += host-gpp host-nodejs

LOST_TARGETS += $(if $(BR2_USB_SUPPORT),libusb-compat,)

# attach to smth
host-d-link_apps: $(LOST_TARGETS)
