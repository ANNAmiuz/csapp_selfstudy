#include "csapp.h"

#define NTHREADS 4
#define SBUFSIZE 16
#define INITTHREAD 1
#define MAXTHREAD 1024

typedef struct
{
  int *buf;    /* Buffer array */
  int n;       /* Maximum number of slots */
  int front;   /* buf[(front+1)%n] is first item */
  int rear;    /* buf[rear%n] is last item */
  sem_t mutex; /* Protects accesses to buf */
  sem_t slots; /* Counts available slots */
  sem_t items; /* Counts available items */
} sbuf_t;

typedef struct
{
  pthread_t tid;
  sem_t mutex;
} ithread;

void sbuf_init(sbuf_t *sp, int n);
void sbuf_deinit(sbuf_t *sp);
void sbuf_insert(sbuf_t *sp, int item);
int sbuf_remove(sbuf_t *sp);
void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg);
void *server_thread(void *vargp);
void *change_thread(void *vargp); //change size of all the threads
void init();
void create_threads(int start, int size);
int isFULL(sbuf_t * sbuf);
int isEMPTY(sbuf_t *sbuf);


sbuf_t sbuf;                //all the connfds
int nthreads;               //current number of threads
ithread threads[MAXTHREAD]; //all the threads(tid+mutex)

int main(int argc, char **argv)
{
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  pthread_t tid;

  /* Check command line args */
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  init();
  listenfd = Open_listenfd(argv[1]);
  pthread_create(&tid, NULL, change_thread, NULL);
  while (1)
  {
    clientlen = sizeof(struct sockaddr_storage);
    connfd = accept(listenfd, (SA *)&clientaddr, &clientlen);
    sbuf_insert(&sbuf, connfd);
  }
}

void init()
{
  nthreads = INITTHREAD;
  sbuf_init(&sbuf, SBUFSIZE);
  create_threads(0, nthreads);
}

void create_threads(int start, int size)
{
  int i;
  for (i=start; i<start+size; ++i){
    //for the ith thread, create a server thread
    sem_init(&threads[i].mutex, 0, 1);
    int * arg = malloc(sizeof(int));
    *arg=i;
    pthread_create(&(threads[i].tid), NULL, server_thread, (void*)arg);
  }
}

void *change_thread(void *vargp){
  while(1){
    if (isFULL(&sbuf)){
      if (nthreads==MAXTHREAD) continue;
      create_threads(nthreads, nthreads);
      nthreads*=2;
      continue;
    }
    if (isEMPTY(&sbuf)){
      if (nthreads==1) continue;
      for (int i=nthreads/2; i<nthreads; ++i){
        P(&(threads[i].mutex));
        pthread_cancel(threads[i].tid);
        V(&(threads[i].mutex));
      }
      nthreads/=2;
      continue;
    }

  }
}

void *server_thread(void*vargp){
  int i=*(int *)vargp;
  free(vargp);
  while (1){
    //ensure the thread would not be affected
    P(&(threads[i].mutex));
    int connfd = sbuf_remove(&sbuf);
    doit(connfd);
    close(connfd);
    V(&(threads[i].mutex));
  }

}

int isEMPTY(sbuf_t *sbuf){
  int res;
  P(&sbuf->mutex);                          /* Lock the buffer */
  res = sbuf->front == sbuf->rear;
  V(&sbuf->mutex);                          /* Lock the buffer */
  return res;
}

int isFULL(sbuf_t *sbuf){
  int res;
  P(&sbuf->mutex);                          /* Lock the buffer */
  res = (sbuf->rear - sbuf->front) == sbuf->n;
  V(&sbuf->mutex);                          /* Lock the buffer */
  return res;
}

/* Create an empty, bounded, shared FIFO buffer with n slots */
void sbuf_init(sbuf_t *sp, int n)
{
  sp->buf = Calloc(n, sizeof(int));
  sp->n = n;                  /* Buffer holds max of n items */
  sp->front = sp->rear = 0;   /* Empty buffer iff front == rear */
  Sem_init(&sp->mutex, 0, 1); /* Binary semaphore for locking */
  Sem_init(&sp->slots, 0, n); /* Initially, buf has n empty slots */
  Sem_init(&sp->items, 0, 0); /* Initially, buf has zero data items */
}

/* Clean up buffer sp */
void sbuf_deinit(sbuf_t *sp)
{
  Free(sp->buf);
}

/* Insert item onto the rear of shared buffer sp */
void sbuf_insert(sbuf_t *sp, int item)
{
  P(&sp->slots);                          /* Wait for available slot */
  P(&sp->mutex);                          /* Lock the buffer */
  sp->buf[(++sp->rear) % (sp->n)] = item; /* Insert the item */
  V(&sp->mutex);                          /* Unlock the buffer */
  V(&sp->items);                          /* Announce available item */
}

/* Remove and return the first item from buffer sp */
int sbuf_remove(sbuf_t *sp)
{
  int item;
  P(&sp->items);                           /* Wait for available item */
  P(&sp->mutex);                           /* Lock the buffer */
  item = sp->buf[(++sp->front) % (sp->n)]; /* Remove the item */
  V(&sp->mutex);                           /* Unlock the buffer */
  V(&sp->slots);                           /* Announce available slot */
  return item;
}

