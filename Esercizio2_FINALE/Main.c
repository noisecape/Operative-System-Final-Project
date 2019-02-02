#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "Config.h"


enum ExecMode mode;
int main (void) {
    int n = 0;
    printMenu();
    switch (mode){
        case Sequence:
            execSequentialMode(n);
            break;
        case Parallel:
            execParallelMode(n);
            break;
    }
    return 0;
}

void printMenu(){
    int p = 0;
    do{
        printf("Scegliere modalità di esecuzione\n");
        printf("0 = Esecuzione sequenziale\n");
        printf("1 = Esecuzione in parallelo\n");
        scanf("%d", &p);
    }while(p<0 || p>1);
    mode = p;
}

/*
  Function used to create a child of the current process
  that will execute the instruction
*/
void createChild(char*instruction[], int n){
    int pid = fork();
    int status = 0;
    if (pid < 0) {
        printf("Fork Failed\n");
        exit(-1);
    }else if(pid == 0){//child
        printf("Eseguo figlio\n");
        createFile(n);
        execvp(instruction[0], instruction);
        perror(instruction[0]);
        exit(status);
    }else{//father
        if((pid = wait(&status)) == -1){
            perror("wait error");
        } else if(WIFEXITED(status) != 0) {
            printf("Il processo figlio è terminato correttamente, status = %d\n", WIFEXITED(status));
        } else {
            printf("Il processo figlio non è terminato correttamente\n");
        }
        return;
    }

}

/*
  Function used to execute command in parallel mode
*/
void execParallelMode(int n){
    int fd = openFileInstruction();
	 while(1){
        char line[LENGTH];
        line[0] = '\0';
        getchar();
        printf("Inserisci un comando\r\n");
        scanf("%[^\n]", line);
        if(line[0] == '\0'){
            close(fd);
	    //Eseguo i comandi dentro il file
	    execCmdFromInstruction(n);
            printf("Programma terminato\n");
	    exit(0);
        }else{
	    int c = countsChar(line);
            FILE *newFile = fopen("instruction", "a");
            if(newFile == NULL){
                perror("Pointer Error");
                exit(1);
            }
            fprintf(newFile,"%s%s",&line[0], "\n");
            fseek(newFile,c, SEEK_CUR);
            printf("done\n");
            fclose(newFile);
        }
    }
}

/*
  Function used to execute commands contained into the file instruction
*/
void execCmdFromInstruction(int n){
    FILE *newFile = fopen("instruction","r");
    if(newFile == NULL){
        perror("Pointer Error");
        exit(1);
    }
    char command[LENGTH];
    char *instruction[LENGTH];
    while(fgets(command,LENGTH,newFile)){
        int pid = fork();
        int status = 0;
        if(pid < 0){
	    printf("Fork failed\n");
            exit(-1);
        }else if(pid == 0){//child
          eliminateEmptySpace(command);
          readLine(command, instruction);
          createFile(n);
          execvp(instruction[0],instruction);
          perror(instruction[0]);
          exit(status);
        }else{//father
        n++;
        }
    }
    fclose(newFile);
}

/*
Function that counts characters which compose the instrunction
*/
int countsChar(char instruction[]){
	int i = 0;
	while(strcmp(&instruction[i], "\0")){
		i++;
	}
	return i;
}

/*
  Function used to execute instruction inserted in sequential mode
*/

void execSequentialMode(int n){
    while(1){
        char line[LENGTH];
        char *instruction[LENGTH];
        line[0] = '\0';
        getchar();//read \n, then prepare to read the next line
        printf("Inserisci un comando\r\n");
        scanf("%[^\n]", line);
        if(line[0] == '\0'){
            //empty string
            printf("Programma terminato\n");
	    exit(0);
        }else{
            eliminateEmptySpace(line);
            readLine(line,instruction);
            createChild(instruction, n);
            n++;
        }
    }
}

/*
  Function used to open file where the instruction results
  will be written. If the file is not already created, then
  it will be created; Also an existing file is truncated.
*/
int openFile(int n){
    int fd;
    char fileName[32];
    sprintf(fileName, "out.%d",n);
    fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0){
        perror("Open");
        exit(1);
    }
	return fd;
}

/*
  Function used to create the file that will contain all the instruction
  inserted by the user
*/
int openFileInstruction(){
    int fd;
    char fileName[32];
    sprintf(fileName, "instruction");
    fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0){
        perror("Open");
        exit(1);
    }
	return fd;
}

/*
  Function used to create a file which will contain the instruction results
*/
void createFile(int n){
    int fd,rstDup2, errDup2;
    fd = openFile(n);
    rstDup2 = dup2(fd,1);//1 = stdout
    errDup2 = dup2(fd,2);
    if(rstDup2 == -1 || errDup2 == -1){
        perror("Error on dup");
        exit(1);
    }
    close(fd);
}

/*
  Function used to replace empty spaces with char '\0'
*/
void eliminateEmptySpace(char newLine[]){
    int i = 0;
    while(strcmp(&newLine[i], "\0")){
        if(newLine[i] == ' '){
            newLine[i] = '\0';
        }
        i++;
    }
    newLine[i] = '\n';
}

/*
  Function used to read command inserted by the user
  and build the executable instruction
*/
void readLine(char line[],char *instruction[]){
    int i = 0;
    int j = 0;
    int k = 0;
    while(strcmp(&line[i], "\n")){
        char temp[16] = "";
        while(line[j] != '\n' && line[j] != '\0'){
            temp[j] = line[j];
            j++;
        }
        if (line[j] != '\n'){
            instruction[k++] = (line +i);
            j++;
            i = j;
        }else{
            if (j == LENGTH){
                instruction[k++] = (line +i);
            }else{
                line[j] = '\0';
                instruction[k++] = (line +i);
                instruction[k] = NULL;
            }
            break;
        }
    }
}
