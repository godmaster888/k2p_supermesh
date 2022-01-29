################################################################################
#
# gengetopt
#
################################################################################

GENGETOPT_VERSION = 2.23
GENGETOPT_SOURCE = gengetopt-$(GENGETOPT_VERSION).tar.xz
GENGETOPT_SITE = $(DLINK_STORAGE)
GENGETOPT_LICENSE = GPL-3.0+
GENGETOPT_LICENSE_FILES = COPYING LICENSE

# Parallel build broken
GENGETOPT_MAKE = $(MAKE1)

$(eval $(call AUTOTARGETS,host))
