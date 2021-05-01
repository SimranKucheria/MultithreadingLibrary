/*
Tests Mutex Lock
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "thread.h"
int var = 0;
static threadmutexlock mtx;
void * threadFunc(void *arg)
{
    thread_mutex_lock(&mtx);
    for (int k = 0; k < 10; k++)
        var++;
    thread_mutex_unlock(&mtx);
    return NULL;
}
int main(int argc, char *argv[])
{
    int threads;
    thread_t *thread;  
    thread = calloc(10, sizeof(thread_t));
    if (thread == NULL)
        printf("calloc");
    initmutexlock(&mtx);
    for (int i=0;i<10;i++){
        thread_create(&thread[i], threadFunc, NULL);
        thread_join(thread[i], NULL);
    }
    
    printf("var = %d\n", var);
    
}