void doit(int fd)
{
  int is_static;
  struct stat sbuf;
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char filename[MAXLINE], cgiargs[MAXLINE];
  rio_t rio;

  /* Read request line and headers */
  Rio_readinitb(&rio, fd);
  if (!Rio_readlineb(&rio, buf, MAXLINE)) //line:netp:doit:readrequest
    return;
  printf("%s", buf);
  sscanf(buf, "%s %s %s", method, uri, version); //line:netp:doit:parserequest
  if (strcasecmp(method, "GET"))
  { //line:netp:doit:beginrequesterr
    clienterror(fd, method, "501", "Not Implemented",
                "Tiny does not implement this method");
    return;
  }                       //line:netp:doit:endrequesterr
  read_requesthdrs(&rio); //line:netp:doit:readrequesthdrs

  /* Parse URI from GET request */
  is_static = parse_uri(uri, filename, cgiargs); //line:netp:doit:staticcheck
  if (stat(filename, &sbuf) < 0)
  { //line:netp:doit:beginnotfound
    clienterror(fd, filename, "404", "Not found",
                "Tiny couldn't find this file");
    return;
  } //line:netp:doit:endnotfound

  if (is_static)
  { /* Serve static content */
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))
    { //line:netp:doit:readable
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't read the file");
      return;
    }
    serve_static(fd, filename, sbuf.st_size); //line:netp:doit:servestatic
  }
  else
  { /* Serve dynamic content */
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode))
    { //line:netp:doit:executable
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't run the CGI program");
      return;
    }
    serve_dynamic(fd, filename, cgiargs); //line:netp:doit:servedynamic
  }
}

void read_requesthdrs(rio_t *rp)
{
  char buf[MAXLINE];

  Rio_readlineb(rp, buf, MAXLINE);
  printf("%s", buf);
  while (strcmp(buf, "\r\n"))
  { //line:netp:readhdrs:checkterm
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
  }
  return;
}

int parse_uri(char *uri, char *filename, char *cgiargs)
{
  char *ptr;

  if (!strstr(uri, "cgi-bin"))
  { /* Static content */             //line:netp:parseuri:isstatic
    strcpy(cgiargs, "");             //line:netp:parseuri:clearcgi
    strcpy(filename, ".");           //line:netp:parseuri:beginconvert1
    strcat(filename, uri);           //line:netp:parseuri:endconvert1
    if (uri[strlen(uri) - 1] == '/') //line:netp:parseuri:slashcheck
      strcat(filename, "home.html"); //line:netp:parseuri:appenddefault
    return 1;
  }
  else
  { /* Dynamic content */  //line:netp:parseuri:isdynamic
    ptr = index(uri, '?'); //line:netp:parseuri:beginextract
    if (ptr)
    {
      strcpy(cgiargs, ptr + 1);
      *ptr = '\0';
    }
    else
      strcpy(cgiargs, ""); //line:netp:parseuri:endextract
    strcpy(filename, "."); //line:netp:parseuri:beginconvert2
    strcat(filename, uri); //line:netp:parseuri:endconvert2
    return 0;
  }
}

void serve_static(int fd, char *filename, int filesize)
{
  int srcfd;
  char *srcp, filetype[MAXLINE], buf[MAXBUF];

  /* Send response headers to client */
  get_filetype(filename, filetype);    //line:netp:servestatic:getfiletype
  sprintf(buf, "HTTP/1.0 200 OK\r\n"); //line:netp:servestatic:beginserve
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n", filesize);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: %s\r\n\r\n", filetype);
  Rio_writen(fd, buf, strlen(buf)); //line:netp:servestatic:endserve

  /* Send response body to client */
  srcfd = Open(filename, O_RDONLY, 0);                        //line:netp:servestatic:open
  srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); //line:netp:servestatic:mmap
  Close(srcfd);                                               //line:netp:servestatic:close
  Rio_writen(fd, srcp, filesize);                             //line:netp:servestatic:write
  Munmap(srcp, filesize);                                     //line:netp:servestatic:munmap
}

void get_filetype(char *filename, char *filetype)
{
  if (strstr(filename, ".html"))
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".gif"))
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".png"))
    strcpy(filetype, "image/png");
  else if (strstr(filename, ".jpg"))
    strcpy(filetype, "image/jpeg");
  else
    strcpy(filetype, "text/plain");
}

void serve_dynamic(int fd, char *filename, char *cgiargs)
{
  char buf[MAXLINE], *emptylist[] = {NULL};

  /* Return first part of HTTP response */
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));

  if (Fork() == 0)
  { /* Child */ //line:netp:servedynamic:fork
    /* Real server would set all CGI vars here */
    setenv("QUERY_STRING", cgiargs, 1);                         //real servers will set the other env var as well
    Dup2(fd, STDOUT_FILENO); /* Redirect stdout to client */    //line:netp:servedynamic:dup2
    Execve(filename, emptylist, environ); /* Run CGI program */ //line:netp:servedynamic:execve
  }
  Wait(NULL); /* Parent waits for and reaps child */ //line:netp:servedynamic:wait
}

void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg)
{
  char buf[MAXLINE];

  /* Print the HTTP response headers */
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n\r\n");
  Rio_writen(fd, buf, strlen(buf));

  /* Print the HTTP response body */
  sprintf(buf, "<html><title>Tiny Error</title>");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "<body bgcolor="
               "ffffff"
               ">\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "<hr><em>The Tiny Web server</em>\r\n");
  Rio_writen(fd, buf, strlen(buf));
}