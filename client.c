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
#include <math.h>

int getportnum();
void getip(char* dst, int size);

int main (int argc, char** argv) {
  int sockfd = socket(AF_INET,SOCK_STREAM,0);
  if(sockfd<0){
    printf("There was an error creating a socket\n");
    return 1;
  }

  //Get IP and Port number
  char ipaddr[13];
  int portnum = getportnum();
  getip(ipaddr, sizeof(ipaddr));
  
  //Connect to server
  struct sockaddr_in serveraddr;
  serveraddr.sin_family=AF_INET;
  serveraddr.sin_port=htons(portnum);
  serveraddr.sin_addr.s_addr=inet_addr(ipaddr);
  int e = connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
  if(e<0){
    printf("There was an error connecting to port %d and IP %s\n", portnum, ipaddr);
    return 1;
  }
  
  char menu[4096];
  char fp[5000];
  while(1){
    printf("\n-------------------------------\n");
    recv(sockfd, menu, sizeof(menu),0);
    printf(menu);
    printf("\n");
    
    //Get user's choice
    printf("Enter choice: ");
    fgets(fp,5000,stdin);
    fp[strlen(fp)-1] = '\0';  //Remove newline character
    send(sockfd,fp,strlen(fp)+1,0);
    
    if(strncmp(fp, "0", 1)==0){
      //chose to exit
      break;
    } else if(strncmp(fp, "1", 1) == 0){
      //print list of files
      char filelist[12288];
      recv(sockfd,filelist, sizeof(filelist), 0);
      printf("\nAvailable Files: \n");
      printf("%s\n", filelist);
      continue;
    } else {
      //Server responds with either a Y or N to signal
      //if it could find the file
      char result[1];
      recv(sockfd,result, sizeof(result),0);
      if(strncmp(result, "N",1)==0){
	printf("Server could not find file\n");
	continue;
      }
      
      //Create file for destination of transfer
      char* filename = basename(fp);
      FILE* file = fopen(filename, "w");
      if(file == NULL){
	printf("Error: Failed to open/create file\n");
	continue;
      }
      
      //check number of expected packets
      int size, numPackets;
      uint32_t sendSize;
      char dataBuf[128];
      int numBytesRead = 0;
      
      recv(sockfd,&sendSize,sizeof(sendSize),0);
      size = ntohl(sendSize);
      numPackets = ceil((double)size / sizeof(dataBuf));
      //transfer the file
      do {
	numBytesRead = recv(sockfd,dataBuf,sizeof(dataBuf),0);
	if(numBytesRead < 0){
	  printf("Error: failed to read data.\n");
	} else if(numBytesRead > 0) {
	  fwrite(dataBuf, 1, numBytesRead, file);
	}
	numPackets--;
      }
      while(numBytesRead > 0 && numPackets > 0);
      fclose(file);
      send(sockfd,"Done",5,0);
      printf("File transfer complete\n\n");
    }

  }
  
  return 0;
}

int getportnum()
{
  int num = 0;
  while(1) {
    printf("Enter the port number: ");
    scanf("%d", &num);
    while(getchar() != '\n');  //Clear stdin
    if(num >= 1024 && num <= 49151) break;  //Check for valid input
    printf("Invalid port number, try again.\n");
  }
  return num;
}

void getip(char* dst, int size)
{
  printf("Enter IP address: ");
  fgets(dst, size, stdin);
  if(dst[strlen(dst) - 1] == '\n') {
    dst[strlen(dst) - 1] = '\0';
  }
}
