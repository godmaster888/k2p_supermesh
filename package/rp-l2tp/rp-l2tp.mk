#############################################################
#
# rp-l2tp
#
#############################################################
RP_L2TP_VERSION = master

RP_L2TP_SITE = $(DLINK_GIT_STORAGE)/rp-l2tp
RP_L2TP_LICENSE = GPL-2.0-or-later AND PROPRIETARY
RP_L2TP_LICENSE_FILES = README
RP_L2TP_CONF_ENV = STRIP=${TARGET_STRIP}

define RP_L2TP_INSTALL_TARGET_CMDS
	install -D ${@D}/l2tpd ${TARGET_DIR}/usr/sbin/l2tpd
	install -D ${@D}/handlers/l2tp-control ${TARGET_DIR}/usr/sbin/l2tp-control
	install -d ${TARGET_DIR}/usr/lib/l2tp/plugins
	install -D ${@D}/handlers/*.so ${TARGET_DIR}/usr/lib/l2tp/plugins
	

endef
$(eval $(call AUTOTARGETS))


