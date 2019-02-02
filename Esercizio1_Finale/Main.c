#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Stampe.h"
#include "Config.h"

Task *coda = NULL;
int count = 1;
Scheduler scheduler;
ExecuteChoice choice;

int main(void) {
	printPrompt();
	return 0;
}

/*
	Function used to create task.
*/
Task* createTask() {
	int id;
	id = count++;
	Task *newPtr = malloc(sizeof(Task));
	newPtr->id = id;
	newPtr->next = NULL;
	assignTaskName(newPtr);
	assignTaskPriority(newPtr);
	assignTaskExecLeft(newPtr);
	return newPtr;
}

/*
	Function used to assign name to the task created
	*/
void assignTaskName(Task* task) {
	char name[256];
	do {
		printf(" Inserire nome \n");
		scanf("%s", name);
	} while (strlen(name) > 9);
	strcpy(task->name, name);
}

/*
	Function used to assign priority to the task created
	*/
void assignTaskPriority(Task* task) {
	int priority;
	do {
		printf(" Inserire priorita' \n");
		scanf("%d", &priority);
	} while (priority <= 0 || priority >= 9);
	task->priority = priority;
}

/*
	Function used to assign the number of execution left to the task created
	*/
void assignTaskExecLeft(Task* task) {
	int execLeft;
	do {
		printf(" Inserire esecuzioni rimanenti \n");
		scanf("%d", &execLeft);
	} while (execLeft <= 0 || execLeft >= 100);
	task->execLeft = execLeft;
}

/*
	Function used to order the queue based on the actual scheduling policy
*/
void ordering_list(Task *task) {
	switch (scheduler) {
	case Priority:
		insertByPriority(task);
		break;
	case ExecLeft:
		insertByExecLeft(task);
		break;
	}
}

/*
	Function used to sort current list based on the scheduling policy
*/
void sort() {

	Task *sorted = NULL;
	Task *current;
	current = coda;
	while (current != NULL)
	{
		Task *next = current->next;
		switch (scheduler) {
		case ExecLeft:
			insertSortExecLeft(&sorted, current);
			break;
		case Priority:
			insertSortPriority(&sorted, current);
			break;
		}
		current = next;
	}
	coda = sorted;
}

/*
	Function creates a new sorted list based on the number of execution left of a process.
*/
void insertSortExecLeft(Task** coda, Task* new_node) {
	Task *current;
	if (*coda == NULL){
		new_node->next = *coda;
		*coda = new_node;
	}else if((*coda)->execLeft >= new_node->execLeft){
		if((*coda)->execLeft == new_node->execLeft){
			*coda = orderHeadByID(coda, new_node);
		}else{
			new_node->next = *coda;
			*coda = new_node;
		}
	}else{
		current = *coda;
		while (current->next != NULL && current->next->execLeft <= new_node->execLeft){
			if(new_node->execLeft == current->next->execLeft){
				if(new_node->id < current->next->id){
					current = current->next;
				}else{
					break;
				}
			}else{
				current = current->next;
			}
		}
		new_node->next = current->next;
		current->next = new_node;
	}
}

/*
	Function creates a new sorted list based on the priority of a process.
*/
void insertSortPriority(Task** coda, Task* new_node) {
	Task *current;
	if (*coda == NULL){
		new_node->next = *coda;
		*coda = new_node;
	}else if((*coda)->priority <= new_node->priority){
		if((*coda)->priority == new_node->priority){
			*coda = orderHeadByID(coda, new_node);
		}else{
			new_node->next = *coda;
			*coda = new_node;
		}
	}else{
		current = *coda;
		while (current->next != NULL && current->next->priority >= new_node->priority){
			if(new_node->priority == current->next->priority){
				if(new_node->id < current->next->id){
					current = current->next;
				}else{
					break;
				}
			}else{
				current = current->next;
			}
		}
		new_node->next = current->next;
		current->next = new_node;
	}
}

