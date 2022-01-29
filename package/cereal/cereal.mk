################################################################################
#
# cereal
#
################################################################################

CEREAL_VERSION = master
CEREAL_SITE = $(DLINK_GIT_STORAGE)/cereal
CEREAL_LICENSE = BSD license - http://opensource.org/licenses/BSD-3-Clause
CEREAL_LICENSE_FILES = LICENSE

CEREAL_INSTALL_STAGING = YES
CEREAL_INSTALL_TARGET = NO

ifeq ($(BR2_PACKAGE_CEREAL_INSTALL_HEADERS_ONLY),y)
CEREAL_CONF_OPT += \
	-DINSTALL_CEREAL_HEADERS_ONLY=ON \
	-DCEREAL_HEADERS_DEST_PATH=/usr/include
endif

$(eval $(call CMAKETARGETS))
