ifeq ($(call qstrip,$(BR2_DLINK_GENERIC_PROFILE)),)
  $(error "BR2_DLINK_GENERIC_PROFILE is empty! Fill this field.")
endif
