/*
Tests thread kill
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thread.h"
#include <signal.h>
#include <unistd.h>
void *func()
{
    printf("%s", "hi");
    int i = 1;
    while (i)
    {
        printf("%d", i);
        i++;
    }
}

int main()
{
    void *status;
    thread_t thread1, thread2;
    thread_create(&thread1, func, NULL);
    thread_create(&thread2, func, NULL);
    sleep(1);
    thread_kill(thread1, SIGINT);
    thread_kill(thread2, SIGSTOP);
    return 0;
}
