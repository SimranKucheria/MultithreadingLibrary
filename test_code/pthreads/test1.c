#define _OPEN_THREADS                                                           
#include<stdlib.h>                                                                        
#include <errno.h>                                                              
#include <pthread.h>                                                            
#include <signal.h>                                                             
#include <stdio.h>                                                              
#include <unistd.h> 
void *simpleFunc(){
    printf("Thread Created Successfully\n");
}


int main(){
    pthread_t thread[10];
    for (int i=0;i<10;i++){
        pthread_create(&thread[i],NULL,simpleFunc,NULL);
    }
    for(int i=0;i<10;i++){
        pthread_join(thread[i],NULL);
        printf("Joining thread");
    }
    return 0;
}