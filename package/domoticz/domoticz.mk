################################################################################
#
# domoticz
#
################################################################################

DOMOTICZ_VERSION = $(call qstrip,$(BR2_PACKAGE_DOMOTICZ_VERSION))

ifeq ($(DOMOTICZ_VERSION),)
DOMOTICZ_VERSION = dlink-main
endif

DOMOTICZ_SITE = $(DLINK_GIT_STORAGE)/domoticz
DOMOTICZ_LICENSE = GPL-3.0-or-later
DOMOTICZ_LICENSE_FILES = License.txt
DOMOTICZ_DEPENDENCIES = \
	boost \
	libcurl \
	lua \
	openssl \
	openzwave \
	cereal \
	sqlite \
	zlib


# Due to the dependency on mosquitto, domoticz depends on
# !BR2_STATIC_LIBS so set USE_STATIC_BOOST and USE_OPENSSL_STATIC to OFF
DOMOTICZ_CONF_OPT += \
	-DUSE_STATIC_BOOST=OFF \
	-DUSE_OPENSSL_STATIC=OFF

# Do not use any built-in libraries which are enabled by default for
# lua, sqlite and mqtt
DOMOTICZ_CONF_OPT += \
	-DUSE_BUILTIN_LUA=OFF \
	-DUSE_BUILTIN_SQLITE=OFF \
	-DUSE_LUA_STATIC=OFF

#parameter "USE_BUILTIN_LUA=OFF" is deprecated,
#used for compatibility with the old version of domoticz

ifeq ($(BR2_PACKAGE_MOSQUITTO),y)
	DOMOTICZ_CONF_OPT += -DUSE_BUILTIN_MQTT=OFF
	DOMOTICZ_DEPENDENCIES += mosquitto
else
	DOMOTICZ_CONF_OPT += -DUSE_BUILTIN_MQTT=ON
endif

ifneq ($(strip $(BR2_PACKAGE_DOMOTICZ_SQLITEDB_PAGESIZE)),0)
DOMOTICZ_CONF_OPT += \
	-DUSE_SQLITE_PAGE_SIZE=$(BR2_PACKAGE_DOMOTICZ_SQLITEDB_PAGESIZE)
endif

ifeq ($(BR2_PACKAGE_DOMOTICZ_COMPRESS_DB_WITH_COPY_SETTINGS),y)
DOMOTICZ_CONF_OPT += \
	-DUSE_COMPRESS_DB_WITH_COPY_SETTINGS=ON
endif

ifeq ($(BR2_PACKAGE_DOMOTICZ_USE_PRECOMPILED_HEADER),y)
	DOMOTICZ_CONF_OPT += -DUSE_PRECOMPILED_HEADER=ON
else
	DOMOTICZ_CONF_OPT += -DUSE_PRECOMPILED_HEADER=OFF
endif

ifeq ($(BR2_PACKAGE_DEUTERON_FRAMEWORK),y)
DOMOTICZ_DEPENDENCIES += deuteron_framework jansson
endif

ifeq ($(BR2_PACKAGE_LIBUSB),y)
DOMOTICZ_DEPENDENCIES += libusb
endif

ifeq ($(BR2_PACKAGE_OPENZWAVE),y)
DOMOTICZ_DEPENDENCIES += openzwave

# Due to the dependency on mosquitto, domoticz depends on
# !BR2_STATIC_LIBS so set USE_STATIC_OPENZWAVE to OFF otherwise
# domoticz will not find the openzwave library as it searches by
# default a static library.
DOMOTICZ_CONF_OPT += -DUSE_STATIC_OPENZWAVE=OFF
endif

ifeq ($(BR2_PACKAGE_PYTHON3),y)
DOMOTICZ_DEPENDENCIES += python3
DOMOTICZ_CONF_OPT += -DUSE_PYTHON=ON
else
DOMOTICZ_CONF_OPT += -DUSE_PYTHON=OFF
endif

# Install domoticz in a dedicated directory (/opt/domoticz) as
# domoticz expects by default that all its subdirectories (www,
# Config, scripts, ...) are in the binary directory.
DOMOTICZ_TARGET_DIR = /opt/domoticz
DOMOTICZ_CONF_OPT += -DCMAKE_INSTALL_PREFIX=$(DOMOTICZ_TARGET_DIR)

define DOMOTICZ_REMOVE_UNNEEDED_FILES
	$(RM) $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/License.txt
	$(RM) $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/updatedomo
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/styles/dark-th3me
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/styles/elemental
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/styles/element-dark
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/styles/element-light
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/styles/simple-blue
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/styles/simple-gray
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-ar.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-bg.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-bs.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-ca.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-cs.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-da.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-de.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-el.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-es.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-et.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-fa.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-fi.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-fr.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-he.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-hu.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-is.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-it.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-lt.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-lv.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-mk.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-nl.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-no.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-pl.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-pt.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-ro.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-sk.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-sl.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-sq.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-sr.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-sv.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-th.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-tr.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-uk.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-zh.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/i18n/domoticz-zh_TW.json.gz
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/plugins/examples/*
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/plugins/AwoxSMP
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/History.txt
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/secpanel
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/images/security.png
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/images/startup
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/images/floorplans/example.jpg
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/images/nebula.jpg
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/images/shutdown.png
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/www/images/update.png
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/scripts/*
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/dzVents/documentation
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/dzVents/runtime/integration-tests
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/dzVents/runtime/misc
	$(RM) -r $(TARGET_DIR)/$(DOMOTICZ_TARGET_DIR)/dzVents/runtime/tests
endef

DOMOTICZ_POST_INSTALL_TARGET_HOOKS += DOMOTICZ_REMOVE_UNNEEDED_FILES

$(eval $(call CMAKETARGETS))
