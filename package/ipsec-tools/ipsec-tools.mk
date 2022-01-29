################################################################################
#
# ipsec-tools
#
################################################################################

IPSEC_TOOLS_VERSION = master
IPSEC_TOOLS_SITE = $(DLINK_GIT_STORAGE)/ipsec-tools
IPSEC_TOOLS_LICENSE = BSD-3-Clause
IPSEC_TOOLS_LICENSE_FILES =
IPSEC_TOOLS_INSTALL_STAGING = YES
IPSEC_TOOLS_MAKE = $(MAKE1)
IPSEC_TOOLS_DEPENDENCIES = openssl

# configure hardcodes -Werror, so override CFLAGS on make invocation
IPSEC_TOOLS_MAKE_OPT = CFLAGS='$(TARGET_CFLAGS)'

ifeq ($(BR2_TOOLCHAIN_USES_MUSL),y)
$(warning possibly not musl but gcc-6.3.0 perdit monocle)
IPSEC_TOOLS_MAKE_OPT = CFLAGS='$(TARGET_CFLAGS) -include ${@D}/src/racoon/gnuc.h'
endif

IPSEC_TOOLS_CONF_OPT = \
	  --disable-hybrid \
	  --without-libpam \
	  --disable-gssapi \
	  --enable-security-context=no \
	  --with-kernel-headers=$(STAGING_DIR)/usr/include \
	  --enable-shared \
	  --enable-static \
	  --without-libradius \
	  --enable-adminport \
	  --enable-natt \
	  --enable-frag \
	  --enable-dpd


define IPSEC_TOOLS_INSTALL_TARGET_CMDS
	install -m 755 ${@D}/src/libipsec/.libs/libipsec.so.0.0.1 $(TARGET_DIR)/usr/lib/libipsec.so.0
	install -m 755 ${@D}/src/racoon/.libs/libracoon.so.0.0.0 $(TARGET_DIR)/usr/lib/libracoon.so.0
	install -m 755 ${@D}/src/racoon/.libs/racoon $(TARGET_DIR)/usr/bin
	install -m 755 ${@D}/src/racoon/.libs/racoonctl $(TARGET_DIR)/usr/bin
	install -m 755 ${@D}/src/setkey/.libs/setkey $(TARGET_DIR)/usr/bin
	install -m 755 ${@D}/src/racoon/plainrsa-gen $(TARGET_DIR)/usr/bin

endef

$(eval $(call AUTOTARGETS))
