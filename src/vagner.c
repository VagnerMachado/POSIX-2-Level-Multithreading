/*
 * 		vagner.c
 *  	Created on: Mar 13, 2020
 *
 *      Author: Vagner Machado
 *      		QC ID 23651127
 *      		CSC 340 - Queens College
 *      		Professor John Svadlenka
 *
 	 	 	 	 *** This program accomplishes ***

			0.  The program generates 10,000 random integers.
			1.  The program finds the lowest and maximum values using a single thread and prints the values.
			2.  Given the parameter -printInput, the program prints the 10,000 random integers to file.
			3.  The main process creates 5 threads and each is responsible for processing 2,000 of those random integers.
			4.  Each of the 5 threads creates 20 child threads. Each of 20 child threads finds the lowest value in 100 random integers.
			5.  This will find the 100 lowest values, 20 in each parent thread.
			6.  Each parent thread finds the lowest of its 20 integers, so there are 5 lowest values left.
			7.  The lowest 20 values for each child thread are printed.
			8.  The lowest 5 values for the parent threads are printed.
			9.  The lowest value out of lowest 5 is printed.
			10. The main thread scans the 5 lowest values to find the lowest value.
			11. All output is printed to file vagner-output.txt.

			NOTE: User can pass parameter -printInput to see all generated integers printed to file.

				***   COMPILATION INSTRUCTIONS   ***

			gcc -c vagner.c                  // compiles the file to object file.
			gcc -o app.exe vagner.o          // links object file to executable.
			OPTION 1: ./app.exe              // runs the program without printing the random integers.
			OPTION 2: ./app.exe -printInput  // runs the program printing the random input integers.
			OPTION 3: ./app.exe -help        // runs the program, prints these instructions to console and vagner-output.txt file.;
 */

//libraries
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

//DEFINE SOME CONSTANTS
#define INPUT 10000
#define FIRST_LEVEL_DATA 2000
#define SECOND_LEVEL_DATA 100
#define FIVE 5
#define HUNDRED 100
#define TWENTY 20

//declare function stubs
int * find_min_max_singleThread(int * array, int lowIndex, int highIndex);
int find_min_multiThread(int * array, int lowIndex, int highIndex);
void printArrayToFile(int * array, int lowIndex, int highIndex, int mod, FILE * output);
void helpNeeded(FILE * output);
void runner (void * param);
void splitter(void * param);

//declare global data buffers
int * topFive;
int * topHundred;
int * randomArray;

//declare output file
FILE * output;

/**
 * The main function sets up all variables declared above and allocates necessary memory.
 * Generates array of 10000 random integers and checks for any arguments passed in.
 * Finds the minimum and maximum values in random array and prints the values to file.
 * Creates five threads, each to be responsible to find the minimum in 2000 value sections of random array
 * When all threads join, the lowest 100 values and lowest 5 values are printed to file.
 * The main function running on the main thread finds the lowest of 5 values and prints it to file.
 */
