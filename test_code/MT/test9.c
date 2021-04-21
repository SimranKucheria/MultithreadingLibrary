#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thread.h"

void *func()
{
    int b;
    printf("Enter a number:");
    scanf("%d", &b);
    printf("Number entered is %d", b);
}

int main()
{
    void *status;
    thread_t thread1;
    thread_create(&thread1, func, NULL);
    thread_join(thread1, &status);
    return 0;
}
