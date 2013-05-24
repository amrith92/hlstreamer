CC		= gcc
C_OPTS	= -Wall -fPIC -c *.c `pkg-config --cflags --libs gstreamer-1.0`
B_OPTS	= -shared -Wl,-soname,libhlsegmenter.so.0 -o libhlsegmenter.so.0.1 *.o `pkg-config --libs gstreamer-1.0`
HEADERS	= *.h

LIB		= libhlsegmenter.so.0.1
TARGET	= libhlsegmenter.so.0
CURRENT = libhlsegmenter.so
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


