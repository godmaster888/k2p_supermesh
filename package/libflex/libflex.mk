#############################################################
#
# libflex
#
#############################################################
LIBFLEX_VERSION = ralink
LIBFLEX_SITE = $(DLINK_GIT_STORAGE)/libflex
LIBFLEX_LICENSE = BSD-3-Clause
LIBFLEX_LICENSE_FILES = COPYING
LIBFLEX_DEPENDENCIES = host-pkg-config
LIBFLEX_INSTALL_STAGING = YES
LIBFLEX_INSTALL_TARGET = YES
#LIBFLEX_CONF_OPT += enable_timerfd=no
LIBFLEX_MAKE_OPT = \
	INSTALL_DIR="$(TARGET_DIR)" \
	CC="$(TARGET_CC)" \
	MAKE="$(MAKE)" \
	LD="$(TARGET_LD)" \
	STRIP="$(TARGET_STRIP)" \
	STRIPTOOL="$(TARGET_STRIP)" \
	INSTALL="$(INSTALL)" \
	MAJOR_VERSION=0 \
	MINOR_VERSION=9 \
	SUBLEVEL=28 \
	LN="ln" \
	TOPDIR="$(TARGET_DIR)/" \

define LIBFLEX_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) $(LIBFLEX_MAKE_OPT) -C $(@D) shared
endef

define LIBFLEX_CLEAN_CMDS
	${MAKE} -C $(@D) clean
endef

$(eval $(call GENTARGETS))
