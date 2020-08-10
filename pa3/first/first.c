// HEADER LINK

#include "first.h"

// GLOBAL VARIABLES

// cache and memory event counts
int memory_reads = 0, memory_writes = 0, cache_hits = 0, cache_misses = 0;

// cache attributes
int number_of_sets = 0, number_of_blocks = 0;

// address bit attributes
int set_bits = 0, offset_bits = 0;

// PRINT FUNCTIONS

// prints the output as required by the assigment specifications
void printSimulationCounts(){
	printf("Memory reads: %d\n", memory_reads);
	printf("Memory writes: %d\n", memory_writes);
	printf("Cache hits: %d\n", cache_hits);
	printf("Cache misses: %d\n", cache_misses);
}


// print contents of the cache
void printCache(size_t** cache){
	int set, block;
	for (set = 0; set < number_of_sets; set++){
		printf("set #%d\n", set);
		// print the blocks within a set with a tab indentation
		for (block = 0; block < number_of_blocks; block++){
			printf("\tline %d: %zx\n", block, cache[set][block]);
		}
	}
}

// print remaining global variables
void printCacheInfo(){
	// print cache dimensions
	printf("Number of Sets: %d\n", number_of_sets);
	printf("Number of Blocks: %d\n", number_of_blocks);
	// print bits
	printf("Number of Set Index Bits: %d\n", set_bits);
	printf("Number of Offset Bits: %d\n", offset_bits);
}

// CREATE LOCAL CACHE

// fill local cache with NULL values 
void clearCache(size_t cache[number_of_sets][number_of_blocks]){
  	int set, block;
	for(set = 0; set < number_of_sets; set++){
    		for(block = 0; block < number_of_blocks; block++){
			// a NULL value is equivalent to an invalid tag
      			cache[set][block] = (size_t) NULL;
    		}
  	}
}

// VALID INPUT CHECK FUCTIONS

// checks if a given number is a power of 2
int isPowerOfTwo(int num){
	// continously divide a number by 2 until the number is no longer divisible by two (cannot be a power of two) or is equal or less than 2
	while ((num % 2 == 0) && (num > 2)){
		num /= 2;
	}
	// if the resulting number is 2, then the original number is a power of 2
	return (num == 2);
}

// ensure that input values are valid
void checkInputs(int cache_size, int block_size, char * cache_policy, char * associativity, FILE * trace){
  	// checks for preconditions 
	if (isPowerOfTwo(cache_size) == 0){
    		printf("ERROR: Cache size must be a power of 2\n");
    		exit(0);
  	}

  	if (isPowerOfTwo(block_size) == 0){
    		printf("ERROR: Block size must be a power of 2\n");
    		exit(0);
  	}

  	if (strcmp(cache_policy, "fifo") != 0 && strcmp(cache_policy,"lru") != 0){
    		printf("ERROR: Replacement policy must be either fifo or lru\n");
    		exit(0);
  	}

	// first character must be either d for direct or a for assoc
	if ((associativity[0] == 'd' || associativity[0] == 'a') == 0){	
    		printf("ERROR: Associativity must be direct, assoc, or assoc:n\n");
		exit(0);
	}

	if (trace == NULL){
		printf("ERROR: Trace file does not exist or is not formatted properly\n");
		exit(0);
	}
	// if the function does not call exit(0), then the inputs are valid
}

// ACCESS FUNCTIONS

// calculates the number of sets given the type of associativity
int calculateNumberOfSets(int cache_size, int block_size, char* associativity){
	
	// direct mapping
	if(strcmp(associativity, "direct") == 0){
		// exactly one line per set
    		return (cache_size / block_size);
  	}

	// full associative
	else if(strcmp(associativity, "assoc") == 0){
		// exactly one set
    		return 1;
  	}
	// else, it must be n-way assocaitive
	
	// create a temporary char array to convert to numbers later with only the numbers after the colon
  	char number[strlen(associativity) - 6];
  	// number index in the associativity string
	int n;
	for(n = 6; n < strlen(associativity); n++){
		// add number to char array
    		number[n - 6] = associativity[n];
  	}
	// convert the number string into an integer
  	return (cache_size / (block_size*atoi(number)));
}

// CACHE MANIPULATION FUNCTIONS

// linearly searches cache for address
int searchCache(size_t address, int numBlockOffBits, int numSetBits, int numBlocks, size_t cache[number_of_sets][number_of_blocks]){ 
  	// use bit manipulation to obtain the the corresponding set and tag
	size_t setIndex = (address>>numBlockOffBits)&((1<<numSetBits)-1);
  	// iterate through each block of the set
	int block;
	// iterate through each block
	for(block = 0; block < numBlocks; block++){	
		// if the tag equals the block, then return true
		if ((address >> (numSetBits + numBlockOffBits)) == cache[setIndex][block]){
      			return 1;
    		}
  	}
	// if it iterates through full cache without finding tag, then return false
  	return 0;
}

