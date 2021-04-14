#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include "thread.h"
void *func(){
    printf("%s","hi");
   // char *ret;
    //if ((ret = (char*) malloc(20)) == NULL) {
     //   perror("malloc() error");
    //    exit(2);
    //}
    //strcpy(ret, "This is a test");
    //printf("%s",ret);
    //thread_exit(ret);
}


void *newfunc(){
    // sleep(10);
    printf("%d ",getpid());
    printf("%s\n\n\n\n", "hi3");
    
    // sleep(10);
    return (void *)50;
}
void *thread(void *arg) {
  char *ret;

  if ((ret = (char*) malloc(20)) == NULL) {
    perror("malloc() error");
    exit(2);
  }
  strcpy(ret, "This is a test");
  thread_exit(ret);
}

int main()
{
    void *status;
    thread_t thread1;
    thread_create(&thread1,newfunc, NULL);
    thread_join(thread1,&status);   
    printf("%d",(int)status);
    // thread_kill(thread1,SIGINT);
    return 0;
}
