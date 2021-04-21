#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
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
    pthread_kill(*(pthread_t *)arg, 0);
}

int main()
{
    void *status;
    int err;
    pthread_t thread1, thread2, thread3;
    pthread_create(&thread1, NULL, func1, NULL);
    pthread_create(&thread2, NULL, func2, (void *)&thread1);
    sleep(1);
    printf("New\n");
    err = pthread_kill(thread3, SIGINT);
    printf("%s", strerror(err));
    pthread_create(&thread1, NULL, func1, NULL);
    err = pthread_kill(thread1, 5000);
    printf("%s", strerror(err));
    return 0;
}
