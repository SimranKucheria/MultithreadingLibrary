
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "thread.h"
static int glob = 0;
static threadmutexlock mtx;

static void * threadFunc(void *arg)
{
    for (int j = 0; j < 10; j++) {
      
       thread_mutex_lock(&mtx);
            

        for (int k = 0; k < 10; k++)
            glob++;

       
       thread_mutex_unlock(&mtx);
    }

    return NULL;
}

int
main(int argc, char *argv[])
{

    int numThreads;
    thread_t *thread;
    alarm(120);        
    numThreads = atoi(argv[optind]);
    thread = calloc(numThreads, sizeof(thread_t));
    if (thread == NULL)
        printf("calloc");
    initmutexlock(&mtx);
    for (int j = 0; j < numThreads; j++) {
        thread_create(&thread[j], threadFunc, NULL);
    }

    for (int j = 0; j < numThreads; j++) {
        thread_join(thread[j], NULL);
    }
    printf("glob = %d\n", glob);
    
}
