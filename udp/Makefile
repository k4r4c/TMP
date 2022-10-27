CPPFLAGS=-g
LDFLAGS=-g
LDLIBS=-LBoost -Lpthread -pthread

all: client server

server: server.o
	g++ $(LDFLAGS) -o server server.o $(LDLIBS)

server.o: server.cpp
	g++ $(CPPFLAGS) -pthread -c server.cpp

client: client.o
	g++ $(LDFLAGS) -o client client.o $(LDLIBS)

client.o: client.cpp
	g++ $(CPPFLAGS) -pthread -c client.cpp

