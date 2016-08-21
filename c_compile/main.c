/*
	Andrew Goodman
	August 20, 2016

	Practice compiling a C file
*/
#include <string.h>

int main()
{
	char *hello = "Hello, world!";
	printf("Greeting message: %s\nLength: %i.\n",hello,strlen(hello));

	return 1;
}
