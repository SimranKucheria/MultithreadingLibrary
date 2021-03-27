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
    // int ret;
    pthread_t thread1,thread2;
    void *status;
    // thread_create(&thread1, NULL,func, NULL);
    pthread_create(&thread2, NULL, newfunc, NULL);
    pthread_join(thread2, &status);
    //printf("")
    //thread_join(thread1,(void *)&ret);
    printf("%d\n",(int)status); 
    
    printf("WHy");
}