/*
	Function used to order current process and the bottom of the queue by id.
	Process with bigger id is placed in front
*/
Task* orderHeadByID(Task** coda, Task* new_node){
	if(new_node->id > (*coda)->id){
		new_node->next = *coda;
		coda = &new_node;
	} else {
		Task* tmp = (*coda)->next;
		new_node->next = tmp;
		(*coda)->next = new_node;
	}
	return *coda;
}

/*
	Function used to insert the process into the queue based on
	priority scheduling policy
*/
void insertByPriority(Task* task) {
	Task *temp, *p, *previous;
	if (isEmpty() || task->priority >= coda->priority) {
		task->next = coda;
		coda = task;
	}
	else {
		p = coda;
		previous = coda;
		while (p->next != NULL && p->next->priority > task->priority) {
			previous = p;
			p = p->next;
		}
		if (p->priority == task->priority) {
			previous = getNodeWithMinId(p, task);
		}
		if (previous->next != NULL) {
			temp = p->next;
			p->next = task;
			task->next = temp;
		}
		else {
			p->next = task;
		}
	}
}

/*
	Function used to insert the process into the queue based on
	execution left scheduling policy
*/
void insertByExecLeft(Task* task) {
	Task *temp, *p, *previous;
	if (isEmpty() || task->execLeft <= coda->execLeft) {
		task->next = coda;
		coda = task;
	}
	else {
		p = coda;
		previous = coda;
		while (p->next != NULL && p->next->execLeft < task->execLeft) {
			previous = p;
			p = p->next;
		}
		if (p->execLeft == task->execLeft) {
			previous = getNodeWithMinId(p, task);
		}
		if (previous->next != NULL) {
			temp = p->next;
			p->next = task;
			task->next = temp;
		}
		else {
			p->next = task;
		}
	}
}

/*
	Function used to execute task in head to the queue
*/
void executeTaskInHead(){
	Task* current = coda;
	Task* prev = NULL;
	while (current->next != NULL) {
		prev = current;
		current = current->next;
	}
	if(current->execLeft > 1){
		current->execLeft -= 1;
		if (scheduler == ExecLeft)
		sort();
	}else{
		if(prev == NULL){
			coda = NULL;
		} else {
		prev->next = NULL;
		}
		free(current);
	}
}

/*
	Function used to execute task with current id inserted by the user
*/
void executeTask(int id){
	Task * current = coda;
	while (current != NULL) {
		if(current->id == id){
			break;
		}else{
			current = current->next;
		}
	}
	if(current->execLeft > 1){
		current->execLeft -= 1;
		if (scheduler == ExecLeft)
		sort();
	}else{
		eliminateTask(current);
	}
}

Task* getPrevious(Task* current){
	Task *previous = NULL;
	Task *temp = coda;
	if(current->id != coda->id){
		while (temp != NULL) {
			if (current->id != temp->id){
				previous = temp;
				temp = temp->next;
			}else{
				break;
			}
		}
	}
	return previous;
}

/*
	Function used to eliminated the process specified by the user throught id
*/
void eliminateID(int id){
	Task * current = coda;
	while (current != NULL) {
		if(current->id == id){
			break;
		}else{
			current = current->next;
		}
	}
	eliminateTask(current);
}
/*
	Function used to eliminate a task from the queue
*/
void eliminateTask(Task* current){
	Task* nextElement = current->next;
	Task* previous = getPrevious(current);
	switch (choice) {
	case EliminateCoda:
		free(current);
		coda = nextElement;
		break;
	case EliminateID:
		if(previous == NULL){
			choice = 2;
			eliminateTask(current);
		}else{
			previous->next = nextElement;
			free(current);
		}
		break;
	}
}

