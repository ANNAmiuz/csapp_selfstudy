#include "csapp.h"

//using processes, signals, and nonlocal jump

jmp_buf buf;

void sig_CHLD_handler(int sig){
    siglongjmp(buf, 0);
}

//times out after 5s
char *tfgets(char *s, int size, FILE *stream)
{
    sigset_t onemask;
    int res;
    if (res = signal(SIGCHLD, sig_CHLD_handler)==SIG_ERR) exit(0);
    
    if (Fork()==0){
        sleep(5);
        exit(0);
    }
    switch(sigsetjmp(buf, 1)){
        case 0: return fgets(s, size, stream);
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
