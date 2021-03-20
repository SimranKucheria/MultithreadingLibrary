#define _GNU_SOURCE
#include "thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#define STACKSIZE 128*1024
static pid_t id=0;
char* stack;
char * stackTop ;
static int  func(){
    stackTop[0]=5;
}
void * function(){
    printf("%s","hi everybody");
}
/* unique id for each thread */
void get_id(thread_t * thread){
    thread->t_id=id++;
}
/* something along these lines needs to be implemented
1. Allocate stack space for each thread 
2. Stack +stacksize is done because stack grows downwards
3. here the parent waits for the child
4. If you run code many times you'll see varying answers different types(between 1 and 5) because
both are modifying same address space
5. another thing observed is only global data gets shared so stack and stackTop had to be declared global
 */
int thread_create(thread_t *thread,void *(*start_routine) (void *), void *arg){
       
        thread->start_routine=start_routine;
        thread->arg=arg;
        stack =(char *)malloc(STACKSIZE);
        stackTop = stack + STACKSIZE;
        thread->t_id=clone(func,stackTop,CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SYSVSEM
                       | CLONE_SIGHAND | CLONE_THREAD
                       | CLONE_SETTLS | CLONE_PARENT_SETTID
                       | CLONE_CHILD_CLEARTID
                       | 0,NULL);
        if(thread->t_id==-1){
            free(stack);
            printf("%s","Failed");
        }
        stackTop[0]=1;
        waitpid(thread->t_id, NULL, 0);
        printf("%d",stackTop[0]);
}
int main(){
    thread_t * thread;
    thread=(thread_t *)calloc(1,sizeof(thread_t));
    thread_create(thread,function,NULL);
}