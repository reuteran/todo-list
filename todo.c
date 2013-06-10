#include "stdlib.h"
#include "stdio.h"
#include "errno.h"
#include "string.h"

#define FILENAME "/home/ponken/bin/todo_res/todo.txt"
#define FILENAME_TMP "/home/ponken/bin/todo_res/todo_tmp.txt"
#define MAX_ITEMS 20
#define MAX_DESC_LEN 256



/*Declarations*/
typedef struct ToDoItem
{	
	char desc[MAX_DESC_LEN];
	unsigned int id;
	struct ToDoItem *next;
	struct ToDoItem *prev;

} ToDoItem;

void printItems();
void insertItem(char *argv[], int argc);
void deleteItem(unsigned int del_id);
void init(FILE *file);
void quit(const char *message);
int getNextId();
void initIDs();
FILE *initFile();
void cleanUp();






/*Variables*/
ToDoItem *firstItem;
int ids[MAX_ITEMS];




/* Functions */
void printItems()
{
	ToDoItem *current = firstItem;
	while(current != NULL){
		printf("ID: %d \tDesc: %s\n",current->id,current->desc);
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

void cleanUp() 
{
	ToDoItem *current = firstItem;
	ToDoItem *temp;
	while(current != NULL){
		temp = current->next;
		free(current);
		current = temp;
	}
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
	cleanUp();
	exit(1);
}

/*
void changePrio(unsigned int c_id, unsigned int c_prio)
{
	FILE *file = fopen(FILENAME_TMP,"w");
	if(!file){
		quit("Error opening file");
	}
	ToDoItem *current = firstItem;
	int i;
	while(current != NULL){
		if(current->id == c_id){
			current->prio = c_prio;
		}
		i = fprintf(file,"%u %u %s\n", current->id, current->prio, current->desc);
		if(i<0){
				fclose(file);
				remove(FILENAME_TMP);
				quit("Error writing to file");
		}

		current = current->next;
	}

	fclose(file);
	remove(FILENAME);
	rename(FILENAME_TMP,FILENAME);



}
*/
void deleteItem(unsigned int del_id)
{


	FILE *file = fopen(FILENAME_TMP,"w");

	if(!file){
		quit("Error opening file");
	}

	ToDoItem *current = firstItem;
	int i;

	while(current != NULL){
		if(current->id != del_id){
			i = fprintf(file, "%u %s\n", current->id, current->desc);
			if(i<0){
				fclose(file);
				remove(FILENAME_TMP);
				quit("Error writing to file");
			}
		}
		current = current->next;

	}

	fclose(file);
	remove(FILENAME);
	rename(FILENAME_TMP,FILENAME);



}



void insertItem(char *argv[],int argc)
{

	char desc_c[MAX_DESC_LEN];

	strcpy(desc_c,argv[2]);

	int i;
	for(i = 3;i<argc;i++){
		strcat(desc_c," ");
		strcat(desc_c,argv[i]);
	}


	FILE *file = fopen(FILENAME,"a");

	if(!file){
		quit("Error opening file");
	}

	unsigned int id = getNextId();
	if(!id){
		printf("List is full, delete an item before adding a new one\n");
		exit(0);
	}


	char buffer[strlen(desc_c)+1];
	strcpy(buffer,desc_c);

	if(fprintf(file, "%u %s\n",id,buffer) < 0){
//		ids[id-1] = 0;
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
		sscanf(buffer,"%u %[^\n]", &firstItem->id, firstItem->desc);

		firstItem->prev = NULL;

		ToDoItem *last = firstItem;

		while(fgets(buffer,128,file)){
			ToDoItem *current = malloc(sizeof(ToDoItem));
			sscanf(buffer,"%d %[^\n]",&current->id,current->desc);
			ToDoItem *biggest = last;


			//Find first Item that has a prio bigger than the one be inserted (current)
			while(current->id >= biggest->id && biggest->next != NULL){
				biggest = biggest->next;
			}

			//Now current has to be inserted before biggest
			//Insert it 
			if(biggest->next == NULL && current->id >= biggest->id){
				biggest->next = current;
				current->prev = biggest;
				current->next = NULL;
			} else {
				if(current->id < biggest->id){
					biggest->prev->next = current;
					current->prev = biggest->prev;
					biggest->prev = current;
					current->next = biggest;
				}
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
	} else {


		if(!strcmp(argv[1],"add")){
			if(argc<3){
				printf("Usage: todo add [<priority>] <text>\n");
			} else {
				insertItem(argv,argc);
			}
		}

		if(!strcmp(argv[1],"del")){
			if(argc<3){
				printf("Usage: todo del <id>\n");
			} else {
				deleteItem(atoi(argv[2]));
			}
		}

		/*
		if(!strcmp(argv[1],"prio")){
			if(argc < 4){
				printf("Usage: todo prio <id> <new_prio>\n");
			} else {
				changePrio(atoi(argv[2]), atoi(argv[3]));

			}


		}
		*/
	}
	cleanUp();



	return 0;
}

