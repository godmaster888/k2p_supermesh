#############################################################
#
# radvd
#
#############################################################

RADVD_VERSION = 1.8.5
RADVD_SITE = $(DLINK_GIT_STORAGE)/radvd
# See https://fedoraproject.org/wiki/Licensing/radvd_License
RADVD_LICENSE = Radvd
RADVD_LICENSE_FILES = COPYRIGHT
RADVD_CONF_OPT = --program-prefix="" --program-suffix=""

$(eval $(call AUTOTARGETS))
