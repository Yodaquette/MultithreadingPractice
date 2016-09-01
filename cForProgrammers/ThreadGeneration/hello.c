#include <pthread.h>
#include <stdio.h>

void printHello(void *threadID)
{
	long tid;
	tid = (long)threadID;
	printf("Hello there, it's me, thread #%ld\n",tid);
	pthread_exit(NULL);
}