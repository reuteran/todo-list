#include "stdlib.h"
#include "stdio.h"
#include "errno.h"
#include "string.h"

#define FILENAME "todo.txt"
#define MAX_ITEMS 20
#define MAX_DESC_LEN 96



/*Declarations*/
typedef struct ToDoItem
{	
	char desc[MAX_DESC_LEN];
	uint prio;
	uint id;
	struct ToDoItem *next;
	struct ToDoItem *prev;

} ToDoItem;

void printItems();
void insertItem(char *prio, char *desc);
void deleteItem(uint del_id);
void init(FILE *file);
void quit(const char *message);
int getNextId();
void initIDs();
FILE *initFile();






/*Variables*/
ToDoItem *firstItem;
int ids[MAX_ITEMS];




/* Functions */
void printItems()
{
	ToDoItem *current = firstItem;
	while(current != NULL){
		printf("ID: %d \tPrio: %d \tDesc: %s\n",current->id,current->prio,current->desc);
		current = current->next;
	}


}

FILE *initFile()
{
	FILE *file = fopen(FILENAME,"r");

	if(!file){
		file = fopen(FILENAME,"w");
		if(!file){
			quit("Error creating file");
		
		}
	}
	return file;
}

void initIDs()
{

	int i;
	int setIDs[MAX_ITEMS];

	for(i = 0;i<MAX_ITEMS;i++){
		setIDs[i] = 0;
	}
	ToDoItem *current = firstItem;

	while(current !=NULL){
		setIDs[current->id - 1] = 1;
		current = current->next;
	}
	for(i = 0;i<MAX_ITEMS;i++){
		ids[i] = setIDs[i];
	}


/*
	char buffer[4];
	int buf_id;
	while(!fgets(buffer,4,file)){
		sscanf(buffer,"%d",&buf_id);
		setIDs[buf_id-1] = 1;
	}
*/

}


//Returns next id between 1 and 20.
int getNextId()
{
	int i = 0;
	for(i = 0;i<MAX_ITEMS;i++){
		if(!ids[i]){
			ids[i] = 1;
			return i+1;
		}
	}
	return 0;

}

void quit(const char *message)
{
	if(errno){
		perror(message);
	} else {
		printf("ERROR: %s\n",message);
	}
	exit(1);
}


void deleteItem(uint del_id)
{


}



void insertItem(char *prio_c, char *desc_c)
{

	uint prio = atoi(prio_c);



	if(prio > 100){
		printf("Priority has to be in [0,100]\n");
		exit(0);
	}
	FILE *file = fopen(FILENAME,"a");

	if(!file){
		quit("Error opening file");
	}

	uint id = getNextId();
	if(!id){
		printf("List is full, delete an item before adding a new one\n");
		exit(0);
	}


	char buffer[strlen(desc_c)+1];
	strcpy(buffer,desc_c);

	if(fprintf(file, "%d %d %s\n",id,prio,buffer) < 0){
		ids[id-1] = 0;
		quit("Error writing to file");
	}
	

	fclose(file);

}



void init(FILE *f)
{	
	FILE *file = f;
	char buffer[96];

	if(!fgets(buffer,95,file)){
		if(!feof){
			quit("Error reading file");	
		}
	} else {


		firstItem = malloc(sizeof(ToDoItem));
		sscanf(buffer,"%d %d %[^\n]", &firstItem->id, &firstItem->prio, firstItem->desc);

		firstItem->prev = NULL;

		ToDoItem *last = firstItem;

		while(fgets(buffer,128,file)){
			ToDoItem *current = malloc(sizeof(ToDoItem));
			sscanf(buffer,"%d %d %[^\n]",&current->id, &current->prio, current->desc);
			ToDoItem *biggest = last;

			while(current->prio > biggest->prio && biggest->prev != NULL){
				biggest = biggest->prev;
			}

			if(current->prio <= biggest->prio){
				current->next = biggest->next;
				current->prev = biggest;

				if(current->next == NULL){
					last = current;
				} else {
					
					current->next->prev = current;
				}

				biggest->next = current;
			} else {
				current->prev = NULL;
				current->next = biggest;
				biggest->prev = current;
			}



		}
		fclose(file);

		while(last->prev !=NULL){
			last = last->prev;
		}
		firstItem = last;

		initIDs();
	}



}


int main(int argc, char *argv[])
{
	FILE *file = initFile();
	init(file);
	if(argc == 1){
		printItems();
	} else if(argc>3){
		if(!strcmp(argv[1],"add")){
			insertItem(argv[2],argv[3]);
		}
		
	}



	return 0;
}

