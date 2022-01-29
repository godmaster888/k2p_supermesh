################################################################################
#
# lua-ev
#
################################################################################

LUA_EV_VERSION = master
LUA_EV_SITE_METHOD = git
LUA_EV_SITE = $(DLINK_GIT_STORAGE)/lua-ev
LUA_EV_DEPENDENCIES = lua libev
LUA_EV_LICENSE = MIT
LUA_EV_LICENSE_FILES = README
LUA_EV_CONF_OPT = -DINSTALL_CMOD="/usr/lib/lua/$(BR2_PACKAGE_LUAINTERPRETER_ABI_VERSION)"

LUA_EV_SUPPORTS_IN_SOURCE_BUILD = NO


$(eval $(call CMAKETARGETS))
