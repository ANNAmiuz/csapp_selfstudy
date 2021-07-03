//#include "csapp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    struct stat stat1;
    char *type, *readok;
    int fd;

    if (argc <= 1){
        printf("less argument than wanted. \n");
    }
    else{
        fd = atoi(argv[1]);
    }

    fstat(fd, &stat1);
    if (S_ISREG(stat1.st_mode)) /* Determine file type */
        type = "regular";
    else if (S_ISDIR(stat1.st_mode))
        type = "directory";
    else
        type = "other";
    if ((stat1.st_mode & S_IRUSR)) /* Check read access */
        readok = "yes";
    else
        readok = "no";

    printf("type: %s, read: %s\n", type, readok);
    exit(0);
}