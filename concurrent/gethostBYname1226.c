// Use the lock-and-copy technique to implement a thread-safe non-reentrant version of gethostbyname called gethostbyname_ts. A correct solution will use a
// deep copy of the hostent structure protected by a mutex

#include <sys/socket.h>
#include "csapp.h"

//struct hostent *gethostbyname(const char *name);

// struct hostent
// {
//     char *h_name;       /* official name of host */
//     char **h_aliases;   /* alias list */
//     int h_addrtype;     /* host address type */
//     int h_length;       /* length of address */
//     char **h_addr_list; /* list of addresses */
// }

//    h_name The official name of the host.

//    h_aliases
//           An array of alternative names for the host, terminated by a null pointer.

//    h_addrtype
//           The type of address; always AF_INET or AF_INET6 at present.

//    h_length
//           The length of the address in bytes.

//    h_addr_list
//           An array of pointers to network addresses for the host (in network  byte  order),  termi‐
//           nated by a null pointer.

//    h_addr The first address in h_addr_list for backward compatibility.

static sem_t mutex;

struct hostent *gethostbyname_ts(const char *name, struct hostent *targetbuf)
{
    struct hostent *tmpptr;
    sem_wait(&mutex);
    tmpptr = gethostbyname(name);
    /* begin deep copying */
    //copy the char[]/char*
    targetbuf->h_name = (char *)malloc(strlen(tmpptr->h_name) + 1);
    strcpy(targetbuf, tmpptr->h_name);

    targetbuf->h_addrtype = tmpptr->h_addrtype;
    targetbuf->h_length = tmpptr->h_length;

    //copy the char ** (null-terminated)
    int idx = 0;
    char *cur;
    while ((cur = tmpptr->h_aliases[idx]) != NULL)
    {
        ++idx;
    }
    targetbuf->h_aliases = (char **)Malloc((idx + 1) * sizeof(char *));
    int i;
    for (i = 0; i < idx; ++i)
    {
        targetbuf->h_aliases[i] = (char *)Malloc(strlen(tmpptr->h_aliases[i]) + 1);
        strcpy(targetbuf->h_aliases[i], tmpptr->h_aliases[i]);
    }
    targetbuf->h_aliases[i] = NULL;

    idx = 0;
    while ((cur = tmpptr->h_addr_list[idx]) != NULL)
    {
        ++idx;
    }
    targetbuf->h_addr_list = (char **)Malloc((idx + 1) * sizeof(char *));
    for (i = 0; i < idx; ++i)
    {
        targetbuf->h_addr_list[i] = (char *)Malloc(strlen(tmpptr->h_addr_list[i]) + 1);
        strcpy(targetbuf->h_addr_list[i], tmpptr->h_addr_list[i]);
    }
    targetbuf->h_addr_list[i] = NULL;
    sem_post(&mutex);
    return targetbuf;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <hostname>\n", argv[0]);
        exit(0);
    }

    Sem_init(&mutex, 0, 1); //for exclusive use of the return pointer to static value

    struct hostent *host;
    host = gethostbyname_ts(argv[1], host);
    printf("%s\n", host->h_name);
    printf("%d\n", host->h_addrtype);
    printf("%d\n", host->h_length);

    return 0;
}