CC		= gcc
C_OPTS	= -Wall -fPIC -c *.c
B_OPTS	= -shared -Wl,-soname,libm3u8.so.0 -o libm3u8.so.0.1 *.o
HEADERS	= *.h

LIB		= libm3u8.so.0.1
TARGET	= libm3u8.so.0
CURRENT = libm3u8.so
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


