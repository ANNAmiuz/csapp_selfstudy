#include "csapp.h"

//16-bit network byte order --  16-bit hex
int main(int argc, char ** argv){
    uint16_t addr;

    if (argc != 2){
        fprintf(stderr, "usage: %d <network byte order address>", argv[0]);
        exit(0);
    }
    sscanf(argv[1], "%d", &addr);
    uint16_t res = ntohs(addr);
    printf("0x%x\n", addr);
    exit(0);
}