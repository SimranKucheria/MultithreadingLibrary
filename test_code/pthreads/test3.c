#include <stdio.h>                                                              
#include <unistd.h>
#include <errno.h>                                                              
#include "pthread.h"                                                                                                                   
#include <stdlib.h> 
#include <string.h>

void *threadFunc(void * args){
    int *ret;
    if ((ret = (int*) malloc(20)) == NULL) {
        perror("malloc() error");
        exit(2);
    }
    *ret=200;
    pthread_exit(ret);
}

int main(){
    pthread_t * thread;
    thread = calloc(2, sizeof(pthread_t));
    if (thread == NULL){
        printf("calloc");
    }
    void *status;
    void * status1;
    int  i=0;

    pthread_create(&thread[0],NULL,threadFunc,NULL);
    pthread_create(&thread[1],NULL,threadFunc,NULL);  
    pthread_join(thread[0],&status);
    pthread_join(thread[1],&status1);
    printf("First thread value %d\n",*(int *)status);
    printf("Second thread value %d\n",*(int*)status1);
    return 0;
}