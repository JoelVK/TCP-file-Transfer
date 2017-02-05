/*
 * Michael Brecker
 * Joel Vander Klipp
 * CIS 457 TCP File Transfer Project
 * 1/24/1
 *
 * Description: This is the server side of the TCP file transfer.
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>

int getPortNum();
int clientcount = 0;

void* clienthandler(void* arg){
  int clientsocket = *(int*) arg;
  
  char msg[4096];
  char menu[] = "0: Exit \n1: List Available Files \nOtherwise Enter File\n";
  while(1){
    //Send choices/menu to client (list of available files, exit)
    send(clientsocket, menu, strlen(menu)+1, 0); 
    
    //Receive choice from client
    recv(clientsocket,msg,sizeof(msg),0);
    
    if(strncmp(msg, "0", 1)==0){
      //exit
      break;
      
    } else if(strncmp(msg, "1", 1)==0){
      //print out list of files
      DIR* d;
      struct dirent* dir;
      
      if((d = opendir(".")) != NULL){
	char filelist[12288];
	filelist[0] = '\0'; //reset
	while((dir = readdir(d)) != NULL){
	  sprintf(filelist + strlen(filelist), "%s\n", dir->d_name);
	}
	send(clientsocket, filelist, sizeof(filelist), 0);
	closedir(d);
      }
    } else{
      //Check if file exists, send message Y or N
      FILE* file = fopen(msg, "r");
      
      if(file == NULL){		  
	send(clientsocket,"N", 1, 0); 
      } else {
	
	send(clientsocket,"Y", 1, 0); 
	char dataBuf[128];
	int numBytesRead = 0;
	int size;
	uint32_t sendSize;
        
	//get size of file in bytes
	fseek(file,0,SEEK_END);
	size = ftell(file);
	fseek(file,0,SEEK_SET);
	
	sendSize = htonl(size);
	send(clientsocket,&sendSize,sizeof(sendSize),0);
	while((numBytesRead = fread(dataBuf, 1, sizeof(dataBuf),file)) > 0){
	  send(clientsocket,dataBuf,numBytesRead,0);
	}
	recv(clientsocket,msg,sizeof(msg), 0); 
	fclose(file);
      }
    }        
  }
  
  close(clientsocket);
  clientcount--;
  printf("Client has disconnected. %d client(s) still connected.\n", clientcount);

  return 0;
}

int main( int argc, char **argv){	
  int portnum  = getPortNum();
  
  int sockfd = socket(AF_INET,SOCK_STREAM,0);
  
  struct sockaddr_in serveraddr, clientaddr;
  serveraddr.sin_family=AF_INET;
  serveraddr.sin_port=htons(portnum);
  serveraddr.sin_addr.s_addr=INADDR_ANY;
  
  bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
  listen(sockfd,10);
  
  printf("\nWaiting for clients...\n");
  while(1) {
    socklen_t len = sizeof(clientaddr);
    int clientsocket = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
    clientcount++;
    printf("Client has connected. %d client(s) now connected\n", clientcount);
    
    //Create child process to handle client
    pthread_t child;
    pthread_create(&child, NULL, clienthandler, &clientsocket);
    pthread_detach(child);
  }
  return 0;	
}

int getPortNum(){
  int portnum  = 0;
  while(1){
    printf("Enter the port number: ");
    scanf("%d", &portnum);
    while(getchar() != '\n');  //Clear stdin
    if(portnum >= 1024 && portnum <= 49151) break; //Check for valid input
    printf("Invalid port number, try again.\n");
  }
  return portnum;
}
