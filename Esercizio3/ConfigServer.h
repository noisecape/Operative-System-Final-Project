#ifndef ConfigServer_h
#define ConfigServer_h

typedef struct Client {

  int id;
  struct Client *nextClient;

} Client;

Client *head;

#define PIPENAME "ServerPipe"

//Enum per decidere il tipo di comando
typedef enum Commands{Connect = 1, Request = 2, Send = 3, Disconnect = 4, Exit = 5} Commands;

//Prototype per leggere messaggio del client
int readLine(int, char[]);

//Funzione per chiudere tutti i client connessi al server quando il server viene chiuso
void killClients(void);

//Funzione per inserire nell'ultima posizione della lista dei clienti
void insertElement(char[]);

//Funzione per inviare la lista dei client connessi, al client che lo richiede
void sendList(char[]);

//Handler per gestire chiusura del server
void handleCloseServer(int);

//Funzione per esaminare il messaggio in arrivo dal client
void parseMessage(char[]);

int isConnected(char[]);


//Funzione per leggere il miopid e quello del destinatario
void readPid(char[],char[]);
//Funzione per mandare il messaggio al destinatario
void sendMessage(char[],char[],char[]);

void fillClientList(char[]);

//Funzione per disconnettere il cliente e rimuoverlo dalla lista
void disconnect(char[]);

#endif
