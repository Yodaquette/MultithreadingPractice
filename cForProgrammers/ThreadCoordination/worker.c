/*
	Andrew Goodman
	August 31, 2016
 
	The Worker thread reads and then removes values 
	from the shared resource
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void *consume(void *resource)
{
	// Consumer threads do not terminate until the
	// total quota is complete
	while(1)
	{
		// Lock
		pthread_mutex_lock(resource->m);
			// Underflow
			if(resource->totElem < 0)
			{
				exit(1);
			}
		
			// Block if buffer is empty
			while(resource->totElem == 0)
			{
				pthread_cond_wait(resource->condWorker,resource->m);
			}
		
			// If executing this section, then buffer not empty
			// remove element
			printf("Value %i at index %i\n",resource->numList[resource->addrRem],resource->addrRem);
			resource->numList[resource->addrRem] = -1;
			resource->tasksComplete++;
		// Unlock
		pthread_mutex_unlock(resource->m);
		
		// Signal the Boss that we're done working
		pthread_cond_signal(resource->condBoss);
	}
	
	// Exit
	//return 0;
	pthread_exit(0);
}