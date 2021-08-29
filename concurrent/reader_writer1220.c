/* Global variables */

#include "csapp.h"

#define total 10
sem_t readcnt; /* Initially = 0 */
sem_t mutex, w; /* Both initially = 1 */
void reader(void)
{
    while (1) {
        P(&readcnt);
        P(&mutex);
        /* Critical section */
        /* Reading happens */
        V(&mutex);
        V(&readcnt);
    }
}
void writer(void)
{
    while (1) {
        P(&mutex);
        /* Critical section */
        /* Writing happens */
        V(&mutex);
    }
}

void init(int n){
    Sem_init(&mutex, 0, 1);
    Sem_init(&readcnt, 0, n);
}

int main(int argc, char ** argv){
    if (argc!=2) exit(0);
    int n=atoi(argv[1]);
    pthread_t tid;
    init(n);
    for (int i=0; i<total; ++i){
        if (i&1==0) pthread_create(&tid, NULL, reader, NULL);
        else Pthread_create(&tid, NULL, writer, NULL);
    }
    pthread_exit(NULL);
    exit(0);
}