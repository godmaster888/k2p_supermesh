################################################################################
#
# nodejs
#
################################################################################

NODEJS_ARCH = $(shell $(CC) -dumpmachine | sed -e s'/-.*//' -e 's/x86_64/x64/' -e 's/i.86/x86/')

NODEJS_VERSION = 8.15.0
NODEJS_SOURCE = node-v$(NODEJS_VERSION)+gulp-$(NODEJS_ARCH).tar.xz
NODEJS_SITE = $(DLINK_STORAGE)
NODEJS_LICENSE = MIT AND ICU AND BSD-3-Clause AND IPADIC AND BSD-2-Clause AND ISC AND OpenSSLAND Zlib AND Artistic-2.0
NODEJS_LICENSE_FILES = LICENSE

BR2_NODEJS = $(HOST_DIR)/usr/bin/node
BR2_NPM = $(HOST_DIR)/usr/bin/npm

define HOST_NODEJS_EXTRACT_CMDS
    cd $(HOST_DIR) && tar xJf $(DL_DIR)/$(NODEJS_SOURCE) --strip-components=1
endef

$(eval $(call GENTARGETS,host))
