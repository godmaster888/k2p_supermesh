################################################################################
#
# Accel pppd
#
################################################################################
ACCEL_PPP_VERSION:=1.3.5
ACCEL_PPP_SITE:=$(DLINK_GIT_STORAGE)/accel-ppp
ACCEL_PPP_DEPENDENCIES = openssl pcre
ACCEL_PPP_LICENSE = GPL-2.0-or-later
ACCEL_PPP_LICENSE_FILES = COPYING
ACCEL_PPP_CONF_OPT = -DCMAKE_EXE_LINKER_FLAGS="$(TARGET_LDFLAGS)" \
		      -DCMAKE_C_FLAGS="-I$(STAGING_DIR)/usr/include" -DKDIR=${LINUX26_DIR} \
		      -DBUILD_DRIVER=FALSE -DCMAKE_BUILD_TYPE=Release -DLOG_PGSQL=FALSE -DSHAPER=FALSE -DRADIUS=TRUE
$(eval $(call CMAKETARGETS,package,accel-ppp))
