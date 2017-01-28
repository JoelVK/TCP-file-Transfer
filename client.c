/*
 * Michael Brecker
 * Joel Vander Klipp
 * CIS 457 TCP File Transfer Project
 * 1/24/17
 * 
 * Description: This is the client side of the TCP file transfer.
 */

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

	//Get IP and Port number
	char portnum[6], ipaddr[20];
	printf("Enter the port number: \n");
	fgets(portnum,6,stdin);
	int num = atoi(portnum);
	if(num < 1024 || num > 49151){
	  printf("Error: Invalid port number.\n");
	  return 1;
	}
	printf("Enter the IP: \n");
	fgets(ipaddr,20,stdin);
	
	//Connect to server
	struct sockaddr_in serveraddr;
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(atoi(portnum));
	serveraddr.sin_addr.s_addr=inet_addr(ipaddr);
	int e = connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	if(e<0){
		printf("There was an error connecting \n");
		return 1;
	}

	//Get file from user
	printf("Enter a filepath: ");
	char fp[5000];
	fgets(fp,5000,stdin);
	fp[strlen(fp)-1] = '\0';  //Remove newline character
	send(sockfd,fp,strlen(fp)+1,0);
	
	//Server responds with either a Y or N to signal
	//if it could find the file
	char result[1];
	recv(sockfd,result, sizeof(result),0);
	if(strncmp(result, "N",1)==0){
	  printf("Server could not find file\n");
	  return 1;
	}

	//Create file for destination of transfer
	char* filename = basename(fp);
	FILE* file = fopen(filename, "w");
	if(file == NULL){
	  printf("Error: Failed to open/create file\n");
	  return 1;
	}

	//Transfer the file
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

