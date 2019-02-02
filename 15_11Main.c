//
//  main.c
//  Esercizio1
//
//  Created by Tommaso Capecchi on 12/10/17.
//  Copyright � 2017 Tommaso Capecchi. All rights reserved.
//
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stampe.h"
#include "config.h"

Task *head = NULL;
int count = 1;
Scheduler scheduler;
ExecuteChoice choice;

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

void assignTaskName(Task* task) {
	char name[256];
	do {
		printf(" Inserire nome \n");
		scanf("%s", name);
	} while (strlen(name) > 9);
	strcpy(task->name, name);
}

void assignTaskPriority(Task* task) {
	int priority;
	do {
		printf(" Inserire priorita' \n");
		scanf("%d", &priority);
	} while (priority <= 0 || priority >= 9);
	task->priority = priority;
}

void assignTaskExecLeft(Task* task) {
	int execLeft;
	do {
		printf(" Inserire esecuzioni rimanenti \n");
		scanf("%d", &execLeft);
	} while (execLeft <= 0 || execLeft >= 100);
	task->execLeft = execLeft;
}


void ordering_list(Task *task) {
	
	switch (scheduler) {
		case Priority:
			orderByPriority(task);
			break;
		case ExecLeft:
			orderByExecLeft(task);
			break;
	}
}

void sort() {
	// Initialize sorted linked list
	Task *sorted = NULL;

	// Traverse the given linked list and insert every
	// node to sorted
	Task *current;
	current = head;
	while (current != NULL)
	{
		// Store next for next iteration
		Task *next = current->next;

		// insert current in sorted linked list
        switch (scheduler) {
            case ExecLeft:
                InsertSortExecLeft(&sorted, current);
                break;
            case Priority:
                InsertSortPriority(&sorted, current);
                break;
        }
		// Update current
		current = next;
	}

	// Update head to point to sorted linked list
	head = sorted;
}

void InsertSortExecLeft(Task** head_ref, Task* new_node) {
    Task *current, *temp;
    /* Special case for the head end */
    if (*head_ref == NULL){
        new_node->next = *head_ref;
        *head_ref = new_node;
    }else if((*head_ref)->execLeft >= new_node->execLeft){
        if((*head_ref)->execLeft == new_node->execLeft){
            *head_ref= orderHeadByID(head_ref, new_node);
        }else{
            new_node->next = *head_ref;
            *head_ref = new_node;
        }
    }else{
        /* Locate the node before the point of insertion */
        current = *head_ref;
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

/* function to insert a new_node in a list. Note that this
function expects a pointer to head_ref as this can modify the
head of the input linked list*/
void InsertSortPriority(Task** head_ref, Task* new_node) {
	Task *current;
	/* Special case for the head end */
    if (*head_ref == NULL){
        new_node->next = *head_ref;
        *head_ref = new_node;
    }else if((*head_ref)->priority <= new_node->priority){
        if((*head_ref)->priority == new_node->priority){
            *head_ref = orderHeadByID(head_ref, new_node);
        }else{
            new_node->next = *head_ref;
            *head_ref = new_node;
        }
	}else{
		/* Locate the node before the point of insertion */
		current = *head_ref;
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

Task* orderHeadByID(Task** head, Task* new_node){
    if(new_node->id > (*head)->id){
        new_node->next = *head;
        head = &new_node;
	} else {
		Task* tmp = (*head)->next;
		new_node->next = tmp;
		(*head)->next = new_node; 
	}
	return *head;
}


void orderByPriority(Task* task) {
	Task *temp, *p, *previous;
	if (isEmpty() || task->priority >= head->priority) {
		task->next = head;
		head = task;
	}
	else {
		p = head;
		previous = head;
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
void orderByExecLeft(Task* task) {
	Task *temp, *p, *previous;
	if (isEmpty() || task->execLeft <= head->execLeft) {
		task->next = head;
		head = task;
	}
	else {
		p = head;
		previous = head;
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

void executeTaskInHead(){
    if(head != NULL){
        if(head->execLeft > 1){
            head->execLeft -= 1;
            if (scheduler == ExecLeft)
                sort();
        }else{
            eliminateTask(head);
        }
    }
}

void executeTask(int id){
    Task * current = head;
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

Task * getPrevious(Task* current){
    Task *previous = NULL;
    Task *temp = head;
    if(current->id != head->id){
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

void eliminateID(int id){
    Task * current = head;
    while (current != NULL) {
        if(current->id == id){
            break;
        }else{
            current = current->next;
        }
    }
    eliminateTask(current);
}

void eliminateTask(Task* current){
        Task* nextElement = current->next;
        Task* previous = getPrevious(current);
        switch (choice) {
            case ExecuteHead:
                free(current);
                head = nextElement;
                break;
            case EliminateID:
                if(previous == NULL){
                    choice = 2;
                    eliminateTask(current);
                }else{
                    previous->next = current->next;
                    free(current);
                }
                break;
        }
    }

int promptID(int casenumber){
    int currentID = 0;
    int found = 0; //0 -> not found ------ 1 -> found
	if (casenumber == 3) {
		printf("Inserisci ID che vuoi eseguire\n");
	}else {
		printf("Inserisci ID che vuoi eliminare\n");
	}
    scanf("%d", &currentID);
    Task *current = head;
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
    return currentID;
}

Task* getNodeWithMinId(Task* p, Task* task) {
	if (p->id < task->id)
		return p;
	else
		return task;
}

int isEmpty() {
	if (head == NULL)
		return 1;
	else
		return 0;
}

void printTasks() {
	printf(SEPARATORI);
	printf("| ID  | PRIORITY   |      TASK NAME       |    REMAINING EXEC    | \n\r");
	printf(SEPARATORI);
	Task* t = head;
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

void printPrompt() {
	int index, casenumber, id = 0;
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
        choice = 2;
        executeTaskInHead();
        printTasks();
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
		break;
	case 6:
        changeSchedulingPolicy();
        printTasks();
		printPrompt();
		break;
	case 7:
		break;
	default:
		printf("Errore: comando non disponibile \n");
		printPrompt();
		break;
	}

}

int main(void) {
	printPrompt();
	system("PAUSE");
	return 0;
}
