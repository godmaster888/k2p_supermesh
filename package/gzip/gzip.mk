#############################################################
#
# gzip
#
#############################################################
GZIP_VERSION:=1.4
GZIP_SITE:=$(DLINK_GIT_STORAGE)/gzip
GZIP_LICENSE = GPL-3.0-or-later
GZIP_LICENSE_FILES = COPYING
$(eval $(call AUTOTARGETS))
