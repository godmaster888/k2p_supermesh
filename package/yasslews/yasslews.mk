#############################################################
#
# yasslEWS embedded web server
#
#############################################################

YASSLEWS_VERSION = master
YASSLEWS_SITE = $(DLINK_GIT_STORAGE)/yasslews
YASSLEWS_LICENSE = MIT AND GPL-2.0-or-later
YASSLEWS_LICENSE_FILES =

ifeq (${BR2_PACKAGE_LIBCYASSL},y)
        COPT+=-DSSL_LIB_CYASSL
        YASSLEWS_DEPENDENCIES = libcyassl
        YASSLEWS_POST_INSTALL_TARGET_HOOKS += INSTALL_SSL_CERTIFICATE
else
ifeq (${BR2_YASSLEWS_USE_AXTLS},y)
        COPT+=-DSSL_LIB_AXTLS
        YASSLEWS_DEPENDENCIES = axtls
        YASSLEWS_POST_INSTALL_TARGET_HOOKS += INSTALL_SSL_CERTIFICATE
else
        COPT+=-DNO_SSL
endif
endif

ifeq (${BR2_INET_IPV6},y)
        COPT+=-DUSE_IPV6
endif
ifeq (${BR2_YASSLEWS_EXTENDED_BUFFER},y)
	COPT+=-DLARGE_RECEIVING_BUF
endif

define YASSLEWS_BUILD_CMDS
        COPT="$(COPT)" $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" V=99 -C $(@D) linux
endef

define YASSLEWS_CLEAN_CMDS
        ${MAKE} -C $(@D) clean
endef

define INSTALL_SSL_CERTIFICATE
        $(INSTALL) -D -m 0444 $(@D)/server.pem $(TARGET_DIR)/etc/ssl_cert_def.pem
endef

define YASSLEWS_INSTALL_TARGET_CMDS
        $(INSTALL) -D -m 0755 $(@D)/yasslews $(TARGET_DIR)/usr/bin
endef

$(eval $(call GENTARGETS))
