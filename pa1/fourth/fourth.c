#include <stdio.h>
#include <stdlib.h>

// print 2D array
void printMatrix(int numberOfRows, int numberOfColumns, double matrix[numberOfRows][numberOfColumns]){
	int i, j;
	for (i = 0; i < numberOfRows; i++) {
		for (j = 0; j < numberOfColumns; j++) {
			// separate each column by a tab
			printf("%lf\t", matrix[i][j]);
		}
		// go to new line representing new row
		printf("\n");
	}
}

// print 1D array
void printVector(int length, double vector[]) {
	int i;
	for (i = 0; i < length; i++){
		printf("%lf\t", vector[i]);
	}
	// print new line
	printf("\n");
}

// print 1D array
void printPrices(int length, double vector[length][1]) {
        int i;
        for (i = 0; i < length; i++){
		// round to the nearest integer
                printf("%0.0lf\n", vector[i][0]);
        }
}


// multiplies matrices
void matrixMultiplication(int rowOne, int colOne, double one[rowOne][colOne], int rowTwo, int colTwo, double two[rowTwo][colTwo], double product[rowOne][colTwo]){
	// ensure that the number of columns of the first matrix and the number of rows of the second column are equal
	if (colOne != rowTwo || rowOne < 1 || rowTwo < 1 || colOne < 1 || colTwo < 1) {
		printf("matrix multiplication: mismatched sizes\n");
		return;
	}

	int i, j, z;
	for (i = 0; i < rowOne; i++){
		for (j = 0; j < colTwo; j++) {
			// get the corresponding dot products to place in product matrix
			// make sure that all elements in product matrix are zero
			product[i][j] = 0;
			for (z = 0; z < colOne; z++){
				product[i][j] += one[i][z] * two[z][j];
			}
		}
	}
}

// multiplies a matrix and a vector
void matrixVectorProduct(int row, int col, double matrix[row][col], int vectorSize,  double vector[vectorSize], double product[row]){
	if (col != vectorSize || row < 1 || col < 1 || vectorSize < 1) {
		printf("matrix vector product: mismatched sizes\n");
		return;
	}
	int i, j;
	for (i = 0; i < row; i++) {
		// get the corresponding dot products to place in product vector
		product[i] = 0;
		for (j = 0; j < col; j++) {
			product[i] += matrix[i][j] * vector[j];
		}
	}
}

// make the last parameter to be the transpose of the original
void getTranspose(int row, int col, double original[row][col], double transpose[col][row]) {
	if (row < 1 || col < 1) {
		printf("transpose: mismatched sizes\n");
		return;
	}
	int i, j;
	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			// all off-diagonal elements are placed in transposed positions
			transpose[j][i] = original[i][j];
		}
	}
}

// make the identity matrix
void getEye(int size, double matrix[size][size]){
	// ensure that the size of the matrix is at least 1 by 1
	if (size < 1) {
		printf("identity: mismatched size\n");
	}
	int i, j;
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			// if on diagonal, make one else make zero
			matrix[i][j] = (i == j) ? 1 : 0;	
		}
	}
}

// obtains the inverse using Gauss-Jordan elimination
void getInverse(int size, double original[size][size], double inverse[size][size]) {
	// ensure that the size of the matrix is at least 1 by 1
	if (size < 1){
		printf("inverse: mismatched sizes\n");
		return;
	}
	// make the inverse matrix an identity matrix	
	getEye(size, inverse);
	// initialize index indicators
	int pivotRow, currRow, pivotCol = 0, currCol;
	// iterate down matrix starting from top of matrix
	for (pivotRow = 0; pivotRow < size; pivotRow++, pivotCol++) {
		// get constant so as to obtain the reciprocal of the first entry
		double originalValueOfPivot = (original[pivotRow][pivotCol] == 0) ? 1 : original[pivotRow][pivotCol] ;
		// normalize first entry of pivot row while maintaining row equality
		for (currCol = 0; currCol < size; currCol++){
			original[pivotRow][currCol] /= originalValueOfPivot;
			inverse[pivotRow][currCol] /= originalValueOfPivot;
		}
		// iterate down rows starting from one row below pivot row
		for (currRow = pivotRow+1; currRow < size; currRow++) {
			// calculate constant which would multiply with the pivot position to sum the corresponding values of the same column to be zero
			double constant = - (original[currRow][pivotCol] / original[pivotRow][pivotCol]);
			// sum the curr row with the pivot row multiplied by the constant 
			for (currCol = 0; currCol < size; currCol++){
				original[currRow][currCol] += constant*original[pivotRow][currCol];  
				inverse[currRow][currCol] += constant*inverse[pivotRow][currCol];
			}
		}	
	}
	// iterate up the matrix starting from the bottom of the matrix
	for (pivotRow = size-1, pivotCol= size-1; pivotRow >= 0; pivotRow--, pivotCol--){
		// iterate up the matrix starting from one row above the current pivot row
		for (currRow = pivotRow-1; currRow >= 0; currRow--){
			// calculate constant which would multiply with the pivot position to sum the corresponding values of the same column to be zero
			double constant = - (original[currRow][pivotCol] / original[pivotRow][pivotCol]);
			// sum the curr row with the pivot row multiplied by the constant
			for (currCol = size-1; currCol >= 0; currCol--){
				original[currRow][currCol] += constant*original[pivotRow][currCol];
                                inverse[currRow][currCol] += constant*inverse[pivotRow][currCol];
			} 			
		}
	}
}

