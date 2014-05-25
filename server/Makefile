CXX = clang++
CXXFLAGS = -pg -Wall -Wextra -std=c++11 -pedantic

INCLUDES = -Igen-cpp/

LFLAGS = -L /usr/lib/ -L ./gen-cpp/

LIBS = -lpthread -lthrift

SRCS = $(wildcard ./gen-cpp/*.cpp)

OBJS = $(SRCS:%.cpp=%.o)

SERVER = hlserver
CLIENT = hlclient

.PHONY: depend clean

default: all

all: $(SERVER) $(CLIENT)
	@echo hlserver successfully compiled

$(SERVER): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) server.cpp -o $(SERVER) $(OBJS) $(LFLAGS) $(LIBS)

$(CLIENT): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) client.cpp -o $(CLIENT) $(OBJS) $(LFLAGS) $(LIBS)

%.o: %.c
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) ./gen-cpp/*.o ./gen-cpp/*~ $(SERVER) $(CLIENT) *.o

depend: $(SRCS)
	makedepend $(INCLUDES) $^


