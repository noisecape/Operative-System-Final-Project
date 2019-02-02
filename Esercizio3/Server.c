#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include "ConfigServer.h"

int idCount;
Commands command;
int fd;

int main(){
  int error;
  char str[256];
  head = NULL;
  idCount = 0;
  if(signal(SIGINT,handleCloseServer) == SIG_ERR) {
          printf("SIGINT install error\n");
          exit(-1);
  }
  unlink(PIPENAME);
  error = mkfifo(PIPENAME, 0660);
  if(error == -1){
    perror("Error while creating the pipe:\n");
    exit(-1);
  }
  fd = open(PIPENAME, O_RDONLY);
  if(fd == -1){
    perror("Error while reading\n");
    exit(-1);
  }
  while(1){
    while(readLine(fd, str)){
      parseMessage(str);
    }
  }
}

void handleCloseServer(int sig){
 if(sig == SIGINT){
	printf("Signal %d: server closed\n", sig);
  killClients();
	close(fd);
	unlink(PIPENAME);
	exit(-1);
 }
 return;
}

void killClients(){
  Client *temp = head;
  while(temp != NULL){
    kill(temp->id, SIGINT);
    temp = temp->nextClient;
  }
  sleep(1);//wait for all clients to disconnect
}

void parseMessage(char *str){
  command = str[0]-'0'; //convert char[0] into an integer
  char pid[10] = "";
  switch (command) {
		case Connect:
		strcpy(pid,str+2); //copy message (contained into str buffer) from str+2 into pid
		insertElement(pid);
		break;
    case Request:
		strcpy(pid,str+2);
		sendList(pid);
		break;
    case Send:
    readPid(str,pid);
    break;
    case Disconnect:
    case Exit:
    strcpy(pid, str+2);
    disconnect(pid);
    break;
    default:
    printf("DEFAULT\n");
    break;
  }
}

/*
  function used to disconnect client from server, so remove it from clients list
*/
void disconnect(char *pid) {
  Client *temp = head;
  Client *prev = NULL;
  while(temp != NULL) {
    if(temp->id != atoi(pid)) {
      prev = temp;
      temp = temp->nextClient;
    } else {
      if(prev == NULL) {
        head = temp->nextClient;
      } else if(temp->nextClient != NULL) {
        prev->nextClient = temp->nextClient;
      } else {
        prev->nextClient = NULL;
      }
      break;
    }
  }
  free(temp);
  printf("Client %s disconnected \n", pid);
}

/*
  Function used to read both sender and receiver pid, and build the message to send
*/
void readPid(char *str, char *myPid){
  char destPid[10] = "";
  char message[256] = "";
  char *space, *cp, *cpMessage;
  space = strchr(str, ' '); //searches for the first occurrence of the character space in the string pointed to by the argument str.
  *space = '\0';
  space = strchr(str+2, ' ');
  *space = '\0';
  strcat(myPid, str+2);
  cp = space;
  space = strchr(cp+1, ' ');
  *space = '\0';
  strcat(destPid, cp+1);
  cpMessage = space+1;
  sprintf(message,"Message from %s:  %s", myPid,cpMessage);
  sendMessage(myPid,destPid,message);
}
/*
  This function send the message from the sender client to receiver client
*/
void sendMessage(char *myPid, char *destPid, char *message){
  if(isConnected(destPid) == 1){
		int error,fd;
		char namepipe[20] = "pipe ";
		strcat(namepipe, destPid);
		error = mkfifo(namepipe,0660);
		if(error == -1){
			perror("Error while creating the pipe:\n");
			exit(-1);
		}
		kill(atoi(destPid), SIGUSR1);
		do { /* Keep trying to open the file until successful */
		    fd = open(namepipe, O_WRONLY); /* Open named pipe for writing */
			if (fd == -1) sleep (1); /* Try again in 1 second */
		} while (fd == -1);
		int messageLen = strlen(message)+1;
		write(fd, message, messageLen);
		close(fd);
		} else {
		kill(atoi(myPid), SIGUSR2);
  }
}

/*
  Function used to update clients list when new connections occur
  if the list is empty, then the head is set as the first client connected.
*/
void insertElement(char *str){
  Client *temp = head;
  if(temp == NULL){
    Client *new = (Client *) malloc(sizeof(Client));
    new->nextClient = NULL;
    new->id = atoi(str);//convert char in integer
    head = new;
  }else{
    while(temp->nextClient != NULL){
      temp = temp->nextClient;
    }
    Client *new = (Client *) malloc(sizeof(Client));
    new->nextClient = NULL;
    new->id = atoi(str);
    temp->nextClient = new;
  }
}

int readLine(int fd, char *str){
  int n;
  do{
    n = read(fd,str,1);
  }while(n>0 && *str++ != '\0'); //leggi fino a \0, quando vede '\0' ritorna numero > 0
    return (n>0);
}
/*
  Function used to send the list of clients to the client that has requested it.
*/
void sendList(char *str){
		int error,fd;
		char namepipe[20] = "pipe ";
		strcat(namepipe, str);
		error = mkfifo(namepipe,0660);
		if(error == -1){
			perror("Error while creating the pipe:\n");
			exit(-1);
		}
		char clientList[256] = "";
		fillClientList(clientList);
		kill(atoi(str), SIGUSR1);
		do { /* Keep trying to open the file until successful */
		    fd = open(namepipe, O_WRONLY); /* Open named pipe for writing */
			if (fd == -1) sleep (1); /* Try again in 1 second */
		} while (fd == -1);
		int messageLen = strlen(clientList) +1;
		write(fd, clientList, messageLen);
		close(fd);
	}

int isConnected(char *pid){
	Client *temp = head;
	int check = 0;
	if(temp != NULL){
		while( temp != NULL && check != 1){
			if(temp->id == atoi(pid)){
				check = 1;
			}else{
				temp = temp->nextClient;
			}
		}
	}
	return check;
}

/*
  Function used to create the list of clients separated by spaces,
  used to send it to the client as requested
*/
void fillClientList(char *clientList){
	Client *temp = head;
	if(temp != NULL){
		while( temp != NULL ){
			char str[10];
			sprintf(str, "%d", temp->id);
			clientList = strcat(clientList, str);
			clientList = strcat(clientList, " ");
			temp = temp->nextClient;
		}
	}
}
