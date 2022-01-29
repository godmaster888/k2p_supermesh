#############################################################
#
# cmake
#
#############################################################

CMAKE_VERSION = 3.15.3
CMAKE_SITE = $(DLINK_GIT_STORAGE)/cmake
CMAKE_LICENSE = BSD-3-Clause
CMAKE_LICENSE_FILES = README.rst

define HOST_CMAKE_CONFIGURE_CMDS
 (cd $(@D); \
	LDFLAGS="$(HOST_LDFLAGS)" \
	CFLAGS="$(HOST_CFLAGS)" \
	./bootstrap --prefix=$(HOST_DIR)/usr --parallel=$(PARALLEL_JOBS) \
 )
endef

define HOST_CMAKE_BUILD_CMDS
 $(HOST_MAKE_ENV) $(MAKE) -C $(@D)
endef

define HOST_CMAKE_INSTALL_CMDS
 $(HOST_MAKE_ENV) $(MAKE) -C $(@D) install
endef

$(eval $(call GENTARGETS))
$(eval $(call GENTARGETS,host))
