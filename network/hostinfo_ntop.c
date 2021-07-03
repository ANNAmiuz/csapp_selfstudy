#include "csapp.h"

int main(int argc, char ** argv){
    if (argc!=2){
        fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
        exit(0);
    }
    struct addrinfo hints, *listp;
    struct sockaddr_in *sockp;
    char * res;
    int rc;
    char dst[MAXLINE];
    
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0){
        fprintf(stderr, "getaddrinfo failure information: %s\n", gai_strerror(rc));
        exit(1);
    }
    
    for (struct addrinfo * cur = listp; cur; cur = cur->ai_next){
        sockp = (struct sockaddr_in *)(cur->ai_addr);
        res = inet_ntop(AF_INET, &((sockp->sin_addr).s_addr), dst, MAXLINE);
        if (res == NULL){
            fprintf(stderr, "fail in inet_ntop. \n");
            Freeaddrinfo(listp);
            exit(2);    
        }
        printf("%s\n", dst);
    }
    Freeaddrinfo(listp);
    exit(0);


}