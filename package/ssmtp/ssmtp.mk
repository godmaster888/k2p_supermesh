#############################################################
#
# ssmtp
#
#############################################################
SSMTP_VERSION = master
SSMTP_SITE = $(DLINK_GIT_STORAGE)/ssmtp
SSMTP_LICENSE = GPL-2.0-or-later
SSMTP_LICENSE_FILES = COPYRIGHT COPYING
SSMTP_AUTORECONF = NO
SSMTP_INSTALL_STAGING = NO
SSMTP_INSTALL_TARGET = YES
SSMTP_CONF_OPT= --sysconfdir=/tmp

define SSMTP_INSTALL_TARGET_CMDS
#	mkdir -p $(TARGET_DIR)/etc/ssmtp
#	ln -sf $(TARGET_DIR)/etc/ssmtp/ssmtp.conf /tmp/ssmtp.conf
#	cp $(@D)/ssmtp.conf $(TARGET_DIR)/etc/ssmtp/
#	cp $(@D)/revaliases $(TARGET_DIR)/etc/ssmtp/
	install -pm0755 $(@D)/ssmtp $(TARGET_DIR)/usr/sbin/
	${TARGET_STRIP} $(TARGET_DIR)/usr/sbin/ssmtp
endef

$(eval $(call AUTOTARGETS))
