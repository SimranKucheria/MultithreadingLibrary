
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
int var = 0;
static pthread_mutex_t mtx;
void * threadFunc(void *arg)
{
    pthread_mutex_lock(&mtx);
    for (int k = 0; k < 10; k++)
        var++;
    pthread_mutex_unlock(&mtx);
    return NULL;
}
int main(int argc, char *argv[])
{
    int threads;
    pthread_t *thread;    
    thread = calloc(10, sizeof(pthread_t));
    if (thread == NULL)
        printf("calloc");
    pthread_mutex_init(&mtx,0);
    for (int i=0;i<10;i++){
        pthread_create(&thread[i],NULL, threadFunc, NULL);
        pthread_join(thread[i], NULL);
    }
    
    printf("var = %d\n", var);
    
}