/*
	Function used to insert the id of a process:
	it checks if the process with the id inserted exist into the queue.
	In addition if the process was found and casenumber 5 (which indicates
	case 5, or rather modify the priority of the task with the id inserted), then
	the priority of that process is changed with the new one inserted by the user.
*/
int promptID(int casenumber){
	int currentID = 0;
	int found = 0; //0 -> not found ------ 1 -> found
	printf("Inserisci ID del task\n");
	scanf("%d", &currentID);
	Task *current = coda;
	while(current != NULL){
		if (current->id != currentID){
			current = current->next;
		}else{
			found = 1;
			break;
		}
	}
	if (found == 0){
		currentID = 0;
	}
	if(casenumber == 5 && found != 0){
		changePriority(&current);
		if(scheduler == Priority){
			sort();
		}
	}
	return currentID;
}

/*
	Function used to change priority of the current process.
*/
void changePriority(Task** current){
	int newPriority = 0;
	do {
		printf(" Inserisci la nuova priorita' \n");
		scanf("%d", &newPriority);
	} while (newPriority <= 0 || newPriority >= 9);
	(*current)->priority = newPriority;
}

Task* getNodeWithMinId(Task* p, Task* task) {
	if (p->id < task->id)
	return p;
	else
	return task;
}

int isEmpty() {
	if (coda == NULL)
	return 1;
	else
	return 0;
}

void printMenu(){
	printf(CASO1);
	printf(CASO2);
	printf(CASO3);
	printf(CASO4);
	printf(CASO5);
	printf(CASO6);
	printf(CASO7);
	printf("\n");
}

void printTasks() {
	printf(SEPARATORI);
	printf("| ID  | PRIORITY   |      TASK NAME       |    REMAINING EXEC    | \n\r");
	printf(SEPARATORI);
	Task* t = coda;
	printSingleTask(t);
	while (t != NULL && t->next != NULL) {
		printSingleTask(t->next);
		t = t->next;
	}
}

void printSingleTask(Task *t) {
	if (t != NULL){
		printf("|  %d  |     %d      |      %s       |          %d           |\n\r", t->id, t->priority, t->name, t->execLeft);
		printf(SEPARATORI);
	}
}

void changeSchedulingPolicy() {
	switch (scheduler)
	{
	case Priority:
		scheduler = ExecLeft;
		printf("Politica di scheduling: esecuzioni rimanenti\n\n");
		break;
	case ExecLeft:
		scheduler = Priority;
		printf("Politica di scheduling: priorita'\n\n");
		break;
	}
	sort();
}

/*
	Print menu of available operations
*/
void printPrompt() {
	int index, casenumber, id = 0;
	printMenu();
	printf("Inserire un numero tra 1 e 7 compresi \n");
	scanf("%d", &index);
	Task *newTask = NULL;
	switch (index) {
	case 1:
		newTask = createTask();
		ordering_list(newTask);
		printTasks();
		printPrompt();
		break;
	case 2:
		if(!isEmpty()){
			executeTaskInHead();
			printTasks();
		} else { 
			printf("Nessun Task disponibile\n");
		}
		printPrompt();
		break;
	case 3:
		choice = 3;
		casenumber = 3;
		id = promptID(casenumber);
		if (id == 0) {
			printf("Errore: id non trovato\n");
			printPrompt();
		}
		executeTask(id);
		printTasks();
		printPrompt();
		break;
	case 4:
		choice = 3;
		casenumber = 4;
		id = promptID(casenumber);
		if (id == 0) {
			printf("Errore: id non trovato\n");
			printPrompt();
		}
		eliminateID(id);
		printTasks();
		printPrompt();
		break;
	case 5:
		casenumber = 5;
		id = promptID(casenumber);
		if (id == 0) {
			printf("Errore: id non trovato\n");
		}
		printTasks();
		printPrompt();
		break;
	case 6:
		changeSchedulingPolicy();
		printTasks();
		printPrompt();
		break;
	case 7:
		exit(0);
		break;
	default:
		printf("Errore: comando non disponibile \n");
		printPrompt();
		break;
	}
}
