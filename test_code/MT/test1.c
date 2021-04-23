#include <errno.h>
#include "../../thread.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
void *simpleFunc()
{
    printf("Thread Created Successfully\n");
    return NULL;
}

int main()
{
    thread_t thread[10];
    for (int i = 0; i < 10; i++)
    {
        printf("%d", i);
        thread_create(&thread[i], simpleFunc, NULL);
        printf("%d", i);
    }
    for (int j = 0; j < 10; j++)
    {
        thread_join(thread[j], NULL);
        printf("Joining thread\n");
    }
    return 0;
}