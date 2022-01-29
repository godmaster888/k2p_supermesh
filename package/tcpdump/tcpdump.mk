################################################################################
#
# tcpdump
#
################################################################################

TCPDUMP_VERSION = 4.9.0
TCPDUMP_SITE = $(DLINK_GIT_STORAGE)/tcpdump
TCPDUMP_LICENSE = BSD-3-Clause
TCPDUMP_LICENSE_FILES = LICENSE
TCPDUMP_CONF_ENV = \
	ac_cv_linux_vers=2 \
	td_cv_buggygetaddrinfo=no
TCPDUMP_CONF_OPT = \
	--without-crypto \
	--with-system-libpcap \
	$(if $(BR2_PACKAGE_TCPDUMP_SMB),--enable-smb,--disable-smb)
TCPDUMP_DEPENDENCIES = zlib libpcap

ifeq ($(BR2_STATIC_LIBS),y)
TCPDUMP_CONF_OPT += LIBS="`$(STAGING_DIR)/usr/bin/pcap-config --static --additional-libs`"
endif

# make install installs an unneeded extra copy of the tcpdump binary
define TCPDUMP_REMOVE_DUPLICATED_BINARY
	rm -f $(TARGET_DIR)/usr/sbin/tcpdump.$(TCPDUMP_VERSION)
endef

TCPDUMP_POST_INSTALL_TARGET_HOOKS += TCPDUMP_REMOVE_DUPLICATED_BINARY

ifeq ($(BR2_PACKAGE_TCPDUMP_NONPRIVILEGED),y)
define TCPDUMP_PERMISSIONS
	/usr/sbin/tcpdump                f 4755 0  0 - - - - -
endef
endif

$(eval $(call AUTOTARGETS))
