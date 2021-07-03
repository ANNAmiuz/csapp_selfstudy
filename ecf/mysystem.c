#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int mysystem(char * command){
    int status;
    pid_t pid, res;
    //extern char* envp[];

    //exit sh in child process
    if ((pid=fork()) == 0){
        char * argv[4] = {"sh","-c", command, NULL};
        execve("/bin/sh", argv, NULL);
    }

    if (waitpid(-1, &status, 0) > 0){
        //terminate by call exit
        if (WIFEXITED(status)){
            return WEXITSTATUS(status);
        }
    }
    if (WIFSIGNALED(status)){
        return WTERMSIG(status);
    }
}

//test
int main(int argc, char ** argv, char ** envp){
    int res;

    res = mysystem("./test_normal");
    printf("return normally with %d\n", res);

    res = mysystem("./test_abnormal");
    printf("return abnormally with %d\n", res);

    return 0;

}