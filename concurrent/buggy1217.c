/* exit in the main thread is invoked before the peer thread terminated. */
#include "csapp.h"
void *thread(void *vargp);

int main()
{
	pthread_t tid;
	
	Pthread_create(&tid, NULL, thread, NULL);
	Pthread_exit(NULL);
}

void *thread(void *vargp)
{
	Sleep(1);
	printf("Hello, world!\n");
	return NULL;
}