int main(int argc, char ** argv)
{
	//open a file for writing the output
	output = fopen("vagner-output.txt", "w");
	fprintf(output, "\n*** Vagner Machado's Project #2 for CSC 340 ***");

	//allocate memory for the global arrays
	topFive = malloc(sizeof(int) * FIVE);
	topHundred = malloc(sizeof(int) * HUNDRED);
	randomArray = malloc(sizeof(int) * INPUT);

	time_t t;					// declare a time variable to use as seed for random
	srand((unsigned)time(&t));  // seed a randomizer

	/*
	Note: (RAND_MAX / 2) is subtracted from the randomly generated integers
	so there would be negative integers in the array. Otherwise, 0 would
	likely be the minimum all the time. It was the minimum in over 90% of cases
	before the division and subtraction mentioned above.
	 */

	//generate 10000 random integers.
	for(int i = 0; i < INPUT; i++)
		randomArray[i] = rand() - (RAND_MAX / 2);

	//option to print all 10000  random numbers, require help and check for invalid parameter
	if(argc > 1)
	{
		//prints random array to file
		if(strcmp(argv[1],"-printInput") == 0)
		{
			fprintf(output, "\n\n** All randomly generated numbers **\n");
			printArrayToFile(randomArray, 0, INPUT, 10, output);
		}

		//prints program information, compilation and execution to file
		else if(strcmp(argv[1],"-help") == 0)
		{
			fprintf(output, "\t\n\n***  Here is some helpful information ***");
			printf("\t\n*** Here is some helpful information ***");
			helpNeeded(output);
			fclose(output);
			return -1;
		}
		else
		{
			//invalid parameter passed, I decided to stop execution
			fprintf(output, "\n\n***  ERROR: %s is an invalid parameter ***", argv[1]);
			printf("\n***  ERROR: %s is an invalid parameter ***", argv[1]);
			helpNeeded(output);
			fclose(output);
			return -1;
		}
	}

	//array that points to minimum and maximum values found using single a thread
	int * linearResult = find_min_max_singleThread(randomArray, 0, 10000);

	//prints the minimumand maximum values to file
	fprintf(output, "\n\n*** Values found using a single thread ***\nThe minimum value is %d.\n"
			"The maximum value is %d.", linearResult[0], linearResult[1]);

	pthread_t tid [FIVE]; 		// the thread identifiers
	pthread_attr_t attr [FIVE]; // the thread attributes */

	char ** buffer [FIVE];             // buffer for the parameters in create thread
	int upperBound = FIRST_LEVEL_DATA; // used to manipulate parameter to each of five threads

	//initialize attributes, allocate memory for argument, create thread
	for(int i = 0; i < FIVE; i++)
	{
		pthread_attr_init(&attr[i]);							// set the thread attribute
		buffer[i] = malloc(sizeof(char) * 6);					// allocate memory for the argument
		sprintf(buffer[i], "%d", upperBound);					// print thread argument to buffer
		pthread_create(&tid[i], &attr[i], splitter, buffer[i]); // create thread with values set above
		upperBound += FIRST_LEVEL_DATA;							// increment argument for next thread
	}

	//wait for the five threads to join
	for(int i = 0; i < FIVE; i++)
		pthread_join(tid[i],NULL);

	//after all join, prints to file the lowest hundred values. A parent at a time prints twenty values each.
	for(int index = 0; index < FIVE; index++)
	{
		fprintf(output, "\n\n*** These are the twenty lowest values for parent %d ***", index + 1);
		printArrayToFile(topHundred, index * TWENTY, index * TWENTY + TWENTY, FIVE, output);
	}

	//prints to file to lowest five values
	fprintf(output, "\n\n*** These are the five lowest values ***");
	printArrayToFile(topFive, 0, FIVE, FIVE, output);

	//find lowest of top five lowest results from created threads
	int winner = find_min_multiThread(topFive, 0, FIVE);

	//prints the lowest value using multithreading
	fprintf(output, "\n\n*** The lowest value using multithreading is %d ***\n\n", winner);
	int res = fclose(output);

	//checks for error closing the file and provides output case true
	if(res < 0)
	{
		printf("\n*** Error closing file *** \n");
		return -1;
	}

	//successful run.
	printf("\n\n*** Program terminated successfully: See file output-vagner.txt for generated output *** \n");
	return 0;
}

/**
 * Creates twenty threads. Each of this threads calls runner with parameter calculated according to parameter passed into splitter function.
 * The minimum value in lower to upper range is written to global array topFive at position also calculated according to the parameter passed into function.
 * The parameter passed into this function indicates the upper index of random integers this function should operate upon.
 */
void splitter(void * upperParam)
{
	//cast the parameter
	char * paramCast = upperParam;

	//calculate the lower and upper indexes to operate upon
	int upper = atoi(paramCast);
	int lower = upper - FIRST_LEVEL_DATA;

	//calculate the index to write the result into
	int writeTo = lower / FIRST_LEVEL_DATA;

	//printf("\nLEVEL 1: LOWER DELIM: %5d  -- UPPER DELIM: %5d  --  WRITE_TO: %5d", lower, upper, writeTo);

	pthread_t tid [TWENTY];		  				// the thread identifiers
	pthread_attr_t attr [TWENTY]; 				// set of thread attributes
	char ** buffer [TWENTY];					// buffer for the parameters
	int upperBound = lower + SECOND_LEVEL_DATA; // used to manipulate the range passes for each thread

	//initialize attributes, allocate memory for argument, create thread
	for(int i = 0; i < TWENTY; i++)
	{
		pthread_attr_init(&attr[i]);                         // set the attributes
		buffer[i] = malloc(sizeof(char) * 6);                // allocate memory for string parameter to thread created  below
		sprintf(buffer[i], "\n%d", upperBound);              // print the parameter to string
		pthread_create(&tid[i], &attr[i], runner, buffer[i]);// create a thread with values set above
		upperBound += SECOND_LEVEL_DATA;                     // calculate the parameter for next thread
	}

	//wait for the threads to join
	for(int i = 0; i < TWENTY; i++)
		pthread_join(tid[i],NULL);

	//finds the lowest out of 20 lowest values for this thread and writes to topFive array
	topFive[writeTo] = find_min_multiThread(topHundred, lower / SECOND_LEVEL_DATA, upper / SECOND_LEVEL_DATA);
}

