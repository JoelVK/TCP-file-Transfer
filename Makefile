all: server client

server: serverside.c
	gcc -Wall -lpthread serverside.c -o server

client: client.c
	gcc -Wall -lpthread client.c -o client
