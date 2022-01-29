################################################################################
#
# lzo
#
################################################################################

LZO_VERSION = 2.09
LZO_SOURCE = lzo-$(LZO_VERSION).tar.gz
LZO_SITE = $(DLINK_STORAGE)
LZO_LICENSE = GPL-2.0-or-later
LZO_LICENSE_FILES = COPYING
LZO_INSTALL_STAGING = YES
LZO_CONF_OPT += --prefix=/usr

# Ships a beta libtool version hence our patch doesn't apply.
# Run autoreconf to regenerate ltmain.sh.
LZO_AUTORECONF = YES
LZO_INSTALL_STAGING_OPT = CC="$(TARGET_CC)" DESTDIR=$(STAGING_DIR) install

$(eval $(call AUTOTARGETS))
$(eval $(call AUTOTARGETS,host))
