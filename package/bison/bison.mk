################################################################################
#
# bison
#
################################################################################

BISON_VERSION = 3.0.4
BISON_SOURCE = bison-$(BISON_VERSION).tar.gz
BISON_SITE = $(DLINK_STORAGE)
BISON_LICENSE = GPL-3.0-or-later
BISON_LICENSE_FILES = COPYING
HOST_BISON_DEPENDENCIES = host-m4

$(eval $(call AUTOTARGETS,host))
