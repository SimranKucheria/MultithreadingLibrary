#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

void *threadFunc(void *args)
{
    pthread_exit((void *)5);
}

int main()
{
    pthread_t t1;
    void *status;
    pthread_create(&t1, NULL, threadFunc, NULL);
    pthread_join(t1, &status);
    printf("%d", (int)status);
    return 0;
}