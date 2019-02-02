#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include "ConfigClient.h"

enum Commands commands;
int connected = 0;
int main () {
  signal(SIGUSR1, handlerList);
  while(1){
    commands = getCommand();
    switch (commands){
      case Connect:
      firstConnection();
      break;
      case Request:
      isConnected();
      break;
      case Send:
      isConnected();
      break;
      case Disconnect:
      //disconnettiti dal server
      break;
      case Exit:
      //chiudi programma
      break;
      default:
      printf("Error\n");
      break;
    }
  }
}

void firstConnection(){
  if(connected == 0){
    executeCommand(commands);
    connected = 1;
  }else{
    printf("Client %d already connected\n",getpid());
  }
}

void isConnected(){
  if(connected == 1){
    executeCommand(commands);
  }else{
    printf("Client %d not connected\n",getpid());
  }
}

int getCommand(){
  int p = 0;
  do{
    printf("Menu Client\n");
    printf("1) Connect to server\n");
    printf("2) Request registered clients' id\n");
    printf("3) Send message to client or group of clients\n");
    printf("4) Disconnect client from server\n");
    printf("5) Exit program\n");
    scanf("%d", &p);
  }while(p<1 || p>5);
    return p;
}

void executeCommand(int command){
  switch (command) {
    case 1:
    case 2:
    sendMessageToServer();
    break;
    case 3:
    sendMessageToClients();
    break;
    case 4:
    //disconnetti client
    break;
    case 5:
    //esci
    break;
    default:
    break;
  }
}

void sendMessageToClients(){
  char text[MESSAGELENGTH];
  char *clients[MAXCLIENTS];
  int fd;
  text[0] = '\0';//azzera la line
  do {
    getchar();
    printf("Please insert a message:\n");
    scanf("%[^\n]s",text);
  }while(text[0] == '\0');
  int i = insertClients(clients);
  if (i != 1){//se è 1 non ho client a cui mandare il messaggio
    int totSizeMessage = 2+i-1;
    char *message[totSizeMessage];//2 poiche ci sara il comando e il mio pid; -1 non considero lo spazio vuoto
    int l = i-1;
    fillMessage(message, text, clients, totSizeMessage, l);
    /*
    for(int j = 0; j<totSizeMessage; j++){
      if(j != 0 && j != 1){
        printf("%s\n",message[j]);
      }
      */
    }
  }

void fillMessage(char *message[], char text[], char *clients[], int sizeMessage, int numClients){// [comando,pid,messaggio, clients]
  int j = 0;
  for(int i = 0; i<sizeMessage; i++){
    if (i == 0){//imposto il comando
      char c[12];
      sprintf(c,"%d", commands);
      message[i] = (char *) malloc(12);//misura standard per non ottenere overflow
      message[i] = c;
      //printf("%s\n",message[i]);
    }else if(i == 1){//imposto il mio pid
      char myID[12];
      sprintf(myID, "%d", getpid());
      message[i] = (char *) malloc(12);
      message[i] = myID;
      //printf("%s\n",message[i]);
    }else if(i == 2){//il resto
      message[i] = (char *) malloc(strlen(text));
      message[i] = text;
    }else{
      message[i] = (char *) malloc(10);
      message[i] = clients[j++];
    }
  }
}

int insertClients(char *client[]){
  int i = 0;
  char *temp;
  printf("Insert clients' id:\n");
  do{
    getchar();
    client[i] = (char *)malloc(10);
    scanf("%[^\n]s",client[i]);
    temp = client[i++];
  }while(*temp != '\0');
  return i;
  }

void sendMessageToServer(){
  int fd, messageLen, i;
  char message [100];
  /* Prepare message */
  sprintf (message, "%d %d", commands, getpid ());
  printf("%s\n", message);
  messageLen = strlen (message) + 1;
  do{
    fd = open ("ServerPipe", O_WRONLY); /* Open named pipe for writing */
    if (fd == -1) sleep (1); /* Try again in 1 second */
  }
  while (fd == -1);
    write (fd, message, messageLen); /* Write message down pipe */
    //close (fd); /* Close pipe descriptor */
	if(commands == 2) sleep(5); //aspetta che arrivi la lista dei client
}

int readLine(int fd, char *str){
  int n;
  do{
    n = read(fd,str,1);
	}while(n>0 && *str++ != '\0'); //leggi fino a \0, quando vede '\0' ritorna numero > 0
  return (n>0);
}

void handlerList(int signum){
	int fd;
	char namepipe[20] = "pipe ";
	char str[10];
	char read[256];
	sprintf(str, "%d", getpid());
	strcat(namepipe, str);
	fd = open (namepipe, O_RDONLY);
	printf("List of client: \n");
	while (readLine(fd, read)){
		printf ("%s\n", read);
	}
	printf("Ho finito di leggere i clienti connessi attualmente al server\n");
	close(fd);
	unlink(namepipe);
}
