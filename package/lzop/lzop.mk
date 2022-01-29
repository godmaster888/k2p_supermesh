#############################################################
#
# lzop
#
#############################################################
LZOP_VERSION:=1.02rc1
LZOP_SITE:=$(DLINK_GIT_STORAGE)/lzop
LZOP_LICENSE = GPL-2.0-or-later
LZOP_LICENSE_FILES = COPYING
LZOP_CONF_OPT:=--program-prefix=""
LZOP_DEPENDENCIES:=lzo

$(eval $(call AUTOTARGETS))
