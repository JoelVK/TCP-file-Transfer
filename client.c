#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int main (int argc, char** argv) {
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		printf("There was an error creating a socket\n");
		return 1;
	}

	char portnum[6], ipaddr[20];

	printf("enter the port number: \n");
	fgets(portnum,6,stdin);

	printf("now enter the ip: \n");
	fgets(ipaddr,20,stdin);
	
	struct sockaddr_in serveraddr;
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(atoi(portnum));
	serveraddr.sin_addr.s_addr=inet_addr(ipaddr);

	int e = connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	if(e<0){
		printf("there was an error connecting \n");
		return 1;
	}

	printf("enter a line: ");
	char line[5000];
	fgets(line,5000,stdin);
	send(sockfd,line,strlen(line)+1,0);
	
	char line2[5000];
	recv(sockfd,line2,5000,0);
	printf("got from server: %s\n", line2);
	close(sockfd);
	return 0;
}

