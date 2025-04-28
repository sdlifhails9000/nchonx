CXX = g++
CXXFLAGS = -Wall -Wextra -Wpedantic

INC = ./inc
CXXFLAGS += -I$(INC)

LIBS = -lws2_32

# CXXFLAGS += -pthread

SRCS = main.cpp InetListener.cpp InetConnection.cpp http.cpp
OBJS = $(SRCS:.cpp=.o)

outbin = main.exe

.PHONY: all
all: $(outbin) $(OBJS)

$(outbin): $(OBJS)
	$(CXX) -g -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) -g $(CXXFLAGS) -c -o $@ $< -lws2_32

.PHONY: clean
clean:
	-@rm -rf $(outbin) $(OBJS)

