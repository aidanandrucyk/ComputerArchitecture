#include <stdio.h>
#include <stdlib.h>

#define SIZE 10000

// create node struct
typedef struct Node {	
	int value;
	struct Node *next;
} Node;

void insert(Node **hashTable, int *key) {

	// needed to counteract negative values
	int index = *key % SIZE;
	if (index < 0){
		index *= -1;
	}
	
	// create pointer at index
	Node * ptr = hashTable[index];

	// check if duplicated
	while (ptr != NULL) {
		// check to see if the ptr value is the same as the target value
		if (ptr->value == *key) {
			printf("duplicate\n");
			return;
		}
		// move pointer to the next val at the index
		ptr = ptr->next;
	}
	
	// create new node at the front of the array
        Node *item = (Node*) malloc(sizeof(Node));
        item->value = *key;
        item->next = hashTable[index];
        hashTable[index] = item;
	
	printf("inserted\n");
	return;

}

void search(Node **hashTable, int *key) {

	// create index integer to counteract possibility of negative index (segmentation fault)
	int index = *key % SIZE;
	if (index < 0) {
		index *= -1;
	}

	// create a pointer at the hash location
	Node * ptr = hashTable[index];

	// check to see value is present
	while (ptr != NULL){
		// check to see if the ptr value is the same as the target value
		if (ptr->value == *key){
			printf("present\n");
			return;
		}
		// move pointer to the next val at the index
		ptr = ptr->next;		
	}
	// indicates that pointer is NULL and therefore value is absent
	printf("absent\n");
	return;
}

int main(int argc, char *argv[]){
	
	// check if there are enough arguments
	if (argc < 2) {
		return -1;
	}
	
	// open file
	char *filename = argv[1];
	FILE *infile = fopen(filename,"r");

	// create input variables 
	char command[2];
	int value;

	// create an array of pointers to linked lists of size 10,000
	// using calloc to ensure each item is initialized to NULL
	Node ** hashTable = (Node**) calloc(SIZE, sizeof(Node*));
	int i;

	// manually performing calloc
	for (i = 0; i < SIZE; i++) {
		hashTable[i] = NULL;
	}

	while (fscanf(infile, "%s\t%d", command, &value) != EOF) {
		// prints inputs
		// printf("%s\t%d\n", command, value);

		// based on the first entry of command, search or insert value
		// if the char is s, then seach
		if (command[0] == 's'){
			search(hashTable, &value);	
		}
		// if the char is i, then insert
		else {
			insert(hashTable, &value);
		}	
	}

	// free dynamic memory from heap
	// free nodes in linked list
	for (i = 0; i < SIZE; i++){
		Node * ptr = hashTable[i];
		while (ptr != NULL){
			Node * temp = ptr->next;
			free(ptr);
			ptr = temp;
		}
	}
	// free hashTable
	free(hashTable);		

	fclose(infile);
	return 0;
}
