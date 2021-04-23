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
    struct data *threadargs = (struct data *)args;
    printf("Thread %d with message - %s \n", threadargs->num, threadargs->message);
    return NULL;
}

int main()
{
    thread_t thread[10];
    for (int i = 0; i < 10; i++)
    {
        struct data *data = (struct data *)malloc(sizeof(struct data));
        if (!data)
        {
            perror("Malloc error in test");
            return 0;
        }
        data->message = "Received argument of thread ";
        data->num = i;
        thread_create(&thread[i], simpleFunc, (void *)data);
        thread_join(thread[i], NULL);
    }
    return 0;
}