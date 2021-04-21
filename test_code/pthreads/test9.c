#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void *func()
{
    int b;
    printf("%s", "Enter a number:");
    scanf("%d", &b);
    printf("Number entered is %d", b);
}

int main()
{
    void *status;
    pthread_t thread1;
    pthread_create(&thread1, NULL, func, NULL);
    pthread_join(thread1, &status);
    return 0;
}
