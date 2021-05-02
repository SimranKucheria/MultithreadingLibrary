/*
Tests Create & Join with arguments
*/
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
typedef struct data
{
    char *message;
    int num;
} data;

void *simpleFunc(void *args)
{
    printf("Thread %d with message - %s \n", ((data *)args)->num, ((data *)args)->message);
    return NULL;
}

void *simpleFunc1(void *args)
{
    printf("Thread %d with message - %s \n", ((data *)args)->num, ((data *)args)->message);
    return NULL;
}

void *simpleFunc2(void *args)
{
    printf("Thread %d with message - %s \n", ((data *)args)->num, ((data *)args)->message);
    return NULL;
}

int main()
{
    int i;
    pthread_t thread[3];
    data *data1 = (data *)malloc(sizeof(data));
    if (!data1)
    {
        // perror("Malloc error in test");
        return 0;
    }
    data1->message = "Received argument of thread ";
    data1->num = 66;
    pthread_create(&thread[0],NULL, simpleFunc, (void *)data1);
    thread_join(thread[0], NULL);
    pthread_create(&thread[1],NULL, simpleFunc1, (void *)data1);
    thread_join(thread[1], NULL);
    pthread_create(&thread[2],NULL, simpleFunc2, (void *)data1);
    thread_join(thread[2], NULL);
    return 0;
}