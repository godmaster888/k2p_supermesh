#############################################################
#
# libpcap
#
#############################################################

LIBPCAP_VERSION:=1.1.1
LIBPCAP_SITE:=${DLINK_GIT_STORAGE}/libpcap
LIBPCAP_LICENSE = BSD-3-Clause
LIBPCAP_LICENSE_FILES = LICENSE
LIBPCAP_INSTALL_STAGING:=YES
# doesn't have an install-strip
LIBPCAP_INSTALL_TARGET_OPT= DESTDIR="$(TARGET_DIR)" \
	$(if $(BR2_STATIC_LIBS),install,install-shared)
LIBPCAP_INSTALL_STAGING_OPT= DESTDIR="$(STAGING_DIR)" install \
	$(if $(BR2_STATIC_LIBS),,install-shared)
LIBPCAP_DEPENDENCIES:=zlib host-bison host-flex
LIBPCAP_CONF_ENV:=ac_cv_linux_vers=2 \
		  ac_cv_header_linux_wireless_h=yes # configure misdetects this
LIBPCAP_CONF_OPT:=--disable-yydebug --with-pcap=linux --includedir=/usr/include

$(eval $(call AUTOTARGETS))
