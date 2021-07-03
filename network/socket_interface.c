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
 * CLIENT: how to use the socket for connection?
 * connect(): needs fd, socket address (SERVER IP + SERVER PORT), address length (for parsing) = sizeof(sockaddr_in)
 * NOTICE: fd is associated with socket address. CLIENT IP = localhost; CLIENT is appointed by KERNEL (bind to 0 can work also)
 * addr contain IP & PORT of SERVER 
 * IP & PORT of CLIENT is in PH (internet packet header), sent to SERVER
 */
#include <sys/socket.h>
int connect(int clientfd, const struct sockaddr *addr, socklen_t addrlen);
//client establish connection with a server
//Returns: 0 if OK, −1 on error
//blocks until either the connection is successfully established or an error occurs
//if successful, ready for read & write for the connection (x:y,addr.sin_addr:addr.sin_port)

/*
 * CLIENT must know the IP & PORT to access a service.
 * So, SERVER must associate a "socket fd" to this well-known PORT number.
 * 
 * SERVER: bind(), needs info: the new created socket fd, SERVER socket address (IP+PORT), address length 
 * NOTICE: fd is associated with SERVER sockaddr. IP = SERVER host; PORT is given by programmer.
 */
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
//Returns: 0 if OK, −1 on error
//asks the kernel to associate the server’s socket address in addr with the socket descriptor sockfd
//server must have a call to bind(), since the client must know what are the preset ports
//example: 80 bind to web service

/*
 * socket(): create ACTIVE socket by default
 * So, SERVER must tell the kernel that the descriptor will be used by a server instead of a client
 * 
 * SERVER: listen(), needs info: the new created socket fd, max request number
 * NOTICE: fd has been binded with IP & PORT. After invoking listen(), this fd is PASSIVE, binded to SERVER IP & ginven PORT.
 * ACTIVE: send requests
 * PASSIVE: receive requests
 */
#include <sys/socket.h>
int listen(int sockfd, int backlog);
//Returns: 0 if OK, −1 on error
//converts sockfd from an active socket to a listening socket (accept connection from clients)
//backlog: hint about # of outstanding connection requests that the kernel should queue up before it starts to refuse requests

/*
 * "listenfd" is binded to the well-known PORT.
 * So, SERVER must have another fd for the following communication.
 * Otherwise, the "listenfd" will be occupied and leads to long wait time.
 * 
 * SERVER: accept(), needs info: the new created socket listenfd, a pointer to fill in CLIENT socket addr
 * Return connectedFD.
 * LISTEN: created once, exist for the lifetime of SERVER
 * CONNECTED: created each time SERVER accept requests (accept function return)
 */
#include <sys/socket.h>
int accept(int listenfd, struct sockaddr *addr, int *addrlen);
//Returns: nonnegative connected descriptor if OK, −1 on error
//Servers wait for connection requests from clients to arrive on listenfd

/* Given that there are many information necessary for the function call mentioned above,
 * it will be good if there is a stucture that can hold all these information.
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

/*
 * To get the addrinfo struct, we need a function
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
int getaddrinfo(const char *host,             /* hostname or address */
                const char *service,          /* port(80) or service name(http) */
                const struct addrinfo *hints, /* Input parameter */
                struct addrinfo **result);    /* output linked list */
//converts string representations of hostnames, host addresses, service names, and port numbers into socket address structures
//hostname + services in socket address
//Returns: 0 if OK, nonzero error code on error

void freeaddrinfo(struct addrinfo *result);
//frees the entire linked list
//Returns: nothing, avoid memory leak

const char *gai_strerror(int errcode);
//convert error code to message

/* package all these functions to provide convinent calling */
#include "csapp.h"
int open_clientfd(char *hostname, char *port);
//Returns: descriptor if OK, −1 on error
//establishes a connection with a server running on host hostname and listening for connection requests on port number port

#include "csapp.h"
int open_listenfd(char *port);
//Returns: descriptor if OK, −1 on error
//creates a listening descriptor that is ready to receive connection requests by calling the open_listenfd function
//ready to receive requests on port

/* implementation of these two helper function */

int open_clientfd(char *serverhost, char *serverport)
{
    int clientfd;
    struct addrinfo hints, *listp, *curp;

    /* get a list of potential server address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM; /* open a CONNECTION */
    hints.ai_flags = AI_NUMERICSERV; /* use numeric port argument */
    hints.ai_flags |= AI_ADDRCONFIG; /* recommended for connection */
    getaddrinfo(serverhost, serverport, &hints, &listp);

    /* walk the list for one that we can successfully connect to */
    for (curp = listp; curp; curp = curp->ai_next)
    {
        /* create socket fd */
        if ((clientfd = socket(curp->ai_family, curp->ai_socktype, curp->ai_protocol)) < 0)
        {
            continue;
        }

        /* connect to SERVER */
        if (connect(clientfd, curp->ai_addr, curp->ai_addrlen) != -1)
        {
            break;
        }
        close(clientfd);
    }

    /* clean up */
    freeaddrinfo(listp);
    if (!listp)
        return -1;
    else
        return clientfd;
}

int open_listenfd(char *serverport)
{
    struct addrinfo hints, *listp, *curp;
    int listenfd, optval = 1;

    /* get a list of potential SERVER address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;               /* Accept CONNECTION */
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;   /* on any IP addresses */
    hints.ai_flags |= AI_NUMERICSERV;              /* numeric port number */
    getaddrinfo(NULL, serverport, &hints, &listp); /* own server hostname as NULL */

    /* walk the list for one that we can bind to */
    for (curp = listp; curp; curp = curp->ai_next){
        /* create socket fd */
        if ((listenfd = socket(curp->ai_family, curp->ai_socktype, curp->ai_protocol)) < 0)
        {
            continue;
        }

        /* eliminate "Address already in use error" from bind */
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

        /* bind fd to address */
        if (connect(listenfd, curp->ai_addr, curp->ai_addrlen) != -1)
        {
            break;
        }
        close(listenfd);
    }

    /* clean up */
    freeaddrinfo(listp);
    if (!listp)
        return -1;
    /* n make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0){
        close(listenfd);
        return -1;
    }
    return listenfd;
}