
CFLAGS+=-DHAVE_CONFIG_H -Iinc 
CFLAGS += ${EXTRA_CFLAGS}
OBJS := src/FreeList.o  src/iasnprintf.o  src/LinkedList.o  src/ThreadPool.o  src/TimerThread.o

TARGET := libthreadutil.so

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
