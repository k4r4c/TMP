CPPFLAGS=-g
LDFLAGS=-g
LDLIBS=-LBoost -Lpthread -pthread

.PHONY: tcp_signal udp

all: tool tcp_signal udp

tool: tool.o
	g++ $(LDFLAGS) -o tool tool.o $(LDLIBS)

tool.o: tool.cpp
	g++ $(CPPFLAGS) -pthread -c tool.cpp

# Subdirectories
tcp_signal:
	$(MAKE) -C tcp_signal
udp:
	$(MAKE) -C udp

