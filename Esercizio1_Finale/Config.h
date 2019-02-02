#ifndef CONFIG_H_
#define CONFIG_H_

//Definisco il tipo Task usato per creare un Task

typedef struct Task {
	int id;
	int priority;
	int execLeft;
	char name[9];
	struct Task *next;
} Task;

typedef enum Scheduler { Priority, ExecLeft} Scheduler;
typedef enum ExecutionChoice {EliminateCoda = 2 , EliminateID = 3} ExecuteChoice;

//Definisco i prototype per poter gestire il flusso del programma

Task* createTask(void);
void printPrompt(void);
void insertTask(Task*);
int isEmpty(void);
void ordering_list(Task*);
void printSingleTask(Task*);
void printTasks(void);
Task* getNodeWithMinId(Task*, Task*);

//Prototype per assegnare le varie informazioni al nuovo Task

void assignTaskName(Task*);
void assignTaskPriority(Task*);
void assignTaskExecLeft(Task*);

//Prototype per lo scheduling della coda che contiene i processi

void changeSchedulingPolicy(void);
void insertByPriority(Task*);
void insertByExecLeft(Task*);
Task* orderHeadByID(Task**,Task*);
void sort(void);
void insertSortPriority(Task**, Task*);
void insertSortExecLeft(Task**, Task*);

//Prototype per eseguire task in testa

void executeTaskInHead(void);
void eliminateTask(Task*);

//Prototype per eseguire task in base all'ID

int promptID(int);
void executeTask(int);
Task *getPrevious(Task*);
void eliminateID(int);

//Prototyper per stampare il menu

void printMenu(void);

//Prototype per modificare la priorita in base all'ID

void changePriority(Task**);

//Prototype per stampare tabella in colonna
void printRow(void);

#endif // !CONFIG_H_
#pragma once
