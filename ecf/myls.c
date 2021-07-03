#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char ** argv, char ** envp){
    pid_t pid;
    int res = -1;
    char * target = getenv("COLUMNS");  
    char * argv1[2] = {"/bin/ls", NULL};
    if (target == NULL){
        res = setenv("COLUMNS", "80", 0);
        if (res != 0) printf("error in setting envp");
    }
    if ((pid=fork()) == 0){
        execve("/bin/ls", argv1, envp);
        printf("error\n");
        exit(1);
    }
    else waitpid(pid, NULL, 0);
    return 0;
    
}