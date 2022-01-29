#############################################################
#
# libcrc
#
#############################################################
LIBCRC_VERSION = master
LIBCRC_SITE = $(DLINK_GIT_STORAGE)/libcrc
LIBCRC_LICENSE = MIT
LIBCRC_LICENSE_FILES = LICENSE
LIBCRC_INSTALL_STAGING = YES

$(eval $(call CMAKETARGETS))
$(eval $(call CMAKETARGETS,host))
