#include <stdio.h>
#include <stdlib.h>

void printArray(int array[], int * size){
	int i;
	for (i = 0; i < *size; i++){
		printf("%d\t", array[i]);
	}
	printf("\n");
}


// insertion sort from specified starting to ending index.
// int *arr is the SAME AS int arr[]
// key indicates the starting index of the sort (we want a temporary copy not a pointer to the original value because we do not want to change the original value)
void insertionSort(int array[], int *length, int ascending){

	// create local variable i that is one less than key
	// key should be intialized to 1 because first element is sorted within the touched elements by definition
	int i = 0, key = 1;
	// increment key until it reaches the end of the array (won't start if length is 0)
	while (key < *length){
		int k = key;
		// if ascending (1)
		if (ascending == 1) {
			while ((i >= 0) && (array[k] < array[i])){
				// swap elements at respective indexes
				int temp = array[k];
				array[k] = array[i];
				array[i] = temp;
				i--;
				k--;
			}
		}
		// else descending (0)
		else {
			while ((i >= 0) && (array[k] > array[i])) {
				// swap elements
				int temp = array[k];
                                array[k] = array[i];
                                array[i] = temp;
                                i--;
                                k--;
			}
		}
		// set i to key
		i = key;
		// increment key so i is one less than key		
		key++;
	}
}

int main(int argc, char *argv[]){

	// checks if there is a text file
	if (argc < 2) {
		return -1;
	}

	// gets file
	char *filename = argv[1];
	FILE *infile = fopen(filename, "r");

	// get array size
	int SIZE;
	fscanf(infile, "%d\n", &SIZE);
	
	// create array
	int * arr = (int*) malloc(sizeof(int) * SIZE);
	int i;
	for (i = 0; i < SIZE; i++){
		fscanf(infile, "%d", &arr[i]);
	}

	// close file
	fclose(infile);

	// get the count of the number of odd numbers in the array
	int countOfOdds = 0, countOfEvens;
	for (i = 0; i < SIZE; i++){
		if (arr[i] % 2 != 0){
			countOfOdds++;
		}
	}
	// rewritten expression of total = partial1 + partial2
	countOfEvens = SIZE - countOfOdds;
	
	// create odd array
	int *odds = (int*) malloc(sizeof(int) * countOfOdds);
	// create even array
	int *evens = (int*) malloc(sizeof(int) * countOfEvens);

	// put numbers in their respective new array	
	int oddIndex = 0, evenIndex = 0; 
	for (i = 0; i < SIZE; i++){
		// if odd, then add to odd array
		if (arr[i] % 2 != 0){
			odds[oddIndex] = arr[i];
			oddIndex++;
		}		
 		else {
			evens[evenIndex] = arr[i];
			evenIndex++;
		}
	}

	// sort odd numbers in ascending order
	insertionSort(odds, &countOfOdds, 1);
	// sort even numbers in descending order
	insertionSort(evens, &countOfEvens, 0);

	// put values from two arrays back into the original array
	for (i = 0; i < countOfOdds; i++){
		arr[i] = odds[i];
	}
	for (evenIndex = 0; i < SIZE; i++, evenIndex++){
		arr[i] = evens[evenIndex];
	}	

	// print array
	printArray(arr, &SIZE);
	
	// free dynamic memory
	free(arr);
	arr = NULL;
	free(odds);
	odds = NULL;
	free(evens);
	evens = NULL;

	// return 0 to indicate that the main method was executed sucessfuly
	return 0;
}
