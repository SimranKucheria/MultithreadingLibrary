#include <stdio.h>                                                              
#include <unistd.h>
#include <errno.h>                                                              
#include "../../thread.h"                                                                                                                   
#include <stdlib.h> 
#include <string.h>
int r1=200;
void *threadFunc(void * args){
  
    int * num=args;
    int number=*num;
    thread_exit(&r1);
}

int main(){
    thread_t * thread;
    thread = calloc(2, sizeof(thread_t));
    if (thread == NULL){
        printf("calloc");
    }
    int * thread_stat[2];
    int  i=0;
    while(i<2){
        thread_create(&thread[i],threadFunc,(void *)&i);
       
        i++;
    }
    thread_join(thread[0],(void**)&thread_stat[0]);
    thread_join(thread[1],(void**)&thread_stat[1]);
    printf("First thread value %d",*thread_stat[0]);
    printf("Second thread value %d",*thread_stat[1]);
    return 0;
}