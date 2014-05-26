CXX = clang++
CXXFLAGS = -g -Wall -Wextra -std=c++11 -pedantic

INCLUDES = -Igen-cpp/ -I /opt/includes/hlstreamer `pkg-config --cflags gstreamer-1.0`

LFLAGS = -L /usr/lib/ -L ./gen-cpp/ -L /opt/libs/hlstreamer `pkg-config --libs gstreamer-1.0`

LIBS = -lpthread -lthrift

SRCS = $(wildcard ./gen-cpp/*.cpp)
SERVER_SRCS = $(wildcard ./src/server/*.cpp)
CLIENT_SRCS = $(wildcard ./src/client/*.cpp)

OBJS = $(SRCS:%.cpp=%.o)

SERVER = hlserver
CLIENT = hlclient

.PHONY: depend clean

default: all

all: $(SERVER) $(CLIENT)
	@echo hlserver successfully compiled

$(SERVER): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SERVER_SRCS) -o $(SERVER) $(OBJS) $(LFLAGS) $(LIBS)

$(CLIENT): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(CLIENT_SRCS) -o $(CLIENT) $(OBJS) $(LFLAGS) $(LIBS)

%.o: %.c
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) ./gen-cpp/*.o ./gen-cpp/*~ ./src/client/*.o ./src/server/*.o ./src/client/*~ ./src/server/*~ $(SERVER) $(CLIENT) *.o

depend: $(SRCS)
	makedepend $(INCLUDES) $^