// update which block was the one most recently used
void updateLRU(size_t address, int numBlockOffBits, int numSetBits, int numBlocks, size_t cache[number_of_sets][number_of_blocks]){
  	// use bit manipulation to obtain the the corresponding set and tag
	size_t setIndex = (address>>numBlockOffBits)&((1<<numSetBits)-1);
	size_t tag = address >> (numBlockOffBits+numSetBits);
	int touched = 0, block;
  	// iterate through each block in the set except the last one to avoid off by one error
	for(block = 0; block < numBlocks-1; block++){
		// if the next block is empty, then don't update next
    		if(cache[setIndex][block+1] == (size_t) NULL){
		       	break;
		}
		// if the current block equals the tag or is untouched
    		if(cache[setIndex][block] == tag || touched){
      			// set touched to true
			touched = 1;
			// set current block  equal to the next block
      			cache[setIndex][block] = cache[setIndex][block+1];
     		}
  	}
	// set block to tag	
  	cache[setIndex][block] = tag;
} 

// adds address tag to cache
void addToCache(size_t address, int numBlockOffBits, int numSetBits, int numBlocks, size_t cache[number_of_sets][number_of_blocks]){ 
  	// use bit manipulation to obtain the the corresponding set and tag
	size_t setIndex = (address>>numBlockOffBits)&((1<<numSetBits)-1);
	int isAdded = 0, block;
	// insert address into cache
  	for(block = 0; block < numBlocks; block++){
    		// a NULL block represents an invalid bit => can insert without eviction
		if(cache[setIndex][block] == (size_t) NULL){
      			// set block location to tag
			cache[setIndex][block] = address >> (numBlockOffBits+numSetBits);
      			// set isAdded to true
			isAdded = 1;
			// exit loop
      			break;
    		}
  	}
  	if(isAdded == 0){ //this means that the set is full
    	//shift everything one left
    		for(block = 1; block < numBlocks; block++){
      			cache[setIndex][block - 1] = cache[setIndex][block];
    		}
		// sets last block to tag
    		cache[setIndex][numBlocks - 1] = address >> (numBlockOffBits+numSetBits);
  	}
}

// read from trace file and read/write addresses 
void updateCache(FILE * trace_file, int numBlockOffBits, int numSetBits, int numBlocks, char* replacement_policy, size_t cache[number_of_sets][number_of_blocks]){	
	int isLRU = (strcmp("lru", replacement_policy) == 0);
	// will either be 'R' for read  or 'W' for write
	char operation;
	size_t address;
	// reads until end of file
  	while((fscanf(trace_file,"%c %zx\n", &operation, &address) != EOF) && (operation != '#')){
    		// always increment memory writes if it is a write operation
		if(operation != 'R') {
			memory_writes++;
    		}
		// if address is in cache, then hit
		if(searchCache(address, numBlockOffBits, numSetBits, numBlocks, cache) == 1){
      			// if the address is already stored in the cache, then it is a hit
			cache_hits++;
			// if the replacement policy is LRU, then run LRU function
      			if(isLRU != 0){
				// update which block has been most recently used
        			updateLRU(address, numBlockOffBits, numSetBits, numBlocks, cache);
    			}
    		}
		// else is a miss
		else {
      			cache_misses++;
      			memory_reads++;
			// insert
      			addToCache(address, numBlockOffBits, numSetBits, numBlocks, cache);
    		}
	}
}

// MAIN FUNCTION

// parse arguments and call functions
int main(int argc, char *argv[]){

	// ensures that there are exactly 6 inputs
  	if(argc != 6){
    		printf("ERROR: Must have exactly 6 inputs");
    		// equivalent to exiting the program with exit(0)
		return 1;
  	}

	// read arguments
  	int cacheSize = atoi(argv[1]), blockSize = atoi(argv[2]);
  	char* replacementPolicy = argv[3];
	char* associativity = argv[4];
  	FILE * infile = fopen(argv[5], "r");
  
	// ensure that values are valid
	// checkInputs(cache_size, block_size, cache_policy, associativity, trace_file);

	// calculate cache attributes
  	number_of_sets = calculateNumberOfSets(cacheSize, blockSize, associativity);
  	number_of_blocks = cacheSize/(blockSize * number_of_sets);

	// calculate number of bits for set and offset block
  	set_bits = log(number_of_sets)/log(2);
	offset_bits = log(blockSize)/log(2);

	// declare cache
	size_t cache[number_of_sets][number_of_blocks];
	
	// set values within cache set to 0
	clearCache(cache);

	// read from file 
  	updateCache(infile, offset_bits, set_bits, number_of_blocks, replacementPolicy, cache);

	// print out output specified by project description	
	printSimulationCounts();

	// close file
	fclose(infile);

	// if program returns 0 here, then the program was successfully executed without error
	return 0;
}
