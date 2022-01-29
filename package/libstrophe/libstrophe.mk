################################################################################
#
# libstrophe
#
################################################################################

LIBSTROPHE_VERSION = master
LIBSTROPHE_SITE:=$(DLINK_GIT_STORAGE)/libstrophe
LIBSTROPHE_DEPENDENCIES = openssl expat host-pkg-config
# Doesn't ship configure
LIBSTROPHE_AUTORECONF = YES
LIBSTROPHE_LICENSE = MIT OR GPL-3.0-only
LIBSTROPHE_LICENSE_FILES = LICENSE.txt MIT-LICENSE.txt GPL-LICENSE.txt
LIBSTROPHE_INSTALL_STAGING = YES

$(eval $(call AUTOTARGETS))
