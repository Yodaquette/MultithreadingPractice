#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "hello.c"

#define NUM_THREADS 5

int main(int argc,char *argv[])
{
	pthread_t threads[NUM_THREADS];
	
	int rc;
	long t;
	
	for(t = 0;t < NUM_THREADS;t++)
	{
		printf("In main: creating thread %ld\n",t);
		rc = pthread_create(&threads[t],NULL,printHello,(void *)t);
		if(rc)
		{
			printf("ERROR; return code from pthread_create() is %d\n",rc);
			exit(-1);
		}
	}
	
	// exit pthreads using default behavior
	pthread_exit(NULL);
	
	return 0;
}