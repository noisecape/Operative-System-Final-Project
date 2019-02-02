#ifndef Config_h
#define Config_h

#define LENGTH 256

//Enum per decidere il tipo di esecuzione
typedef enum ExecMode{Sequence, Parallel} ExecMode;

//Prototype per stampare il menu
void printMenu(void);

//Prototype per eseguire i processi
void execSequentialMode(int);
void execParallelMode(int);

//Prototype per leggere riga

void readLine(char[],char*[]);
void eliminateEmptySpace(char[]);

//Prototype per creare processo figlio e eseguire comando
void createChild(char*[], int);

//Prototype per aprire e creare file
void createFile(int);
int openFile(int);
int openFileInstruction(void);

//Prototype per contare lunghezza istruzione
int countsChar(char[]);

//Prototype per eseguire file in parallelo
void execCmdFromInstruction(int);

#endif /* Config_h */
