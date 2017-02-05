all: server client

server: serverside.c
	gcc -Wall -pthread serverside.c -o server

client: client.c
	gcc -Wall -lpthread client.c -lm -o client
