#include "stdlib.h"
#include "stdio.h"
#include "errno.h"
#include "string.h"

#define FILENAME "/home/ponken/bin/todo_res/todo.txt"
#define FILENAME_TMP "/home/ponken/bin/todo_res/todo_tmp.txt"
#define MAX_ITEMS 50
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

	//Init setIDs
	for(i = 0;i<MAX_ITEMS;i++){
		setIDs[i] = 0;
	}
	ToDoItem *current = firstItem;

	//Get all used IDs
	while(current !=NULL){
		setIDs[current->id - 1] = 1;
		current = current->next;
	}

	//Then set the global ID array
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

	//Write all items that are not to be deleted in a new file
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

	//Remove original file, rename new file 
	remove(FILENAME);
	rename(FILENAME_TMP,FILENAME);



}



void insertItem(char *argv[],int argc)
{


	//Assemble item description
	char desc_c[MAX_DESC_LEN];
	strcpy(desc_c,argv[2]);
	int i;
	for(i = 3;i<argc;i++){
		strcat(desc_c," ");
		strcat(desc_c,argv[i]);
	}

	unsigned int id = getNextId();
	if(!id){
		//printf("List is full, delete an item before adding a new one\n");
		quit("List is full, delete an item before adding a new one\n");
	}


	FILE *file = fopen(FILENAME,"a");
	if(!file){
		quit("Error opening file");
	}



	char buffer[strlen(desc_c)+1];
	strcpy(buffer,desc_c);

	if(fprintf(file, "%u %s\n",id,buffer) < 0){
//		ids[id-1] = 0;
		quit("Error writing to file");
	}
	

	fclose(file);

}


//Reads the existing items from file and puts them into a double linked list, sorted by id
void init(FILE *f)
{	
	FILE *file = f;
	char buffer[MAX_DESC_LEN];

	if(!fgets(buffer,95,file)){
		if(!feof){
			quit("Error reading file");	
		}
	} else {


		firstItem = malloc(sizeof(ToDoItem));
		sscanf(buffer,"%u %[^\n]", &firstItem->id, firstItem->desc);

		firstItem->prev = NULL;

		ToDoItem *last = firstItem;

		while(fgets(buffer,MAX_DESC_LEN,file)){
			ToDoItem *current = malloc(sizeof(ToDoItem));
			sscanf(buffer,"%d %[^\n]",&current->id,current->desc);

			//biggest is the item the item that is just bigger than the current (next biggest)
			ToDoItem *biggest = firstItem;

			while(biggest->id <= current->id && biggest->next != NULL){
				biggest = biggest->next;
			}

			//If current does not have a "biggest", as in there is no item with a bigger id than current
			if(current->id >= biggest->id){
				//attach behind biggest
				current->prev = biggest;
				current->next = biggest->next;
				biggest->next = current;
				//if biggest was not last element, which really shouldn't be possible
				if(current->next != NULL){
					current->next->prev = current;
				}
			} else {
				//attach in front of biggest
				current->next = biggest;
				current->prev = biggest->prev;
				biggest->prev = current;

				if(current->prev == NULL){
					firstItem = current;
				} else {
					current->prev->next = current;

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

void swapIDs(unsigned int id1, unsigned int id2)
{
	FILE *file = fopen(FILENAME_TMP,"w");

	if(!file){
		quit("Error opening file");
	}

	ToDoItem *current = firstItem;
	ToDoItem *item1;
	ToDoItem *item2;
	int i;

	//Find Items to swap
	while(current != NULL){
		if(current-> id == id1){
			item1 = current;
		}
		if(current->id == id2){
			item2 = current;
		}
		current= current->next;

	}

	item1->id = id2;
	item2->id = id1;

	current = firstItem;

	while(current != NULL){
			i = fprintf(file, "%u %s\n", current->id, current->desc);
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

		if(!strcmp(argv[1],"swap")){
			if(argc<4){
				printf("Usage: todo swap <id1> <id2>\n");
			} else {
				swapIDs(atoi(argv[2]), atoi(argv[3]));
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

