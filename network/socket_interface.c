#include "csapp.h"
/* What is socket? */

/*
 * Two processes on computers: client & server
 * The locating computer, is the HOST, with a unique IP address.
 * There are many processes on ONE machine, so it needs a PORT number to specify the processes.
 * Server: needs to bind the PORT number on one of its services.
 * Client: needs to specify the PORT number to get one of the server's services.
 * Then they can build a connection, and perform write / read.
 * 
 * Socket is: (插口？)
 * a NOTION representating the SYSTEM RESOURCES that can support and provice the communication mentioned above.
 */

/* socket address */
/*
 * For KERNEL: socket is the endpoint for communication
 * For PROGRAM: socket is an open file
 * Connection is specfied by "socket pair" [cliaddr:cliport, servaddr:servport]
 *
 * The socket address needs:
 * Protocal, Port number, IP address, padding
 */
/* IP socket address structure */
struct sockaddr_in
{
    uint16_t sin_family;       /* Protocol family (always AF_INET for internet application) */
    uint16_t sin_port;         /* Port number in network byte order */
    struct in_addr sin_addr;   /* IP address in network byte order */
    unsigned char sin_zero[8]; /* Pad to sizeof(struct sockaddr) */
};
/* Generic socket address structure (for connect, bind, and accept) */
struct sockaddr
{
    uint16_t sa_family; /* Protocol family */
    char sa_data[14];   /* Address data */
};
typedef struct sockaddr SA;

/*
 * How to new a socket? Necessary info: protocol family, socket type, (protocol)
 * NOTICE: socket is not related to the 32-bit IP and 16-bit PORT
 */
#include <sys/types.h>
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
//clients and servers create a socket descriptor
//Returns: nonnegative descriptor if OK, −1 on error

/*
 * Client: how to use the socket for connection?
 * connect(): needs fd, socket address (SERVER IP + SERVER PORT), address length (for parsing) = sizeof(sockaddr_in)
 * NOTICE: CLIENT IP is the localhost name; CLIENT is appointed by KERNEL (bind to 0 can work also)
 * The IP & PORT of client is in PH (internet packet header), sent to SERVER
 */
#include <sys/socket.h>
int connect(int clientfd, const struct sockaddr *addr, socklen_t addrlen);
//client establish connection with a server
//Returns: 0 if OK, −1 on error
//blocks until either the connection is successfully established or an error occurs
//if successful, ready for read & write for the connection (x:y,addr.sin_addr:addr.sin_port)

/*
 * 
 *
 */

/*
 * addrinfo STRUCT
 * 
 * Many of this structures form a linked list, one --> a node
 * Contains info for create socket, connect, bind, listen and accept
 */
struct addrinfo
{
    int ai_flags;             /* Hints argument flags, can be set in hints */
    int ai_family;            /* First arg to socket function, can be set in hints */
    int ai_socktype;          /* Second arg to socket function, can be set in hints */
    int ai_protocol;          /* Third arg to socket function, can be set in hints */
    char *ai_canonname;       /* Canonical hostname */
    size_t ai_addrlen;        /* Size of ai_addr struct */
    struct sockaddr *ai_addr; /* Ptr to socket address structure */
    struct addrinfo *ai_next; /* Ptr to next item in linked list */
};
//can be passed directly to socket(), connect(), bind()