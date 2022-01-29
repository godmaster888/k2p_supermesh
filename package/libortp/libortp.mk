################################################################################
#
# libortp
#
################################################################################

LIBORTP_VERSION = 0.25.0
LIBORTP_SITE = $(DLINK_GIT_STORAGE)/libortp
LIBORTP_INSTALL_STAGING = YES
LIBORTP_LICENSE = LGPL-2.1-or-later
LIBORTP_LICENSE_FILES = COPYING

$(eval $(call AUTOTARGETS))
