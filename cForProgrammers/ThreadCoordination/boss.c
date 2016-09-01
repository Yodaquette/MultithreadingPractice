/*
	Andrew Goodman
	August 31, 2016
 
	The Boss thread produce values for the
	Worker threads to consume
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void *produce(void *resource)
{
	int valToAdd = 0,tasksProduced = 0;
	
	while(resource->tasksComplete < resource->NUM_TASKS)
	{
		valToAdd = rand() % 100;
		
		// Insert into array
		pthread_mutex_lock(resource->m);
		
			// Overflow if array is full
			if(resource->totElem > resource->BUFFER_SIZE)
			{
				exit(1);
			}
		
			// Block if buffer is full
			while(resource->totElem == BUFFER_SIZE)
			{
				pthread_cond_wait(resource->condBoss,resource->m);
			}
		
			// If this section reached then buffer not full
			// add element
			resource->numList[resource->addrAdd] = valToAdd;
			resource->addrAdd = (resource->addrAdd + 1) % resource->BUFFER_SIZE;
			resource->totElem++;
		// Unlock
		pthread_mutex_unlock(resource->m);
		
		// Broadcast to workers that resource is available
		pthread_cond_broadcast(resource->condWorker);
		printf("Boss produced: %i\n",valToAdd);
		
	}
	
	// Exit
	printf("Boss quitting... goodbye!\n");
	fflush(stdout);
	//return 0;
	pthread_exit(0);
}