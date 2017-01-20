#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main( int argc, char **argv){
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
		socklen_t len = sizeof(clientaddr);
		int clientsocket = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
		printf("Client has connected...\n");
		char line[5000];
		recv(clientsocket,line,5000,0);
		FILE* file = fopen(line, "r");
		if(file == NULL){		  
		  send(clientsocket,"N", 1, 0); 
		} else {
		  send(clientsocket,"Y", 1, 0); 
		  char dataBuf[128];
		  int numBytesRead = 0;
		  while((numBytesRead = fread(dataBuf, 1, sizeof(dataBuf),file)) > 0){
      		    send(clientsocket,dataBuf,numBytesRead,0);
		  }
		}
		
		close(clientsocket);
	}	
		
}
