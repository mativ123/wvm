CFLAGS = -std=c++20 -O2
CFLAGS_debug = -std=c++20 -Og -g
LDFLAGS = -lcurl -lboost_filesystem

all: ./main
debug: CFLAGS := $(CFLAGS_debug)
debug: ./main
	./main

main: main.cpp
	g++ $(CFLAGS) $(LDFLAGS) main.cpp -o ./main
