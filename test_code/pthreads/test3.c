#include <stdio.h>                                                              
#include <unistd.h>
#include <errno.h>                                                              
#include<pthread.h>                                                                                                                      
#include <stdlib.h> 
#include <string.h>
int r1=200;
void *threadFunc(void * args){
  
    int * num=args;
    int number=*num;
    printf("%d",*(int *)args);
    pthread_exit(&r1);
}

int main(){
    pthread_t thread[2];
    int * thread_stat[2];
    int  i=0;
    while(i<2){
        pthread_create(&thread[i],NULL,threadFunc,(void *)&i);
        // pthread_join(thread[i],&thread_stat[i]);
        i++;
    }
    pthread_join(thread[0],(void**)&thread_stat[0]);
    pthread_join(thread[1],(void**)&thread_stat[1]);
    printf("First thread value %d",*thread_stat[0]);
    printf("Second thread value %d",*thread_stat[1]);
    return 0;
}