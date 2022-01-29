#############################################################
#
# gperf
#
#############################################################

GPERF_VERSION = 3.1
GPERF_SOURCE = gperf-$(GPERF_VERSION).tar.gz
GPERF_SITE = $(DLINK_STORAGE)
GPERF_LICENSE = GNU GPL
GPERF_LICENSE_FILES = COPYING

$(eval $(call AUTOTARGETS,host))
