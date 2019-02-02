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
   if(signal(SIGUSR1, handleMessages) == SIG_ERR) {
          printf("SIGUSR1 install error\n");
          exit(-1);
  }
    if(signal(SIGUSR2, handleMessageError) == SIG_ERR) {
          printf("SIGUSR2 install error\n");
          exit(-1);
  }
   if(signal(SIGINT, handleCloseClient) == SIG_ERR) {
     printf("SIGINT install error\n");
     exit(-1);
   }
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
      isConnected();
      break;
      case Exit:
      isConnected();
      break;
      default:
      printf("Error\n");
      break;
    }
  }
}

/*
  Function used to connect the client to the server. It checks two cases:
  - if it is already connected, then a message is printed to the console warning
  the user.
  - if it isn't connected yet, then the function "executeCommand" is called, and the variable
  "connected" is set to 1, meaning that the client has been successfully connected
*/
void firstConnection(){
  if(connected == 0){
    executeCommand(commands);
    connected = 1;
  }else{
    printf("Client %d already connected\n",getpid());
  }
}

/*
  Function used to verify is the client is connected to the server.
*/
void isConnected(){
  if(connected == 1){
    executeCommand(commands);
  }else{
    printf("Client %d not connected\n",getpid());
  }
}

/*
  Function used to read the input inserted from the client throght the console
*/
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

/*
  The function that handles the commands inserted by the user.
*/
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
    sendMessageToServer();
    connected = 0;
    break;
    case 5:
    sendMessageToServer();
    connected = 0;
    exit(-1);
    break;
    default:
    break;
  }
}

/*
  The function that build the message and send it to the server.
  The message has this format [command, myPid, destPid, message]
  where:
  - command is the command inserted by the user
  - myPid the pid of the current client
  - destPid is the destination client
  - message contains the text to send to the server
  Once the message is built the pipe is opened and the message is written in it
*/
void sendMessageToClients(){
  char text[MESSAGELENGTH] = "";
  char *clients[MAXCLIENTS];
  int fd, myPid, lengthArray,j;
  do {
    getchar();
    printf("Please insert a message:\n");
    scanf("%[^\n]s",text);
  }while(text[0] == '\0');
  int numClient = getNumClient(clients);
  if (numClient != 1){
    do{
	fd = open ("ServerPipe", O_WRONLY);
	if (fd == -1) sleep (1);
    }
	while (fd == -1);
	myPid = getLengthPid(getpid());
	for(j = 0; j < numClient-1; j++){
		lengthArray = SIZEINIT + myPid + (strlen(clients[j])) + strlen(text);
		char message[lengthArray];
		sprintf (message, "%d %d %s %s", commands, getpid(), clients[j], text);
		printf ("Send message to %s...\n", clients[j]);
		write (fd, message, strlen(message) +1 );
		sleep(1);
		}
		close(fd);
    }
  }

int getLengthPid(int pid){
	  int i;
	  for (i=0; pid!=0; i++){
		  pid=pid/10;
	  }
	  return i;
  }

/*
  The function that get the receiver clients and store it into the client array
  and return the number of clients inserted.
*/
int getNumClient(char *client[]){
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

/*
  Function used to connect the client to the server, disconnects it
  from server and request the clients list from server.
  Client in all of the cases send a message to the server, which contains
  the command inserted by the user and its pid; then a named pipe is opened to
  communicate with the server. Finally, pipe is closed
*/
void sendMessageToServer(){
  int fd, messageLen, i;
  char message [100];
  /* Prepare message */
  sprintf (message, "%d %d", commands, getpid ());
  messageLen = strlen (message) + 1;
  do{
    fd = open ("ServerPipe", O_WRONLY); /* Open named pipe for writing */
    if (fd == -1) sleep (1); /* Try again in 1 second */
  }
  while (fd == -1);
    write (fd, message, messageLen); /* Write message down pipe */
    close (fd); /* Close pipe descriptor */
	if(commands == 2) sleep(3);
}

int readLine(int fd, char *str){
  int n;
  do{
    n = read(fd,str,1);
	}while(n>0 && *str++ != '\0'); //read until \0. when \0 is find return an integer > 0.
  return (n>0);
}

/*
  Function used to read messages received from server and read the list of other
  clients connected to the same server.
*/
void handleMessages(int signum){
	int fd;
	char namepipe[20] = "pipe ";
	char str[10];
	char read[256];
	sprintf(str, "%d", getpid());
	strcat(namepipe, str);
	fd = open (namepipe, O_RDONLY);
	printf("RESPONSE: \n");
	while (readLine(fd, read)){
		printf ("%s\n", read);
	}
	close(fd);
	unlink(namepipe);
}

void handleMessageError(int signum){
	printf("Ops! Dest pid not connected at server\n");
}

/*
Function used to disconnect and close the client from Server.
The client can be closed when command 5 or CTRL-C is inserted.
Also if server goes offline, client is closed.
*/
void handleCloseClient(int signum) {
  if(signum == SIGINT){
 	printf("Signal %d: client closed\n", signum);
	if(connected == 1){
		commands = 5;
		sendMessageToServer();
		connected = 0;
	}
 	exit(-1);
  }
  return;
}