int main(int argc, char *argv[]){
        // checks if there are the correct number of files 
        if (argc != 3) {
        	return -1;
        }
        // get train file
        // char *filename = argv[1];
        FILE *trainfile = fopen(argv[1], "r");
        
	// check if file is null
	if (trainfile == NULL) {
		return -1;
	}

	// create local variables
        int weights, attributes, points;
	// scan for number of columns / predictor variables
	fscanf(trainfile, "%d\n", &attributes);
	// add another column representing constant weight
	weights = attributes+1;
	// scan for number of rows / data points
	fscanf(trainfile, "%d\n", &points);
	
	// get dimensions
	// printf("%d (%d,%d)\n", attributes, weights, points);
        
	// create a 2D design matrix
        // double ** trainMatrix = (double**) malloc(sizeof(double*) * cols * rows);
	double design[points][weights];
	double weight[weights][1];
	double prices[points][1];
	
	// populate design matrix
	int i, j;
	for (i = 0; i < points; i++){
		for (j = 0; j < weights; j++){
			// temporary local variable to save values to
			double num;
			// fill first column with constant attribute values
			if (j == 0){
				design[i][j] = 1;
				fscanf(trainfile, "%lf ,", &num);
				prices[i][0] = num;
			}
			// if last column, then do not account for commas
			else if (j == attributes){
				fscanf(trainfile, "%lf\n", &num);
                                design[i][j] = num;
			}
			// all other columns must account for commas
			else {
				fscanf(trainfile, "%lf ,", &num);
				design[i][j] = num;
			}
		}
	}

        // close train file
        fclose(trainfile);

	// open test file
        FILE *testfile = fopen(argv[2], "r");

	// check if file exists
	if (testfile == NULL) {
		return -1;
	}

        // get number of data points
	int testPoints;
	fscanf(testfile, "%d\n", &testPoints);
        
	// create test matrix
	double test[testPoints][weights];
	
	//populate test matrix
	for (i = 0; i < testPoints; i++){
		for (j = 0; j < weights; j++) {
			// temporary local variable to save values to and read from
			double num;
			// if the first row, fill with one
			if (j == 0) {
				test[i][j] = 1;
			}
			// if the last column, no comma but new line
			else if (j == attributes){
				fscanf(testfile, "%lf\n", &num);
				test[i][j] = num;
			}
			// not last column => must account for space and comma
			else {
				fscanf(testfile, "%lf ,", &num);
				test[i][j] = num;
			}
		}
	}
	
	// close test file
	fclose(testfile);

	// W = (((X^T)(X))^-1)(x^T)(Y)
	
	// create a tranposed design matrix that can be reused
	double transpose[weights][points];
	getTranspose(points, weights, design, transpose);

	// get the inner product
	double innerProduct[weights][weights];
	matrixMultiplication(weights, points, transpose, points, weights, design, innerProduct);
	
	// get the inverse of the inner product
	double inverse[weights][weights];
	getInverse(weights, innerProduct, inverse);
	
	// multiply inverse of transposed design matrix and muted design matrix
	double inverseTransposeProduct[weights][points];
	matrixMultiplication(weights, weights, inverse, weights, points, transpose, inverseTransposeProduct);
	matrixMultiplication(weights, points, inverseTransposeProduct, points, 1, prices, weight);

	// get the predicted prices!
	double predictedPrices[testPoints][1];
	matrixMultiplication(testPoints, weights, test, weights, 1, weight, predictedPrices);	
	printPrices(testPoints, predictedPrices);

	// if returns 0, then function worked properly
	return 0;
}
