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
  signal(SIGINT,handleCloseServer);
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
	close(fd);
	unlink(PIPENAME);
	exit(-1);
 }
 return;
}

void parseMessage(char *str){
  command = str[0]-'0'; //converte il carattere in char[0] in intero
  char pid[10];
  switch (command) {
		case Connect:
		strcpy(pid,str+2); //copia da str+2 in pid
		insertElement(pid);
		break;
    case Request:
		strcpy(pid,str+2); //copia da str+2 in pid
		sendList(pid);
		break;
    case Send:
    printf("3\n");
    break;
    case Disconnect:
    printf("4\n");
    break;
    case Exit:
    printf("5\n");
    break;
    default:
    printf("DEFAULT\n");
    break;
  }
}

void insertElement(char *str){
  Client *temp = head;
  if(temp == NULL){
    Client *new = (Client *) malloc(sizeof(Client));
    new->nextClient = NULL;
    new->id = atoi(str);
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

void sendList(char *str){
	//ERRORE da rivedere
	if(isConnected(str) == 1){
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
		printf("aperta pipe in scrittura\n");
		int messageLen = strlen(clientList) +1;
		write(fd, clientList, messageLen);
		close(fd);
		}
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
