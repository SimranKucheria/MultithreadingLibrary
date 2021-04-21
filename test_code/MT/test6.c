
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../../thread.h"
static volatile int glob = 0;
static threadmutexlock mtx;
static void *
threadFunc(void *arg)
{
    int s;

    for (int j = 0; j < 10; j++) {
      
        s = thread_mutex_lock(&mtx);
            

        for (int k = 0; k < 10; k++)
            glob++;

       
        s = thread_mutex_unlock(&mtx);
    }

    return NULL;
}
static void *
threadSecFunc(void *arg)
{
    int s;

    for (int j = 0; j < 10; j++) {
      
        s = thread_mutex_lock(&mtx);
            

        for (int k = 0; k < 10; k++)
            glob--;

       
        s = thread_mutex_unlock(&mtx);
    }

    return NULL;
}
int
main(int argc, char *argv[])
{
    int opt, s;
    int numThreads;
    thread_t *thread;
    int verbose;
    alarm(120);        
    thread = calloc(2, sizeof(thread_t));
    if (thread == NULL)
        printf("calloc");
    initmutexlock(&mtx);
    thread_create(&thread[0], threadFunc, NULL);
    thread_create(&thread[1], threadSecFunc, NULL);
    thread_join(thread[0], NULL);
    thread_join(thread[1], NULL);
    
    printf("glob = %d\n", glob);
    
}
