#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thread.h"

void *func()
{
    printf("Reached fn");
}

int main()
{
    void *status;
    thread_t thread1;
    thread_create(&thread1, func, NULL);
    thread_join(thread1, &status);
    int b;
    printf("Enter a number:\n");
    scanf("%d", &b);
    printf("Number entered is %d", b);
    return 0;
}
