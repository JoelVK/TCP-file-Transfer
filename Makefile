all: server client

server: serverside.c
	gcc -Wall serverside.c -o server

client: client.c
	gcc -Wall client.c -o client
