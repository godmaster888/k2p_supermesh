#############################################################
#
# libnfnetlink
#
#############################################################

LIBNFNETLINK_VERSION = 1.0.1
LIBNFNETLINK_SITE = ${DLINK_GIT_STORAGE}/libnfnetlink
LIBNFNETLINK_LICENSE = GPL-2.0-or-later
LIBNFNETLINK_LICENSE_FILES = COPYING
LIBNFNETLINK_INSTALL_STAGING = YES
LIBNFNETLINK_CONF_OPT = --prefix=/usr --enable-static --enable-shared
LIBNFNETLINK_DEPENDENCIES = kernel

$(eval $(call AUTOTARGETS))
