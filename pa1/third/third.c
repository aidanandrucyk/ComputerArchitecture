#include <stdlib.h>
#include <stdio.h>

// sets the value of a bit at a specified index
void set(unsigned short * number, int * bitIndex, int * value){
	// set bit at bitIndex to 1
	if (*value) {
		*number |= 1 << *bitIndex;
	}
	// set bit at bitIndex to 0 	
	else {
		// only change value at index if element value is 1, otherwise leave it unchanged
		if ((*number & (1 << *bitIndex)) == (1 << *bitIndex)) { 
			*number ^= 1 << *bitIndex;
		}
	}
	printf("%hu\n", *number);
}

// changes the value of the least significant bit to its compliment
void comp(unsigned short * number, int * bitIndex){
	// if least significant bit is 1, set to 0
	if ((*number & (1 << *bitIndex)) == (1 << *bitIndex)){
		*number ^= 1 << *bitIndex;
	}
	// if least signigicant bit is 0, set to 1
	else {
		*number |= 1 << *bitIndex;
	}
	printf("%hu\n", *number);
}

// get the value of the least significant bit
void get(unsigned short * number, int * bitIndex){
	// bitwise operator for if number and 0001 == 1
	if ((*number & (1 << *bitIndex)) == (1 << *bitIndex)){
		printf("1\n");
	}
	// if does not resolve to 1 (true), then it must be 0 (false)
	else {
		printf("0\n");
	}
}

int main(int argc, char *argv[]){

	// check if there are enough arguments
        if (argc < 2) {
        	return -1;
        }
        
	// open file
        char *filename = argv[1];
        FILE *infile = fopen(filename,"r");
        
	// create variables
        char stringBuffer[5];
        unsigned short number;
	int bitIndex, value;

	// get the initial number
        fscanf(infile,"%hu" ,&number);
	// print the original number in decimal
	// printf("%d\n", number);

	// scan values from infle and get output
	while (fscanf(infile, "%s\t%d\t%d", stringBuffer, &bitIndex, &value) != EOF) {
        	// printf("%s\t%d\t%d\n", stringBuffer, bitIndex, value);
		// if the first char in the string is s, then execute set
		if (stringBuffer[0] == 's'){
			set(&number, &bitIndex, &value);
		}
		// if the first char in the string is c, then execute comp
		else if (stringBuffer[0] == 'c'){
			comp(&number, &bitIndex);

		}
		// if not the other two, must be get
		else {
			get(&number, &bitIndex);
		}
        }
        
	// close file
	fclose(infile);
        return 0;
}                                                                                             
