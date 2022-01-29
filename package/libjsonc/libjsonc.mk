################################################################################
#
# libjsonc
#
################################################################################

LIBJSONC_VERSION = d0b87ee
# LIBJSONC_SITE = https://github.com/json-c/json-c.git
LIBJSONC_SITE = $(DLINK_STORAGE)
LIBJSONC_SOURCE = json-c-$(LIBJSONC_VERSION).tar.gz
LIBJSONC_LICENSE = MIT
LIBJSON_LICENSE_FILES = COPYING
LIBJSONC_INSTALL_STAGING = YES

$(eval $(call CMAKETARGETS))
