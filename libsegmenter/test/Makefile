CC		= gcc
OPTS	= -Wall
INC		= -I/opt/includes/hlstreamer
LIB		= -L/opt/lib/hlstreamer
LFLAG	= -lhlsegmenter
EXTRA	= `pkg-config --cflags --libs gstreamer-0.10`

MAIN	= main.c
MAIN_O	= main

all:
	$(CC) $(OPTS) $(INC) $(LIB) $(MAIN) $(LFLAG) -o $(MAIN_O) $(EXTRA)
