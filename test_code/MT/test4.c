/*
Tests exit
*/
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "thread.h"
#include <stdlib.h>
#include <string.h>

void *threadFunc(void *args)
{
    thread_exit((void *)5);
}

int main()
{
    thread_t t1;
    void *status;
    thread_create(&t1, threadFunc, NULL);
    thread_join(t1, &status);
    printf("%d", (int)status);
    return 0;
}