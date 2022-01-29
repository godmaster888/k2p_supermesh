#############################################################
#
# libcurl
#
#############################################################
LIBCURL_VERSION = 7.60.0

LIBCURL_SOURCE = curl-$(LIBCURL_VERSION).tar.gz
LIBCURL_SITE = $(DLINK_STORAGE)
LIBCURL_LICENSE = curl
LIBCURL_LICENSE_FILES = COPYING
LIBCURL_AUTORECONF = NO
LIBCURL_INSTALL_STAGING = YES
LIBCURL_INSTALL_TARGET = YES

LIBCURL_CONF_OPT = \
	--disable-option-checking \
	--enable-shared \
	--enable-static \
	--disable-ares \
	--disable-debug \
	--disable-dict \
	--disable-gopher \
	--disable-ldap \
	--disable-ldaps \
	--disable-manual \
	--disable-sspi \
	--disable-rtsp \
	--disable-telnet \
	--disable-tls-srp \
	--disable-pop3 \
	--disable-imap \
	--disable-verbose \
	--disable-libcurl-option \
	--with-random="/dev/urandom" \
	--without-ca-bundle \
	--without-gnutls \
	--without-libidn \
	--without-libidn2 \
	--without-nss \
	--without-libssh2 \
	--without-libmetalink \
	--without-librtmp \
	--without-nghttp2 \
	--with-zlib="$(STAGING_DIR)/usr" \
	--exec-prefix=/usr \
	--includedir="/usr/include" \
	--disable-proxy \
	--disable-smb \
	--disable-ntlm-wb \
	--disable-unix-sockets \
	--disable-threaded-resolver


ifeq ($(BR2_TOOLCHAIN_HAS_THREADS),y)
	LIBCURL_CONF_OPT += --enable-threaded-resolver
else
	LIBCURL_CONF_OPT += --disable-threaded-resolver
endif

ifeq ($(BR2_PACKAGE_LIBCURL_WITH_HTTP),y)
	LIBCURL_CONF_OPT += --enable-http
else ifeq ($(BR2_PACKAGE_TRANSMISSION),y)
	LIBCURL_CONF_OPT += --enable-http
else
	LIBCURL_CONF_OPT += --disable-http
endif

ifeq ($(BR2_PACKAGE_LIBCURL_WITH_FTP),y)
	LIBCURL_CONF_OPT += --enable-ftp
else
	LIBCURL_CONF_OPT += --disable-ftp
endif

ifeq ($(BR2_PACKAGE_LIBCURL_WITH_TFTP),y)
	LIBCURL_CONF_OPT += --enable-tftp
else
	LIBCURL_CONF_OPT += --disable-tftp
endif


ifeq ($(BR2_PACKAGE_LIBCURL_WITH_FILE),y)
	LIBCURL_CONF_OPT += --enable-file
else
	LIBCURL_CONF_OPT += --disable-file
endif

ifeq ($(BR2_PACKAGE_LIBCURL_WITH_COOKIES),y)
	LIBCURL_CONF_OPT += --enable-cookies
else
	LIBCURL_CONF_OPT += --disable-cookies
endif

ifeq ($(BR2_PACKAGE_LIBCURL_WITH_CRYPTO_AUTH),y)
	LIBCURL_CONF_OPT += --enable-crypto-auth
else
	LIBCURL_CONF_OPT += --disable-crypto-auth
endif

ifeq ($(BR2_PACKAGE_LIBCURL_WITH_SMTP),y)
	LIBCURL_CONF_OPT += --enable-smtp
else
	LIBCURL_CONF_OPT += --disable-smtp
endif

ifeq ($(BR2_PACKAGE_LIBCURL_WITH_SMTPS),y)
	LIBCURL_CONF_OPT += --enable-smtps
else
	LIBCURL_CONF_OPT += --disable-smtps
endif

ifeq ($(BR2_PACKAGE_LIBCURL_WITH_CYASSL),y)
	LIBCURL_CONF_OPT += --with-cyassl="$(STAGING_DIR)/usr"
	LIBCURL_DEPENDENCIES += libcyassl
else
	LIBCURL_CONF_OPT += --without-cyassl
endif

ifeq ($(BR2_PACKAGE_LIBCURL_WITH_AXTLS),y)
	LIBCURL_CONF_OPT += --with-axtls="$(STAGING_DIR)/usr"
	LIBCURL_DEPENDENCIES += axtls
else
	LIBCURL_CONF_OPT += --without-axtls
endif

ifeq ($(BR2_PACKAGE_LIBCURL_WITH_OPENSSL),y)
	LIBCURL_CONF_OPT += --with-ssl="$(STAGING_DIR)/usr/lib"
	LIBCURL_CONF_ENV = LIBS="-lcrypto -lssl"
	LIBCURL_DEPENDENCIES += openssl
else
	LIBCURL_CONF_OPT += --without-ssl
endif


ifeq ($(BR2_PACKAGE_LIBCURL_BIN),y)
define LIBCURL_TARGET_CLEANUP
	rm -rf $(TARGET_DIR)/usr/bin/curl-config
	rm -rf $(TARGET_DIR)/share/aclocal/libcurl.m4
endef
else
define LIBCURL_TARGET_CLEANUP
	rm -rf $(TARGET_DIR)/usr/bin/curl
	rm -rf $(TARGET_DIR)/usr/bin/curl-config
	rm -rf $(TARGET_DIR)/share/aclocal/libcurl.m4
endef
endif

LIBCURL_POST_INSTALL_TARGET_HOOKS += LIBCURL_TARGET_CLEANUP

LIBCURL_CONF_ENV += CFLAGS="$(TARGET_CFLAGS) -DCURL_DISABLE_NTLM"

$(eval $(call AUTOTARGETS,package,curl))
