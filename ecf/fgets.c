// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/wait.h>
#include "csapp.h"
#include <signal.h>
// #include <stdio.h>
// #include <setjmp.h>

jmp_buf env;

void handler(int sig){
    Sio_puts("run out of time. \n");
    //write(STDOUT_FILENO, "Out of time\n", 1);
    longjmp(env, 1);
}



char * tfgets(char *str, int n, FILE *stream){
    alarm(5);
    signal(SIGALRM, handler);
    if (setjmp(env) == 0){
        fgets(str, n, stream);
    }
    else if (setjmp(env) == 1){
        return NULL;
    }
    return str;
}

int main(){
    int n = 50;
    char  buf[n];
    tfgets(buf, n, stdin);
    if (buf != NULL) 
        printf("successful input");
    return 0;
}