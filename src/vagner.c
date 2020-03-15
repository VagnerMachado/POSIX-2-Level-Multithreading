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
int find_min_multiThread(int lowIndex, int highIndex);
void runner (void * param);
void splitter(void * param);

//declare global data buffers
int * topFive;
int * topHundred;
int * randomArray;

int main()
{
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

	int * linearResult = find_min_max_singleThread(randomArray, 0, 10000);
	printf("*** Values found using linear search ***\nThe minimum value is %d.\n"
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

	printf("\nafter join main");

}





void splitter(void * upperParam)
{
	char * paramCast = upperParam;
	int upper = atoi(paramCast);
	int lower = upper - FIRST_LEVEL_DATA;
	int writeTo = lower / FIRST_LEVEL_DATA;
	printf("\nLEVEL 1: LOWER DELIM: %5d  -- UPPER DELIM: %5d", lower, upper);

	////////////////

	/* the thread identifier */
	pthread_t tid [TWENTY];

	pthread_attr_t attr [TWENTY]; /* set of thread attributes */


	/* set the default attributes of the threads */
	char ** buffer [TWENTY];
	int upperBound = lower + SECOND_LEVEL_DATA;

	for(int i = 0; i < TWENTY; i++)
	{
		pthread_attr_init(&attr[i]);
		buffer[i] = malloc(sizeof(char) * 6);
		sprintf(buffer[i], "\n%d", upperBound);
		//printf("\nLOWER DELIM: %5d  -- UPPER DELIM: %5d  -- UPPER2: %s", lower, upper, buffer[i]);
		pthread_create(&tid[i], &attr[i], runner, buffer[i]);
		upperBound += SECOND_LEVEL_DATA;
	}

	//wait for the threadS to exit
	for(int i = 0; i < TWENTY; i++)
		pthread_join(tid[i],NULL);


	printf("\nafter join in splitter parent %d", writeTo);



}
void runner (void * upperParam)
{
	char * paramCast = upperParam;
	int upper = atoi(paramCast);
	int lower = upper - SECOND_LEVEL_DATA;
	int writeTo = lower / 100;
	printf("\n   LEVEL 2: LOWER DELIM: %5d  -- UPPER DELIM: %5d  --  WRITE_TO %5d", lower, upper, writeTo);
	topHundred[writeTo] = find_min_multiThread(lower, upper);


}

int find_min_multiThread(int lowIndex, int highIndex)
{
	int min = randomArray[lowIndex];
	for(int i = lowIndex; i < highIndex; i++)
		if(randomArray[i] < min)
			min = randomArray[i];
	printf("\n      Min in low index %d: %d", lowIndex, min);
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



