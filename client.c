#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <libgen.h>

int main (int argc, char** argv) {
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		printf("There was an error creating a socket\n");
		return 1;
	}

	char portnum[6], ipaddr[20];

	printf("enter the port number: \n");
	fgets(portnum,6,stdin);
	int num = atoi(portnum);
	if(num < 1024 || num > 49151){
	  printf("Error: Invalid port number.\n");
	  return 1;
	}

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

	printf("Enter a filepath: ");
	char fp[5000];
	fgets(fp,5000,stdin);
	fp[strlen(fp)-1] = '\0';
	send(sockfd,fp,strlen(fp)+1,0);
	
	char result[1];
	recv(sockfd,result, sizeof(result),0);
	if(strncmp(result, "N",1)==0){
	  printf("Server could not find file\n");
	  return 1;
	}

	char* filename = basename(fp);
	FILE* file = fopen(filename, "w");
	if(file == NULL){
	  printf("Error: Failed to open/create file\n");
	  return 1;
	}

      	char dataBuf[128];
	int numBytesRead = 0;
	do
        {
	  numBytesRead = recv(sockfd,dataBuf,sizeof(dataBuf),0);
	  if(numBytesRead < 0){
	    printf("Error: failed to read data.\n");
	  }
	  else if(numBytesRead > 0)
	  {
	     fwrite(dataBuf, 1, numBytesRead, file);
	  }
	}
	while(numBytesRead > 0);
	
      	return 0;
}

