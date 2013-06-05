#include "stdlib.h"
#include "stdio.h"
#include "errno.h"

#define FILENAME "todo.txt"
#define MAX_ITEMS 20
#define MAX_DESC_LEN 96



/*Declarations*/
typedef struct ToDoItem
{	
	char desc[MAX_DESC_LEN];
	uint prio;
	struct ToDoItem *next;

} ToDoItem;

void printItems();
void insertItem(ToDoItem item);
void deleteItem(ToDoItem item);
void init();
void quit(const char *message);






/*Variables*/
ToDoItem *firstItem;





/* Functions */
void printItems()
{
	ToDoItem *current = firstItem;
	while(current != NULL){
		printf("Prio: %d\tDesc: %s\n",current->prio,current->desc);
		current = current->next;
	}


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



void init()
{
	FILE *file = fopen(FILENAME,"r");

	if(!file){
		if(fopen(FILENAME,"w")){
			printf("No ToDos found!\n");
			exit(0);
		} else {
			quit("Error creating todo.txt!");
		}

		quit("Error opening todo.txt");
	}

	char buffer[96];

	if(!fgets(buffer,95,file)){
		if(feof){
			printf("No ToDos found!\n");
			exit(0);
		}
		quit("Error reading file");
	}
	firstItem = malloc(sizeof(ToDoItem));
	sscanf(buffer,"%d %[^\n]", &firstItem->prio, firstItem->desc);

	ToDoItem *last = firstItem;

	while(fgets(buffer,128,file)){
		ToDoItem *current = malloc(sizeof(ToDoItem));
		sscanf(buffer,"%d %[^\n]", &current->prio, current->desc);
		last->next = current;
		last = current;

	}
	fclose(file);


}


int main(int argc, char const *argv[])
{
	if(argc == 1){
		init();
		printItems();
	}



	return 0;
}

