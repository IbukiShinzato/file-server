CC = gcc
CFLAGS = -Wall -Wextra -O2

.PHONY: server client clean

server:
	$(CC) $(CFLAGS) -o server server.c
	./server
	rm ./server

client:
	$(CC) $(CFLAGS) -o client client.c
	./client
	rm ./client

clean:
	rm -f server client
