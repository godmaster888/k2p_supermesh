#############################################################
#
# darkstat
#
#############################################################

DARKSTAT_VERSION = 3.0.718
DARKSTAT_SITE = ${DLINK_GIT_STORAGE}/darkstat
DARKSTAT_LICENSE = GPL-2.0-only AND BSD-4-Clause-UC AND BSD-2-Clause AND FSFUL AND X11
DARKSTAT_LICENSE_FILES = LICENSE COPYING.GPL
DARKSTAT_DEPENDENCIES = libpcap

DARKSTAT_MAKE_OPT = HOSTCFLAGS=""

DARKSTAT_CONF_OPT = --includedir=/include \
					--disable-debug

$(eval $(call AUTOTARGETS))
