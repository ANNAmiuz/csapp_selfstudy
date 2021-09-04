#include "csapp.h"

static int timeout = -1;
char * res = NULL;

struct vargp{
    char * s;
    int size;
    FILE * stream;
};

void * thread1(void * vargp){
    sleep(5);
    timeout = 1;
    pthread_cancel(Pthread_self());
}

void * thread2(void * vargp){
    struct vargp vars = *(struct vargp *) vargp;
    res = fgets(vars.s,vars.size,vars.stream);
    timeout=0;
    pthread_cancel(pthread_self());
}

char *tfgets(char *s, int size, FILE *stream)
{
    struct vargp vars;
    vars.s = s;
    vars.size=size;
    vars.stream=stream;
    pthread_t tid;
    pthread_create(&tid, NULL, thread1, NULL);
    pthread_create(&tid, NULL, thread2, (void*)&vars);
    while (timeout==-1);
    switch(timeout){
        case 0: return res;
        case 1: return NULL;
    }
}

int main(void)
{
	char buf[MAXLINE];
	
	if (tfgets(buf, MAXLINE, stdin) == NULL)
		printf("BOOM!\n");
	else
		printf("%s", buf);
	
	exit(0);
}