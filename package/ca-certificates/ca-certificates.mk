################################################################################
#
# ca-certificates
#
################################################################################

CA_CERTIFICATES_VERSION = 20200601
CA_CERTIFICATES_SOURCE = ca-certificates_$(CA_CERTIFICATES_VERSION).tar.xz
CA_CERTIFICATES_SITE = $(DLINK_STORAGE)
CA_CERTIFICATES_DEPENDENCIES = host-openssl
CA_CERTIFICATES_LICENSE = MPL-2.0

ifeq ($(BR2_PACKAGE_CA_CERTIFICATES_BUNDLE_ONLY),y)
define CA_CERTIFICATES_INSTALL_TARGET_CMDS
	$(INSTALL) -d -m 0755 $(TARGET_DIR)/etc/ssl/certs

	# Remove any existing certificates
	rm -rf $(TARGET_DIR)/usr/share/ca-certificates
	rm -f  $(TARGET_DIR)/etc/ssl/certs/*

	for i in `find $(@D)/usr/share/ca-certificates -name "*.crt" | LC_COLLATE=C sort` ; do \
		cat $$i ;\
	done >$(@D)/ca-certificates.crt

	$(INSTALL) -D -m 644 $(@D)/ca-certificates.crt \
	$(TARGET_DIR)/etc/ssl/certs/ca-certificates.crt
endef
else ifeq ($(BR2_PACKAGE_CA_CERTIFICATES_FULL),y)
define CA_CERTIFICATES_INSTALL_TARGET_CMDS
	$(INSTALL) -d -m 0755 $(TARGET_DIR)/usr/share/ca-certificates/mozilla
	$(INSTALL) -d -m 0755 $(TARGET_DIR)/etc/ssl/certs
	$(INSTALL) -D -m 644 $(@D)/usr/share/ca-certificates/mozilla/*.crt $(TARGET_DIR)/usr/share/ca-certificates/mozilla

	# Remove any existing certificates under /etc/ssl/certs
	rm -f  $(TARGET_DIR)/etc/ssl/certs/*

	# Create symlinks to certificates under /etc/ssl/certs
	# and generate the bundle
	cd $(TARGET_DIR) ;\
	for i in `find usr/share/ca-certificates -name "*.crt" | LC_COLLATE=C sort` ; do \
		ln -sf ../../../$$i etc/ssl/certs/`basename $${i} .crt`.pem ;\
		cat $$i ;\
	done >$(@D)/ca-certificates.crt

	# Create symlinks to the certificates by their hash values
	$(HOST_DIR)/usr/bin/c_rehash $(TARGET_DIR)/etc/ssl/certs
	# Install the certificates bundle
	$(INSTALL) -D -m 644 $(@D)/ca-certificates.crt \
	$(TARGET_DIR)/etc/ssl/certs/ca-certificates.crt
endef
endif

$(eval $(call GENTARGETS))
