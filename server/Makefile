CXX = clang++
CXXFLAGS = -Wall -Wextra -std=c++11 -pedantic

INCLUDES = -Igen-cpp/ -I /opt/includes/hlstreamer `pkg-config --cflags gstreamer-1.0`

LFLAGS = -L /usr/lib/ -L ./gen-cpp/ -L /opt/lib/hlstreamer `pkg-config --libs gstreamer-1.0`

LIBS = -lpthread -lthrift -lboost_system -lboost_filesystem

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

debug: $(SERVER)-debug $(CLIENT)-debug
	@echo hlserver successfully compiled in debug mode

$(SERVER): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SERVER_SRCS) -o $(SERVER) $(OBJS) $(LFLAGS) $(LIBS) -lhlsegmenter

$(SERVER)-debug: $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SERVER_SRCS) -o $(SERVER)-debug $(OBJS) $(LFLAGS) $(LIBS) -lhlsegmenter -g -pg

$(CLIENT): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(CLIENT_SRCS) -o $(CLIENT) $(OBJS) $(LFLAGS) $(LIBS)

$(CLIENT)-debug: $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(CLIENT_SRCS) -o $(CLIENT)-debug $(OBJS) $(LFLAGS) $(LIBS) -g -pg

%.o: %.c
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) ./gen-cpp/*.o ./gen-cpp/*~ ./src/client/*.o ./src/server/*.o ./src/client/*~ ./src/server/*~ $(SERVER) $(CLIENT) $(SERVER)-debug $(CLIENT)-debug *.o

depend: $(SRCS)
	makedepend $(INCLUDES) $^


