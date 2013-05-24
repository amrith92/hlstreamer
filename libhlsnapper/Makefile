CC		= gcc
C_OPTS	= -Wall -fPIC -c *.c `pkg-config --cflags gstreamer-1.0` `pkg-config --cflags gdk-pixbuf-2.0`
B_OPTS	= -shared -Wl,-soname,libhlsnapper.so.0 -o libhlsnapper.so.0.1 *.o `pkg-config --libs gstreamer-1.0` `pkg-config --libs gdk-pixbuf-2.0`
HEADERS	= *.h

LIB		= libhlsnapper.so.0.1
TARGET	= libhlsnapper.so.0
CURRENT = libhlsnapper.so
MV		= mv
LN		= ln
CP		= cp
LN_OPTS	= -sf
INSTALL	= /opt/lib/hlstreamer
INCLUDE	= /opt/includes/hlstreamer

lib:
	$(CC) $(C_OPTS)
	$(CC) $(B_OPTS)

install:
	sudo $(MV) $(LIB) $(INSTALL)
	sudo $(LN) $(LN_OPTS) $(INSTALL)/$(LIB) $(INSTALL)/$(TARGET)
	sudo $(LN) $(LN_OPTS) $(INSTALL)/$(TARGET) $(INSTALL)/$(CURRENT)
	sudo $(CP) $(HEADERS) $(INCLUDE)


