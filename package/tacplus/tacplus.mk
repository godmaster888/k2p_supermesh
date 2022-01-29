#############################################################
#
# tacplus
#
#############################################################
TACPLUS_VERSION = v1.5.0-beta.2
TACPLUS_SOURCE = tacplus-$(TACPLUS_VERSION).tar.gz
# TACPLUS_SITE = https://github.com/jeroennijhof/pam_tacplus/archive/
TACPLUS_SITE = $(DLINK_STORAGE)
TACPLUS_LICENSE = GPL-2.0-or-later
TACPLUS_LICENSE_FILES = COPYING
TACPLUS_AUTORECONF = YES
TACPLUS_INSTALL_STAGING = NO
TACPLUS_INSTALL_TARGET = YES

TACPLUS_CONF_OPT += --disable-shared --enable-static

$(eval $(call AUTOTARGETS))
