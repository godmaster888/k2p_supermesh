#############################################################
#
# sstrip
#
#############################################################

SSTRIP_LICENSE = GPL-2.0-or-later
SSTRIP_LICENSE_FILES = sstrip.c

$(HOST_DIR)/usr/bin/sstrip: package/sstrip/sstrip.c
	mkdir -p $(HOST_DIR)/usr/bin
	$(CC) -Wall -O2 $^ -o $@

host-sstrip: $(HOST_DIR)/usr/bin/sstrip

host-sstrip-source:

host-sstrip-clean:
	rm -f $(HOST_DIR)/usr/bin/sstrip
