
CFLAGS+=-DHAVE_CONFIG_H -Iinc -Isrc/inc 
CFLAGS += ${EXTRA_CFLAGS}

OBJS := src/attr.o src/element.o  src/ixmlmembuf.o src/namedNodeMap.o src/nodeList.o \
		src/document.o  src/ixml.o  src/ixmlparser.o  src/node.o

TARGET := libixml.so

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) -shared -o $@ $^

install: all
	install -d $(INSTALL_DIR)/lib
	install -pm0755 $(TARGET) $(INSTALL_DIR)/lib
	$(STRIP) $(INSTALL_DIR)/lib/$(TARGET)

clean:
	find ./ -name "*.o" -exec rm -f {} \;
	rm -f $(TARGET)

