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

void* clienthandler(void* arg){
  int clientsocket = *(int*) arg;
  
  char msg[4096];
  char menu[] = "0: Exit \n1: List Available Files \nOtherwise Enter File\n";
  while(1){
    //Send choices/menu to client (list of available files, exit)
    send(clientsocket, menu, strlen(menu)+1, 0); 

    //Receive choice from client
    recv(clientsocket,msg,sizeof(msg),0);
    printf("Message: %s\n", msg);

    if(strncmp(msg, "0", 1)==0){
      printf("Client chose to exit\n");
      //exit
      break;
    } else if(strncmp(msg, "1", 1)==0){
      //print out list of files
      printf("Client chose to view available files\n");
      DIR* d;
      struct dirent* dir;
      if((d = opendir(".")) != NULL){
	char filelist[12288];
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
	  while((numBytesRead = fread(dataBuf, 1, sizeof(dataBuf),file)) > 0){
	    printf("Sending packet with %d bytes\n", numBytesRead);
	    send(clientsocket,dataBuf,numBytesRead,0);
	  }
	  recv(clientsocket,msg,sizeof(msg), 0); 
	  fclose(file);
	}
    }     
  }
  
  close(clientsocket);
  printf("The client has disconnected\n");
  //exit(0);
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

	//printf("Press 0 to exit\n");
	//char exitBuf[5];
	while(1) {
		socklen_t len = sizeof(clientaddr);
		int clientsocket = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
		printf("Client has connected\n");

		//Create child process to handle client
		pthread_t child;
		pthread_create(&child, NULL, clienthandler, &clientsocket);
		pthread_detach(child);
		
		/*
		int numBytes = fgets(exitBuf,sizeof(exitBuf),stdin);  
		if(numByes > 0 && strncmp(exitBuf, "0", 1)==0) {
		  //close threads
		  pthread_cancel(child);
		  break;
		}
		*/
		
	}
	return 0;
		
}

int getPortNum()
{
    int portnum  = 0;
    while(portnum < 1024 || portnum > 49151){
      printf("Enter the port number: ");
      scanf("%d", &portnum);
      if(portnum < 1024 || portnum > 49151){
	printf("Invalid port number\n");
      }
      getchar();//clear stdin
    }
    return portnum;
}
