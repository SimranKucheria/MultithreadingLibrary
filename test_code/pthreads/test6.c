
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<pthread.h>
int var = 0;
static pthread_mutex_t mtx;
void * threadFunc(void *arg)
{
    int s;
    pthread_mutex_lock(&mtx);
    for (int k = 0; k < 10; k++)
        var++;
    pthread_mutex_unlock(&mtx);
    return NULL;
}
void *threadSecFunc(void *arg)
{
    int s;
    pthread_mutex_lock(&mtx);
    for (int k = 0; k < 10; k++)
        var--;
    pthread_mutex_unlock(&mtx);
    return NULL;
}
int main(int argc, char *argv[])
{
    int opt, s;
    int threads;
    pthread_t *thread;
    int verbose;
    alarm(120);        
    thread = calloc(2, sizeof(pthread_t));
    if (thread == NULL)
        printf("calloc");
    pthread_mutex_init(&mtx,0);
    pthread_create(&thread[0],NULL, threadFunc, NULL);
    pthread_create(&thread[1],NULL, threadSecFunc, NULL);
    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);
    
    printf("var = %d\n", var);
    
}
