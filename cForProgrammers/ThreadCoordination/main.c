/*
	Andrew Goodman
	August 31, 2016
 
	Write a program that initializes an array
	and then uses multiple threads to read from
	that array and remove read elements from it
 
	Main objective: Synchrozine threads using waits and joins
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
//#include "boss.c"
//#include "worker.c"

#define NUM_THREADS 6
#define BUFFER_SIZE 25
#define NUM_TASKS 100

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;			// Mutex lock for buffer
pthread_cond_t condWorker = PTHREAD_COND_INITIALIZER;	// Worker waits condition
pthread_cond_t condBoss = PTHREAD_COND_INITIALIZER;		// Boss waits on this condition

// Shared resource that the
// threads will work with
typedef struct
{
	int addrAdd;
	int addrRem;
	int totElem;
	int tasksComplete;
	int totWorkers;
	
	int numList[];
} resource;

resource shared_resource;

// Prototype functions
void *produce(void *resource);
void *consume(void *resource);

int main(int argc,char *argv[])
{
	shared_resource.addrAdd = 0;
	shared_resource.addrRem = 0;
	shared_resource.totElem	= 0;
	shared_resource.tasksComplete = 0;
	shared_resource.totWorkers = 0;
	
	int threadID = 0;
	pthread_t thread[NUM_THREADS];
	pthread_attr_t attr;
	
	printf("resource: %i \n",shared_resource.addrRem);
	
	// Initialize and set thread detach attribute
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
	
	// Create threads
	for(threadID = 0; threadID < NUM_THREADS; threadID++)
	{
		if(threadID == 0) // Create Boss thread
		{
			printf("Main: creating boss thread %i\n",threadID);
			
			// Check that return value of pthread_create() denotes "success"
			if(pthread_create(&thread[threadID],&attr,produce,&shared_resource) != 0)
			{
				printf("\n\tFailed to create thread %i\n",threadID);
				exit(1);
			}
		}
		else // Create Worker thread
		{
			printf("Main: creating thread %i\n",threadID);
			
			// Check that return value of pthread_create() denotes "success"
			if(pthread_create(&thread[threadID],&attr,consume,&shared_resource) != 0)
			{
				printf("\n\tFailed to create thread %i\n",threadID);
				exit(1);
			}
			shared_resource.totWorkers++;
		}
	}
	
	// Wait for created threads to exit
	for(threadID = 0; threadID < NUM_THREADS; threadID++)
	{
		pthread_join(thread[threadID],NULL);
	}
	
	// Exit
	printf("COB. See you tomorrow!\n");
	return 0;
}

void *produce(void *resource)
{
	int valToAdd = 0;
	
	while(1)
	{
		// Break out of loop if work is complete
		// and the Workers threads have quit
		if(shared_resource.tasksComplete == NUM_TASKS && shared_resource.totWorkers == 0)
		{
			break;
		}
		
		valToAdd = rand() % 100;
		
		// Insert into array
		pthread_mutex_lock(&m);
		// Overflow if array is full
		if(shared_resource.totElem > BUFFER_SIZE)
		{
			exit(1);
		}
		
		// Block if buffer is full
		while(shared_resource.totElem == BUFFER_SIZE)
		{
			pthread_cond_wait(&condBoss,&m);
		}
		
		// If this section reached then buffer not full
		// add element
		shared_resource.numList[shared_resource.addrAdd] = valToAdd;
		
		shared_resource.addrAdd++;
		// If value for addrAdd is out of range
		// then reset to 0
		if(shared_resource.addrAdd > BUFFER_SIZE)
		{
			shared_resource.addrAdd = 0;
		}
		
		shared_resource.totElem++;
		// Unlock
		pthread_mutex_unlock(&m);
		
		// Broadcast to workers that resource is available
		pthread_cond_broadcast(&condWorker);
		printf("Boss produced: %i\n",valToAdd);
	}
	
	// Exit
	printf("Boss quitting... time to go!\n");
	fflush(stdout);
	pthread_exit(0);
}

void *consume(void *resource)
{
	// Consumer threads do not terminate until the
	// total quota is complete
	while(shared_resource.tasksComplete < NUM_TASKS)
	{
		// Lock
		pthread_mutex_lock(&m);
		// Underflow
		if(shared_resource.totElem < 0)
		{
			exit(1);
		}
		
		// Block if buffer is empty
		while(shared_resource.totElem == 0)
		{
			pthread_cond_wait(&condWorker,&m);
		}
		
		// If executing this section, then buffer not empty
		// remove element
		printf("\tValue %i at index %i\n",shared_resource.numList[shared_resource.addrRem],shared_resource.addrRem);
		
		// Replace element with a -1 flag
		// to notify Boss index address is OK to overwrite
		shared_resource.numList[shared_resource.addrRem] = -1;
		
		// Read next element
		shared_resource.addrRem++;
		
		// If the next address to remove is out of range
		// then reset to index 0
		if(shared_resource.addrRem > BUFFER_SIZE)
		{
			shared_resource.addrRem = 0;
		}
		
		// Increase tasks complete
		if(shared_resource.tasksComplete < NUM_TASKS)
		{
			shared_resource.tasksComplete++;
		}
		
		// Reduce total elements
		shared_resource.totElem--;
		// Unlock
		pthread_mutex_unlock(&m);
		
		// Signal the Boss that we're done working
		pthread_cond_signal(&condBoss);
	}
	
	// Exit
	printf("Worker quitting... goodbye!\n");
	shared_resource.totWorkers--;
	pthread_exit(0);
}