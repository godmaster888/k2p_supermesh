#############################################################
#
# SQLITE
#
#############################################################
SQLITE_VERSION = 3.24.0
SQLITE_SITE = $(DLINK_GIT_STORAGE)/sqlite3
# Public Domain, see https://www.sqlite.org/copyright.html
SQLITE_LICENSE = Unlicense
SQLITE_LICENSE_FILES =
SQLITE_INSTALL_STAGING = YES
SQLITE_INSTALL_TARGET = YES

$(eval $(call CMAKETARGETS))
