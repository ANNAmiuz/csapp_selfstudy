#include "csapp.h"

//convert 16-bit hex argument to 16-bit network byte-order, prints the result

int main(int argc, char **argv)
{
    uint16_t addr;

    struct in_addr inaddr;

    if (argc != 2) {
        unix_error("please input a argument");
        exit(-1);
    }

    sscanf(argv[1], "%x", &addr);
    inaddr.s_addr = htons(addr);
    printf("%u\n",(inaddr));

    exit(0);
}