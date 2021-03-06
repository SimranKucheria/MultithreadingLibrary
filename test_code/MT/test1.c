/* 
Tests Join & Create
*/
#include <errno.h>
#include "thread.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void *simpleFunc()
{
    printf("In fn\n");
    return NULL;
}
void *simpleFunc1()
{
    printf("In fn\n");
    return NULL;
}
void *simpleFunc2()
{
    printf("In fn\n");
    return NULL;
}
void *simpleFunc3()
{
    printf("In fn\n");
    return NULL;
}
void *simpleFunc4()
{
    printf("In fn\n");
    return NULL;
}
void *simpleFunc5()
{
    printf("In fn\n");
    return NULL;
}
int main()
{
    thread_t thread[6];

    thread_create(&thread[0], simpleFunc, NULL);
    printf("Thread Created Successfully\n");
    thread_create(&thread[1], simpleFunc1, NULL);
    printf("Thread Created Successfully\n");
    thread_create(&thread[2], simpleFunc2, NULL);
    printf("Thread Created Successfully\n");
    thread_create(&thread[3], simpleFunc3, NULL);
    printf("Thread Created Successfully\n");
    thread_create(&thread[4], simpleFunc4, NULL);
    printf("Thread Created Successfully\n");
    thread_create(&thread[5], simpleFunc5, NULL);
    printf("Thread Created Successfully\n");

    for (int j = 0; j < 6; j++)
    {
        thread_join(thread[j], NULL);
        printf("Joining thread\n");
    }
    return 0;
}