/**
 * finds the minimum value in the range calculated based on parameter passed into function.
 * The minimum value is written to global array topHundred at position also calculated according to the parameter passed into function.
 * The parameter passed into this function indicates the upper index of random integers this function should operate upon.
 */
void runner (void * upperParam)
{
	//cast the parameter
	char * paramCast = upperParam;

	//calculate the lower and upper indexes to operate upon
	int upper = atoi(paramCast);
	int lower = upper - SECOND_LEVEL_DATA;

	//calculate the index to write the lowest value to
	int writeTo = lower / 100;
	//finds the lowest value in the the array and within range passes as paramenter
	topHundred[writeTo] = find_min_multiThread(randomArray, lower, upper); //finds out of 100.

	//printf("\n   LEVEL 2: LOWER DELIM: %5d  -- UPPER DELIM: %5d  --  WRITE_TO %5d", lower, upper, writeTo);
}

/*
 * Finds the minimum value within the range for the array passed as parameter.
 * Linear search is more efficient than sort to find lowest.
 * @return - the minimum value in the array
 */
int find_min_multiThread(int * array, int lowIndex, int highIndex)
{
	int min = array[lowIndex];
	for(int i = lowIndex; i < highIndex; i++)
		if(array[i] < min)
			min = array[i];
	return min;
}

/**
 * Finds minimum and maximum values in array of random numbers using the parameter array and range provided
 * Linear search is more efficient than sort to find lowest and largest.
 * @return an integer array pointer to two values: min at position 0 and max at position 1.
 */
int * find_min_max_singleThread(int * array, int first, int last)
{
	int * minMax = malloc(sizeof(int) * 2); //allocate memory for the result
	int min, max = randomArray[0];			//default values to be compared against
	for(int i = first + 1; i < last; i++)   //basic linear sort
	{
		if(array[i] < min)
			min = array[i];
		if(array[i] > max)
			max = array[i];
	}
	minMax[0] = min;
	minMax[1] = max;
	return minMax;
}

/**
 * Prints to file output the values from lowIndex up to less than highIndex in array passed as parameter.
 * Mod parameter determined how many values per line.
 */
void printArrayToFile(int * array, int lowIndex, int highIndex, int mod, FILE * output)
{
	for(int i = lowIndex; i < highIndex; i++)
	{
		if(i % mod != 0 && i != 0) 	              //if new line is not needed print next value
			fprintf(output, "%12d ", array[i]);
		else                                      //goes to new line before printing next value
			fprintf(output, "\n%12d ", array[i]);
	}
}

/**
 * prints information about what the program accomplishes, compilation and execution instructions.
 * This function is called if user passes -help as argument to program of if argument passed to program is invalid.
 */
void helpNeeded(FILE * output)
{
	const char message [] = "\n\n***  This program accomplishes ***\n\n"
			"0.  The program generates 10,000 random integers.\n"
			"1.  The program finds the lowest and maximum values using a single thread and prints the values.\n"
			"2.  Given the parameter -printInput as parameter, the program prints the 10,000 random integers to file.\n"
			"3.  The main process creates 5 threads and each is responsible for processing 2,000 of those random integers.\n"
			"4.  Each of the 5 threads creates 20 child threads. Each of 20 child threads finds the lowest value in 100 random integers.\n"
			"5.  This will find the 100 lowest values, 20 in each parent thread.\n"
			"6.  Each parent thread finds the lowest of its 20 integers, so there are 5 lowest values left.\n"
			"7.  The lowest 20 values for each child thread are printed.\n"
			"8.  The lowest 5 values for the parent threads are printed.\n"
			"9.  The lowest value out of lowest 5 is printed.\n"
			"10. The main thread scans the 5 lowest values to find the lowest value.\n"
			"11. All output is printed to file vagner-output.txt.\n\n"
			"NOTE: User can pass parameter -printInput to see all generated integers printed to file.\n\n"
			"***   COMPILATION INSTRUCTIONS   ***\n\n"
			"\tgcc -c vagner.c                  // compiles the file to object file.\n"
			"\tgcc -o app.exe vagner.o          // links object file to executable.\n"
			"\tOPTION 1: ./app.exe              // runs the program without printing the random integers.\n"
			"\tOPTION 2: ./app.exe -printInput  // runs the program printing the random input integers.\n"
			"\tOPTION 3: ./app.exe -help        // runs the program and prints this message to console and vagner-output.txt file.\n\n";

	printf("%s", message);
	fprintf(output, message);

}
