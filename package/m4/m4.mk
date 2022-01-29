################################################################################
#
# m4
#
################################################################################

M4_VERSION = 1.4.18
M4_SOURCE = m4-$(M4_VERSION).tar.xz
M4_SITE = $(DLINK_STORAGE)
M4_LICENSE = GPL-3.0-or-later
M4_LICENSE_FILES = COPYING
HOST_M4_CONF_OPT = --disable-static

$(eval $(call AUTOTARGETS,host))
