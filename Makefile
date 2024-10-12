CXX = g++
CXXFLAGS = -std=c++11

all: build

build: MyBot

MyBot: MyBot.cpp
	$(CXX) $(CXXFLAGS) MyBot.cpp -o MyBot.o

run: build
	@./MyBot.o

clean:
	rm -f MyBot.o
