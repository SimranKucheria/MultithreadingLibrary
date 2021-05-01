/*
Tests thread exit 
*/
#include <stdio.h>                                                              
#include <unistd.h>
#include <errno.h>                                                              
#include "thread.h"                                                                                                                   
#include <stdlib.h> 
#include <string.h>

void *threadFunc(void * args){
    int *ret;

    if ((ret = (int*) malloc(20)) == NULL) {
        perror("malloc() error");
        exit(2);
    }
    // sleep(2);
    *ret=200;
    thread_exit(ret);
}

int main(){
    thread_t * thread;
    thread = calloc(2, sizeof(thread_t));
    if (thread == NULL){
        printf("calloc");
    }
    void *status=(void *)malloc(sizeof(void));
    void * status1=(void *)malloc(sizeof(void));
    int  i=0;

    thread_create(&thread[0],threadFunc,NULL);
    thread_create(&thread[1],threadFunc,NULL);  
    thread_join(thread[0],&status);
    thread_join(thread[1],&status1);
    printf("First thread value %d\n",*(int *)status);
    printf("Second thread value %d\n",*(int*)status1);
    return 0;
}