#include "csapp.h"
#include <signal.h>
#include <stdio.h>
#define N 2

int main(){
    int status, i;
    pid_t pid;

    //parent creates N children
    for (i = 0; i < N; i++){
        if ((pid = fork()) == 0){
            //how to cause write?
            //write the read-only text
            char * ptr = 0;
            *ptr = 'A';
            }
        }
    
    //parent reaps N children in no particular order
    while ((pid = waitpid(-1, &status, 0)) > 0){
        if (WIFEXITED(status)) 
            printf("child %d terminate normally with exit status=%d\n", pid, WEXITSTATUS(status));
        else if (WIFSIGNALED(status)){
                char buffer[52];
                sprintf(buffer, "Child %d terminate by signal %d: ", pid, WTERMSIG(status));
                psignal(WTERMSIG(status), buffer);
        } 
    }

    //the only normal termination is if there are no more children
    if (errno!=ECHILD) perror("waitpid error");
    exit(0);
}