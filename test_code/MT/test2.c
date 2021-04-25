#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "thread.h"
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
    thread_t thread[3];
    data *data1 = (data *)malloc(sizeof(data));
    if (!data1)
    {
        perror("Malloc error in test");
        return 0;
    }
    data1->message = "Received argument of thread ";
    data1->num = 66;
    thread_create(&thread[0], simpleFunc, (void *)data1);
    thread_create(&thread[1], simpleFunc1, (void *)data1);
    thread_create(&thread[2], simpleFunc2, (void *)data1);
    for (i = 0; i < 3; i++)
    {
        thread_join(thread[i], NULL);
    }
    return 0;
}