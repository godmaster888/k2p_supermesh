################################################################################
#
# lua-minify
#
################################################################################

LUA_MINIFY_VERSION = 8406394
LUA_MINIFY_SITE = $(DLINK_STORAGE)/lua-minify-$(LUA_MINIFY_VERSION).tar.gz
LUA_MINIFY_LICENSE = MIT
LUA_MINIFY_LICENSE_FILES = LICENSE.md

HOST_LUA_MINIFY_DEPENDENCIES = host-lua

define HOST_LUA_MINIFY_INSTALL_CMDS
	mkdir -p $(HOST_DIR)/usr/share/lua/5.3/lua-minify
	cp -af $(@D)/* $(HOST_DIR)/usr/share/lua/5.3/lua-minify
endef

define HOST_LUA_MINIFY_LINK_SHORTCUT
	ln -sf $(HOST_DIR)/usr/share/lua/5.3/lua-minify/LuaMinify.sh \
		$(HOST_DIR)/usr/bin/lua-minify
endef

HOST_LUA_MINIFY_POST_INSTALL_HOOKS += HOST_LUA_MINIFY_LINK_SHORTCUT

$(eval $(call GENTARGETS,host))
