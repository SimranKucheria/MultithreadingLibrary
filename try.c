#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <linux/sched.h>
#include <errno.h>

void *newfunc(){
    //sleep(10);
    printf("%d ",getpid());
    printf("%s\n\n\n\n", "hi3");
    // sleep(10);
    return (void *)10;
}

int main()
{
    int ret;
    thread_t thread1,thread2;
    thread_create(&thread1, func, NULL);
   // thread_create(&thread2, newfunc, NULL);
    //printf("")
    thread_join(thread1,(void *)&ret);
    printf("thread exited with '%d'\n",ret);
    
    printf("WHy");
}
