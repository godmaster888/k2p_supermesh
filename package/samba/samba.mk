#############################################################
#
# SAMBA 3.6.*
#
# Copyright (C) 2007-2014 OpenWrt.org
# Copyright (C) D-Link
#
# This is free software, licensed under the GNU General Public License v2.
#
#############################################################


SAMBA_VERSION = actual
SAMBA_SITE = $(DLINK_GIT_STORAGE)/samba3
SAMBA_LICENSE = GPL-3.0-or-later
SAMBA_LICENSE_FILES = COPYING
SAMBA_AUTORECONF = NO
SAMBA_INSTALL_STAGING = NO
SAMBA_INSTALL_TARGET = YES
SAMBA_SUBDIR = source3
SAMBA_CONFIG_FILE = smb.default
SAMBA_DEPENDENCIES = util-linux
SAMBA_CFLAGS = -I./include $(TARGET_CFLAGS) -I$(STAGING_DIR)/include -DMAX_DEBUG_LEVEL=-1 -D__location__=\\\"\\\" -ffunction-sections -fdata-sections -fvisibility=hidden
SAMBA_LDFLAGS = -Wl,--gc-sections -L $(STAGING_DIR)/usr/lib

ifeq ($(BR2_DSYSINIT),y)
SAMBA_LDFLAGS += -ld_service_notify
SAMBA_DEPENDENCIES += deuteron_framework
endif

SAMBA_CONF_ENV += \
	ac_cv_lib_attr_getxattr=no \
	ac_cv_search_getxattr=no \
	ac_cv_file__proc_sys_kernel_core_pattern=yes \
	libreplace_cv_HAVE_C99_VSNPRINTF=yes \
	libreplace_cv_HAVE_GETADDRINFO=yes \
	libreplace_cv_HAVE_IFACE_IFCONF=yes \
	samba_cv_CC_NEGATIVE_ENUM_VALUES=yes \
	samba_cv_HAVE_GETTIMEOFDAY_TZ=yes \
	samba_cv_HAVE_IFACE_IFCONF=yes \
	samba_cv_HAVE_KERNEL_OPLOCKS_LINUX=yes \
	samba_cv_HAVE_SECURE_MKSTEMP=yes \
	samba_cv_HAVE_WRFILE_KEYTAB=no \
	samba_cv_USE_SETREUID=yes \
	samba_cv_USE_SETRESUID=yes \
	samba_cv_have_setreuid=yes \
	samba_cv_have_setresuid=yes \
	ac_cv_header_zlib_h=no \
	samba_cv_zlib_1_2_3=no \
	ac_cv_path_PYTHON="" \
	ac_cv_path_PYTHON_CONFIG="" \
	LDFLAGS="$(SAMBA_LDFLAGS)" \
	CFLAGS="$(SAMBA_CFLAGS)" \
	CC="$(TARGET_CC) \
	--sysroot=$(STAGING_DIR)"

SAMBA_CONF_OPT += \
	--exec-prefix=/usr \
	--prefix=/ \
	--disable-avahi \
	--disable-cups \
	--disable-pie \
	--disable-relro \
	--disable-static \
	--disable-swat \
	--disable-shared-libs \
	--with-codepagedir=/tmp \
	--with-configdir=/tmp \
	--with-included-iniparser \
	--with-included-popt \
	--with-lockdir=/var/lock \
	--with-logfilebase=/var/log \
	--with-nmbdsocketdir=/var/nmbd \
	--with-piddir=/var/run \
	--with-privatedir=/tmp \
	--with-sendfile-support \
	--without-acl-support \
	--without-cluster-support \
	--without-ads \
	--without-krb5 \
	--without-ldap \
	--without-pam \
	--without-winbind \
	--without-libtdb \
	--without-libtalloc \
	--without-libnetapi \
	--without-libsmbclient \
	--without-libsmbsharemodes \
	--without-libtevent \
	--without-libaddns \
	--with-shared-modules=pdb_tdbsam,pdb_wbc_sam,auth_winbind,auth_wbc,auth_domain

ifeq ($(BR2_LARGEFILE),y)
	SAMBA_CONF_ENV += LINUX_LFS_SUPPORT=yes GLIBC_LFS_SUPPORT=yes
endif

define SAMBA_INSTALL_TARGET_CMDS
	install -pm0755 $(@D)/source3/bin/samba_multicall $(TARGET_DIR)/usr/bin/samba_multicall
	$(TARGET_STRIP) $(TARGET_DIR)/usr/bin/samba_multicall
	ln -sf samba_multicall $(TARGET_DIR)/usr/bin/smbd
	ln -sf samba_multicall $(TARGET_DIR)/usr/bin/nmbd
	ln -sf samba_multicall $(TARGET_DIR)/usr/bin/smbpasswd
	$(INSTALL) -m 0755 -D package/samba/$(SAMBA_CONFIG_FILE) $(TARGET_DIR)/etc/
endef

define SAMBA_UNINSTALL_STAGING_CMDS
	$(MAKE) -C $(@D)/source3 clean
	rm -f $(TARGET_DIR)/usr/bin/smbd
	rm -f $(TARGET_DIR)/usr/bin/nmbd
	rm -f $(TARGET_DIR)/usr/bin/smbpasswd
	rm -f $(TARGET_DIR)/usr/bin/samba_multicall
endef

define SAMBA_UNINSTALL_TARGET_CMDS
	$(MAKE) -C $(@D)/source3 clean
	rm -f $(TARGET_DIR)/usr/bin/smbd
	rm -f $(TARGET_DIR)/usr/bin/nmbd
	rm -f $(TARGET_DIR)/usr/bin/smbpasswd
	rm -f $(TARGET_DIR)/usr/bin/samba_multicall
endef

$(eval $(call AUTOTARGETS))
