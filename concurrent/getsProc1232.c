#include "csapp.h"

// int select(int nfds, fd_set *readfds, fd_set *writefds,
//                   fd_set *exceptfds, struct timeval *timeout);

//        void FD_CLR(int fd, fd_set *set);
//        int  FD_ISSET(int fd, fd_set *set);
//        void FD_SET(int fd, fd_set *set);
//        void FD_ZERO(fd_set *set);

char *tfgets(char *s, int size, FILE *stream)
{
    struct timeval timeout;
    timeout.tv_sec=5;
    //timeout.tv_usec=0;
    fd_set set;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    select(1, &set, NULL, NULL, &timeout);;
    if (FD_ISSET(STDIN_FILENO, &set)) return fgets(s, size, stream);
    else return NULL;
}

int main(void)
{
	char buf[MAXLINE];
	
	if (tfgets(buf, MAXLINE, stdin) == NULL)
		printf("BOOM!\n");
	else
		printf("%s", buf);
	
	exit(0);
}