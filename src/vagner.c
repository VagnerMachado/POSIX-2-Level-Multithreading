/*
 * vagner.c
 *
 *  Created on: Mar 13, 2020
 *      Author: vagner
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

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
void printArray(int * array, int lowIndex, int highIndex, int mod);
void runner (void * param);
void splitter(void * param);

//declare global data buffers
int * topFive;
int * topHundred;
int * randomArray;

int main()
{
	printf("\n*** Vagner Machado's Project #2 for CSC 340 ***");
	topFive = malloc(sizeof(int) * FIVE);
	topHundred = malloc(sizeof(int) * HUNDRED);
	randomArray = malloc(sizeof(int) * INPUT);
	time_t t;					// declare a time variable to use as seed for random
	srand((unsigned)time(&t));  // seed a randomizer

	/*
	Note: (RAND_MAX / 2) is subtracted from the randomly generated integers
	so there would be negative integers in the array. Otherwise, 0 would
	likely be the minimum all the time. It was the minimum in about 90% of cases
	before the step above.
	 */

	//generate 10000 random integers.
	for(int i = 0; i < INPUT; i++)
		randomArray[i] = rand() - (RAND_MAX / 2);

	//uncomment two lines below to see all random numbers
	/* printf("\n\n** All randomly generated numbers **\n");
	   printArray(randomArray, 0, INPUT, 10); */

	int * linearResult = find_min_max_singleThread(randomArray, 0, 10000);
	printf("\n\n*** Values found using a single thread ***\nThe minimum value is %d.\n"
			"The maximum value is %d.", linearResult[0], linearResult[1]);

	pthread_t tid [FIVE]; // the thread identifiers
	pthread_attr_t attr [FIVE]; // the thread attributes */

	char ** buffer [FIVE]; // buffer for the parameters in create thread
	int upperBound = FIRST_LEVEL_DATA; // used to manipulate parameter to each of five threads

	//initialize attributes, allocate memory for argument, create thread
	for(int i = 0; i < FIVE; i++)
	{
		pthread_attr_init(&attr[i]);							// set the thread attribute
		buffer[i] = malloc(sizeof(char) * 6);					// allocate memory for the argument
		sprintf(buffer[i], "%d", upperBound);					// add argument to buffer
		pthread_create(&tid[i], &attr[i], splitter, buffer[i]); // create thread
		upperBound += FIRST_LEVEL_DATA;							// increment argument
	}

	//wait for the five threads to exit
	for(int i = 0; i < FIVE; i++)
		pthread_join(tid[i],NULL);

	//prints the lowest hundred values, a parent at a time.
	for(int index = 0; index < FIVE; index++)
	{
		printf("\n\n*** These are the twenty lowest values for parent %d ***", index + 1);
		printArray(topHundred, index * TWENTY, index * TWENTY + TWENTY, FIVE);
	}

	printf("\n\n*** These are the five lowest values ***");
	printArray(topFive, 0, FIVE, FIVE);

	//find lowest of top five lowest results from created threads
	int winner = find_min_multiThread(topFive, 0, FIVE);

	//prints the lowest value using multithreading
	printf("\n\n*** The lowest value using multithreading is %d ***", winner);
}

void splitter(void * upperParam)
{
	char * paramCast = upperParam;
	int upper = atoi(paramCast);
	int lower = upper - FIRST_LEVEL_DATA;
	int writeTo = lower / FIRST_LEVEL_DATA;
	//printf("\nLEVEL 1: LOWER DELIM: %5d  -- UPPER DELIM: %5d  --  WRITE_TO: %5d", lower, upper, writeTo);

	pthread_t tid [TWENTY];		  				// the thread identifiers
	pthread_attr_t attr [TWENTY]; 				// set of thread attributes
	char ** buffer [TWENTY];					// buffer for the parameters
	int upperBound = lower + SECOND_LEVEL_DATA; // used to manipulate the range passes for each thread

	for(int i = 0; i < TWENTY; i++)
	{
		pthread_attr_init(&attr[i]);
		buffer[i] = malloc(sizeof(char) * 6);
		sprintf(buffer[i], "\n%d", upperBound);
		pthread_create(&tid[i], &attr[i], runner, buffer[i]);
		upperBound += SECOND_LEVEL_DATA;
	}

	//wait for the threadS to exit
	for(int i = 0; i < TWENTY; i++)
		pthread_join(tid[i],NULL);

	//finds the lowest out of 100 values
	topFive[writeTo] = find_min_multiThread(topHundred, lower / SECOND_LEVEL_DATA, upper / SECOND_LEVEL_DATA);
}
void runner (void * upperParam)
{
	char * paramCast = upperParam;
	int upper = atoi(paramCast);
	int lower = upper - SECOND_LEVEL_DATA;
	int writeTo = lower / 100;
	topHundred[writeTo] = find_min_multiThread(randomArray, lower, upper); //finds out of 100.
	//printf("\n   LEVEL 2: LOWER DELIM: %5d  -- UPPER DELIM: %5d  --  WRITE_TO %5d", lower, upper, writeTo);
}

/*
 * Finds the minimum value in the range for the array passed as parameter
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
 * Finds minimum and maximum values in array of random
 * numbers using the parameter range provided
 */
int * find_min_max_singleThread(int * array, int first, int last)
{
	int * minMax = malloc(sizeof(int) * 2); //allocate memory for the result
	int min, max = randomArray[0];			//default values to be compared against
	for(int i = first + 1; i < last; i++)
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
 * Prints the values from lowIndex to highIndex from array passed as parameter
 */
void printArray(int * array, int lowIndex, int highIndex, int mod)
{
	for(int i = lowIndex; i < highIndex; i++)
	{
		if(i % mod != 0 && i != 0)
			printf("%11d ", array[i]);
		else
			printf("\n%11d ", array[i]);
	}
}
