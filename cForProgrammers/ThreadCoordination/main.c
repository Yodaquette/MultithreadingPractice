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
#include "boss.c"
#include "worker.c"

//#define NUM_THREADS 6
//#define BUFFER_SIZE 25
//#define NUM_TASKS 100

// Shared resource that the
// threads will work with
struct resource
{
	#define NUM_THREADS 6
	#define BUFFER_SIZE 25
	#define NUM_TASKS 100
	
	int addrAdd = 0;
	int addrRem = 0;
	int totElem = 0;
	int tasksComplete = 0;
	
	int *numList[BUFFER_SIZE];
	
	pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;			// Mutex lock for buffer
	pthread_cond_t condWorker = PTHREAD_COND_INITIALIZER;	// Worker waits condition
	pthread_cond_t condBoss = PTHREAD_COND_INITIALIZER;		// Boss waits on this condition
};

int main(int argc,char *argv[])
{
	int threadID = 0;
	struct resource shared_resource;
	shared_resource = mallac();
	
	pthread_t thread[NUM_THREADS];
	pthread_attr_t attr;
	
	// Initialize and set thread detach attribute
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
	
	// Keep running until all tasks are complete
//	while(tasksComplete <= NUM_TASKS)
//	{
		// Create threads
		for(threadID = 0; threadID < NUM_THREADS; threadID++)
		{
			// Create Boss thread
			if(threadID == 0)
			{
				printf("Main: creating boss thread %i\n",threadID);
				
				// Check that return value of pthread_create() denotes "success"
				if(pthread_create(thread[threadID],&attr,produce,(void *)&shared_resource) != 0)
				{
					printf("\n\tFailed to create thread %i\n",threadID);
					exit(1);
				}
			}
			
			printf("Main: creating thread %i\n",threadID);
			
			// Check that return value of pthread_create() denotes "success"
			if(pthread_create(thread[threadID],&attr,consume,(void *)&shared_resource) != 0)
			{
				printf("\n\tFailed to create thread %i\n",threadID);
				exit(1);
			}
		//}
		
		// Wait for created threads to exit
		for(threadID = 0; threadID < NUM_THREADS; threadID++)
		{
			pthread_join(thread[threadID],NULL);
		}
	}
	
	// Exit
	printf("Program quitting... goodbye!\n");
	return 0;
}