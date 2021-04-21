#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thread.h"
#include <signal.h>
#include <unistd.h>
void *func1()
{
    printf("%s", "In fn 1");
    int i = 1;
    while (i < 100000)
    {
        i++;
    }
    printf("%s", "Fn 1 ended");
}
void *func2(void *arg)
{
    printf("%s", "In fn 2");
    thread_kill(*(thread_t *)arg, 0);
}

int main()
{
    void *status;
    int err;
    thread_t thread1, thread2, thread3;
    thread_create(&thread1, func1, NULL);
    thread_create(&thread2, func2, (void *)&thread1);
    sleep(1);
    printf("New\n");
    err = thread_kill(thread3, SIGINT);
    printf("%s", strerror(err));
    thread_create(&thread1, func1, NULL);
    err = thread_kill(thread1, 5000);
    printf("%s", strerror(err));
    return 0;
}
