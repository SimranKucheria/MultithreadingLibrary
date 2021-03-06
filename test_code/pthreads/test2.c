#include <stdio.h>                                                              
#include <unistd.h>
#include <errno.h>                                                              
#include <pthread.h>                                                                                                                        
#include <stdlib.h> 
#include <string.h>
typedef struct data{
    char *message;
    int num;
}data;

void *simpleFunc(void * args){
    struct data * threadargs=(struct data *)args;
    printf("Thread %d with message - %s \n",threadargs->num,threadargs->message);
}


int main(){
    pthread_t thread[10];
    for (int i=0;i<10;i++){
        struct data * data=(struct data *)malloc(sizeof(struct data));
        data->message="Received argument of thread ";
        data->num=i;
        pthread_create(&thread[i],NULL,simpleFunc,(void *)data);
        pthread_join(thread[i],NULL) ;
    }
    return 0;
}