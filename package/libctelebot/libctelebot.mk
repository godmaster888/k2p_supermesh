#############################################################
#
# libctelebot
#
#############################################################

LIBCTELEBOT_CONF_OPT = -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF
LIBCTELEBOT_VERSION = master
LIBCTELEBOT_SITE_METHOD = git
LIBCTELEBOT_SITE = $(DLINK_GIT_STORAGE)/libctelebot
LIBCTELEBOT_INSTALL_STAGING = YES
LIBCTELEBOT_INSTALL_TARGET = NO
LIBCTELEBOT_DEPENDENCIES += jansson libcurl

$(eval $(call CMAKETARGETS))
