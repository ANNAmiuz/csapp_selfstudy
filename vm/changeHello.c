#include <stdio.h>
#include <assert.h>
#include "csapp.h"

int main(int argc, char ** argv){
    struct stat stat;
    int fd = open("hello.txt", O_RDWR, 0);
    fstat(fd, &stat);
    char * p1 = mmap(NULL, stat.st_size, PROT_WRITE, MAP_SHARED, fd, 0);
    p1[0] = 'J';
    munmap(p1, stat.st_size);
    close(fd);
}