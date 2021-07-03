#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>



void mmapcopy(int fd, int size){
    char * res_ptr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    write(STDOUT_FILENO, res_ptr, size);


}

int main(int argc, char ** argv, char ** envp){
    struct stat stat;
    if (argc != 2){
        printf("invalid argument  for %s. \n", argv[0]);
    }
    else{
        int fd = open(argv[1], O_RDONLY, 0);
        fstat(fd, &stat);
        mmapcopy(fd, stat.st_size);
    }
    exit(0);
}