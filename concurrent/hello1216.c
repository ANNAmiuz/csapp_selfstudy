/* 12.16 */
#include <stdio.h>
#include "csapp.h"


void * thread(void *vargp);

int main(int argc, char ** argv){
    if (argc!=2) {
        fprintf(stderr, "usage: %s <n>\n", argv[0]); 
        exit(0);
    }
    
    int n=atoi(argv[1]);
    pthread_t tid[n];
    for (int i=0; i<n; ++i){
        pthread_create(&tid[n], NULL, thread, NULL);
    }
    pthread_exit(NULL);
}

void *thread(void *argp){
    printf("hello world.\n");
    return;
}
