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
#define INPUT 10000
#define FIRST_LEVEL_DATA 2000
#define SECOND_LEVEL_DATA 100

int * find_min_max_singleThread();
void printer (void * param);
void splitter(void * param);

int topFive [5];
int topHundo [100];
int randomArray [INPUT];

int main()
{
	time_t t;
	srand((unsigned)time(&t));
	for(int i = 0; i < INPUT; i++)
		randomArray[i] = rand() - (RAND_MAX / 2);
	int * linearResult = find_min_max_singleThread(0, 9999);
	printf("*** Using linear search ***\nThe minimum value is %d.\n"
			"The maximum value is %d.", linearResult[0], linearResult[1]);
	///////////////////

	/* the thread identifier */
	pthread_t tid [5];

	pthread_attr_t attr [5]; /* set of thread attributes */


	/* set the default attributes of the threads */
	char ** buffer [5];
	int upperBound = FIRST_LEVEL_DATA;

	for(int i = 0; i < 5; i++)
	{
		pthread_attr_init(&attr[i]);
		buffer[i] = malloc(sizeof(char) * 20);
		sprintf(buffer[i], "%d", upperBound);
		pthread_create(&tid[i], &attr[i], splitter, buffer[i]);
		upperBound += FIRST_LEVEL_DATA;
	}

	for(int i = 0; i < 5; i++)
		pthread_join(tid[i],NULL);

		/* wait for the thread to exit */


}

int * find_min_max_singleThread(int first, int last)
{
	int * minMax = malloc(sizeof(int) * 2);
	int min, max = randomArray[0];
	for(int i = first; i < last; i++)
	{
		if(randomArray[i] < min)
			min = randomArray[i];
		if(randomArray[i] > max)
			max = randomArray[i];
	}
	minMax[0] = min;
	minMax[1] = max;
	//printf("MIN %d, MAX: %d", minMax[0], minMax[1]);
	return minMax;
}

void splitter(void * upperParam)
{
	char * paramCast = upperParam;
	int upper = atoi(paramCast);
	int lower = upper - 2000;
	//printf("\nLOWER DELIM: %5d  -- UPPER DELIM: %5d", lower, upper);

	////////////////

	/* the thread identifier */
		pthread_t tid [20];

		pthread_attr_t attr [20]; /* set of thread attributes */


		/* set the default attributes of the threads */
		char ** buffer [20];
		int upperBound = lower + SECOND_LEVEL_DATA;

		for(int i = 0; i < 20; i++)
		{
			pthread_attr_init(&attr[i]);
			buffer[i] = malloc(sizeof(char) * 20);
			sprintf(buffer[i], "\n%d", upperBound);
			//printf("\nLOWER DELIM: %5d  -- UPPER DELIM: %5d  -- UPPER2: %s", lower, upper, buffer[i]);
			pthread_create(&tid[i], &attr[i], printer, buffer[i]);
			upperBound += SECOND_LEVEL_DATA;
		}

		for(int i = 0; i < 5; i++)
			pthread_join(tid[i],NULL);

			/* wait for the thread to exit */
	///////////////

}
void printer (void * param)
{
    char * bla = param;
	printf("%s", bla);
}

/*
void splitter(void * p)
{
	char low [10];
	char high [10];

	int input = 0;
	int place = 0;
	char param [100];
    strcpy(param, p);

	while(param[input] != ',')
		low[place++] = param[input++];
	low[place] = '\0';
	input++;
	place = 0;
	while(param[input] != '\0')
			high[place++] = param[input++];
	high[place] = '\0';

	printf("\n%s %s", low, high);
}
 */




