#############################################################
#
# mosquitto
#
#############################################################

MOSQUITTO_VERSION = $(call qstrip,$(BR2_PACKAGE_MOSQUITTO_VERSION))

ifeq ($(MOSQUITTO_VERSION),)
MOSQUITTO_VERSION = dsysinit
endif

MOSQUITTO_SITE_METHOD = git
MOSQUITTO_SUPPORTS_IN_SOURCE_BUILD = NO
MOSQUITTO_SITE = $(DLINK_GIT_STORAGE)/mosquitto
MOSQUITTO_DEPENDENCIES = host-patchelf
MOSQUITTO_LICENSE = EPL-1.0
MOSQUITTO_LICENSE_FILES = epl-v10 edl-v10
MOSQUITTO_INSTALL_STAGING = YES

ifeq ($(BR2_PACKAGE_DEUTERON_FRAMEWORK),y)
# for dlink_service_connect(), -ld_service_notify
MOSQUITTO_DEPENDENCIES += deuteron_framework
MOSQUITTO_CONF_OPT += -DWITH_DSYSINIT=ON
endif

ifeq ($(BR2_PACKAGE_MOSQUITTO_FEAT_TLS),y)
MOSQUITTO_DEPENDENCIES += openssl
endif

ifeq ($(BR2_PACKAGE_MOSQUITTO_FEAT_UUID),y)
MOSQUITTO_CONF_OPT += -DWITH_UUID=ON
MOSQUITTO_DEPENDENCIES += util-linux
else
MOSQUITTO_CONF_OPT += -DWITH_UUID=OFF
endif

MOSQUITTO_CONF_OPT += -DWITH_TLS=$(if $(BR2_PACKAGE_MOSQUITTO_FEAT_TLS),ON,OFF)
MOSQUITTO_CONF_OPT += -DWITH_TLS_PSK=$(if $(BR2_PACKAGE_MOSQUITTO_FEAT_TLS_PSK),ON,OFF)
MOSQUITTO_CONF_OPT += -DWITH_EC=$(if $(BR2_PACKAGE_MOSQUITTO_FEAT_TLS_EC),ON,OFF)


MOSQUITTO_CONF_OPT += -DWITH_SOCKS=$(if $(BR2_PACKAGE_MOSQUITTO_FEAT_SOCKS5),ON,OFF)
MOSQUITTO_CONF_OPT += -DWITH_SRV=$(if $(BR2_PACKAGE_MOSQUITTO_FEAT_SRV),ON,OFF)
#MOSQUITTO_CONF_OPT += -DDRU_BROKER_USES_LIB=$(if $(BR2_PACKAGE_MOSQUITTO_FEAT_LIBMQTT),ON,OFF)
MOSQUITTO_CONF_OPT += -DINC_BRIDGE_SUPPORT=$(if $(BR2_PACKAGE_MOSQUITTO_FEAT_BRIDGE),ON,OFF)
MOSQUITTO_CONF_OPT += -DUSE_LIBWRAP=$(if $(BR2_PACKAGE_MOSQUITTO_FEAT_LIBWRAP),ON,OFF)
MOSQUITTO_CONF_OPT += -DINC_DB_UPGRADE=$(if $(BR2_PACKAGE_MOSQUITTO_FEAT_DBUP),ON,OFF)
MOSQUITTO_CONF_OPT += -DINC_MEMTRACK=$(if $(BR2_PACKAGE_MOSQUITTO_FEAT_MEMTRACK),ON,OFF)
MOSQUITTO_CONF_OPT += -DWITH_PERSISTENCE=$(if $(BR2_PACKAGE_MOSQUITTO_FEAT_PERSIST),ON,OFF)
MOSQUITTO_CONF_OPT += -DWITH_SYS_TREE=$(if $(BR2_PACKAGE_MOSQUITTO_FEAT_SYSTREE),ON,OFF)
MOSQUITTO_CONF_OPT += -DWITH_WEBSOCKETS=$(if $(BR2_PACKAGE_MOSQUITTO_FEAT_WEBSOCKS),ON,OFF)

MOSQ_BIN_FILES	:=
MOSQ_SBIN_FILES	:=
MOSQ_LIB_FILES	:=

MOSQ_LIB_FILES	+= $(if $(BR2_PACKAGE_MOSQUITTO_BIN_LIB),lib/libmosquitto.so*,)
MOSQ_SBIN_FILES	+= $(if $(BR2_PACKAGE_MOSQUITTO_BIN_BROKER),src/mosquitto,)
MOSQ_BIN_FILES	+= $(if $(BR2_PACKAGE_MOSQUITTO_BIN_PASSWD),src/mosquitto_passwd,)
MOSQ_BIN_FILES	+= $(if $(BR2_PACKAGE_MOSQUITTO_BIN_SUB),client/mosquitto_sub,)
MOSQ_BIN_FILES	+= $(if $(BR2_PACKAGE_MOSQUITTO_BIN_PUB),client/mosquitto_pub,)

ifneq ($(BR2_PACKAGE_DLINK_HQ_MESHD),)

define mqtt_files_install
	@echo 'Mosquitto files will be installed'
	@echo 'by the dlink-hq (SuperMesh) package.'
	@echo 'Skip the default installation.'
	@echo 'TODO: handle this conflicting case.'
endef

else

define mqtt_files_install
	@echo install default mosquitto files.
	tar -C $(TOPDIR)/$(MOSQUITTO_PKGDIR)/files -c . -f $(TARGET_DIR)/etc/mqtt.files.tar
endef

endif

# TODO удалить RPATH!

#define my_install
#	install $(1) $(2)
#endef
#my_install = install $(1) $(2)
define MOSQUITTO_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/lib
	$(foreach file,$(MOSQ_LIB_FILES),cp -fP $(MOSQUITTO_BUILDDIR)/$(file) $(TARGET_DIR)/usr/lib;)
	$(HOST_DIR)/usr/bin/patchelf --remove-rpath $(TARGET_DIR)/usr/lib/libmosquitto.so
	$(foreach file,$(MOSQ_BIN_FILES),install -D $(MOSQUITTO_BUILDDIR)/$(file) $(TARGET_DIR)/usr/bin;)
	$(foreach file,$(MOSQ_SBIN_FILES),install -D $(MOSQUITTO_BUILDDIR)/$(file) $(TARGET_DIR)/usr/sbin;)
	rm -rf $(TARGET_DIR)/etc/mosquitto
	ln -snf /tmp/mosquitto $(TARGET_DIR)/etc/mosquitto
	$(call mqtt_files_install)
endef

define MOSQUITTO_INSTALL_STAGING_CMDS
	mkdir -p $(STAGING_DIR)/usr/lib
	# now for libapson_mqtt_api.so
	$(foreach file,$(MOSQ_LIB_FILES),cp -fP $(MOSQUITTO_BUILDDIR)/$(file) $(STAGING_DIR)/usr/lib;)
	install $(@D)/lib/mosquitto.h $(STAGING_DIR)/usr/include
endef

$(eval $(call CMAKETARGETS))
