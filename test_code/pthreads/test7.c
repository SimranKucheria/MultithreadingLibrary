#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
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
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, func, NULL);
    pthread_create(&thread2, NULL, func, NULL);
    sleep(1);
    pthread_kill(thread1, SIGINT);
    pthread_kill(thread2, SIGSTOP);
    return 0;
}
