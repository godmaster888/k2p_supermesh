#############################################################
#
# stockfish
#
#############################################################

STOCKFISH_VERSION = $(call qstrip,$(BR2_PACKAGE_STOCKFISH_BRANCH))
ifeq ($(STOCKFISH_VERSION),)
STOCKFISH_VERSION = $(call qstrip,$(BR2_DEFAULT_BRANCH))
endif

STOCKFISH_SITE := $(DLINK_GIT_STORAGE)/stockfish
STOCKFISH_LICENSE = GPL-3.0-or-later
STOCKFISH_LICENSE_FILE = Copying.txt

STOCKFISH_CXXFLAGS += -Wall -Wcast-qual -fno-exceptions -fno-rtti -std=c++11  -pedantic -Wextra -Wshadow -DNDEBUG -flto
STOCKFISH_CXXFLAGS += -O2 -g
STOCKFISH_CXXFLAGS += -pipe

STOCKFISH_LDFLAGS += -lpthread -latomic
#STOCKFISH_LDFLAGS += -Wl,--no-as-needed
STOCKFISH_LDFLAGS += -Wl,--as-needed
#STOCKFISH_LDFLAGS += -static
STOCKFISH_LDFLAGS += $(STOCKFISH_CXXFLAGS)

STOCKFISH_MAKE_OPTS += CXX='$(TARGET_CXX)'
STOCKFISH_MAKE_OPTS += CXXFLAGS='$(STOCKFISH_CXXFLAGS)'
STOCKFISH_MAKE_OPTS += LDFLAGS='$(STOCKFISH_LDFLAGS)'


define STOCKFISH_CONFIGURE_CMDS
	cp -f $(STOCKFISH_PKGDIR)/Makefile.sdk $(@D)/src/
endef


define STOCKFISH_BUILD_CMDS
	$(STOCKFISH_MAKE_OPTS) $(MAKE) -C $(@D)/src -f Makefile.sdk
endef

define STOCKFISH_CLEAN_CMDS
	cd $(@D)/src; rm -f stockfish *.o syzygy/*.o
endef


define STOCKFISH_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/src/stockfish $(TARGET_DIR)/usr/bin
endef

define STOCKFISH_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/usr/bin/stockfish
endef

$(eval $(call GENTARGETS))
