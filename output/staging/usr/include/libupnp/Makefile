
#farisey for 7621 build
# .....mipsel-dlink-linux-uclibc-ld  -shared -o libixml.so src/attr.o src/element.o src/ixmlmembuf.o src/namedNodeMap.o src/nodeList.o src/document.o src/ixml.o src/ixmlparser.o src/node.o
# .....bin/mipsel-dlink-linux-uclibc-ld: src/attr.o: перемещение R_MIPS_HI16 для «__gnu_local_gp» не может использоваться при создании общего объекта; перекомпилируйте с -fPIC
EXTRA_CFLAGS += -fPIC $(VERY_EXTRA_CFLAGS)
export EXTRA_CFLAGS

CFLAGS += -I$(CURR_DIR)/ -I$(CURR_DIR)/threadutil/inc 
CFLAGS += -I$(CURR_DIR)/ixml/inc -I$(CURR_DIR)/upnp/inc
CFLAGS += ${EXTRA_CFLAGS}
install: 
	$(MAKE) -C ixml install
	$(MAKE) -C threadutil install
	$(MAKE) -C upnp install

clean:
	$(MAKE) -C ixml clean
	$(MAKE) -C threadutil clean
	$(MAKE) -C upnp clean

