#ifndef ConfigClient_h
#define ConfigClient_h

#define MAXCLIENTS 100
#define MESSAGELENGTH 256

//Enum per decidere il tipo di comando
typedef enum Commands{Connect = 1, Request = 2, Send = 3, Disconnect = 4, Exit = 5} Commands;

//Stampa menu
int getCommand(void);

//Funzione usata per eseguire comando
void executeCommand(int);

//Funzione per comporre il messaggio: [comando, mio ID,messaggio, clients];
void fillMessage(char*[], char[], char*[], int, int);

//Funzione usata per inviare messaggio
void sendMessageToServer(void);

//Funzione usata per inviare messaggio a client
void sendMessageToClients(void);

//insertClients
int insertClients(char*[]);

//Funzione per ricevere il segnale che sto ricevendo la lista
void handlerList(int);

//Funzione per controllare che il client sia connesso
void isConnected(void);

//Funzione che imposta valore di connected a 1 per indicare che il client è connesso
void firstConnection(void);

#endif
