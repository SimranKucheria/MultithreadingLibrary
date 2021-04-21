
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

int
main(int argc, char *argv[])
{
    int opt, s;
    int numThreads;
    thread_t *thread;
    int verbose;
    alarm(120);        
    numThreads = atoi(argv[optind]);
    thread = calloc(numThreads, sizeof(thread_t));
    if (thread == NULL)
        printf("calloc");
    s = initmutexlock(&mtx);
    for (int j = 0; j < numThreads; j++) {
        s = thread_create(&thread[j], threadFunc, NULL);
    }

    for (int j = 0; j < numThreads; j++) {
        s = thread_join(thread[j], NULL);
    }
    printf("glob = %d\n", glob);
    
}
