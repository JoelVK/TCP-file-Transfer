#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int main( int argc, char *argv){
	int sockfd = socket(AF_INET,SOCK_STREAM,0);

	printf("enter the port number: \n");
	char portnum[6];
	fgets(portnum,6,stdin);
	
	struct sockaddr_in serveraddr, clientaddr;
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(atoi(portnum));
	serveraddr.sin_addr.s_addr=INADDR_ANY;

	bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	listen(sockfd,10);

	while(1) {
		int len = sizeof(clientaddr);
		int clientsocket = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
		char line[5000];
		recv(clientsocket,line,5000,0);
		printf("got from client: %s\n",line);

		send(clientsocket,line,(strlen(line)+1),0);

		close(clientsocket);
	}	
		
}
