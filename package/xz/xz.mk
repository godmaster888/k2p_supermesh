#############################################################
#
# xz-utils
#
#############################################################
XZ_VERSION = 4.999.9b
XZ_SITE = $(DLINK_GIT_STORAGE)/xz
XZ_LICENSE = Unlicense AND LGPL-2.1-or-laer AND GPL-2.0-or-later
XZ_LICENSE_FILES = COPYING COPYING.LGPLv2.1 COPYING.GPLv2
XZ_INSTALL_STAGING = YES
XZ_LIBTOOL_PATCH = NO

$(eval $(call AUTOTARGETS))
$(eval $(call AUTOTARGETS,host))
