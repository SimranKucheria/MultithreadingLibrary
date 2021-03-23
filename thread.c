#define _GNU_SOURCE
#include <sched.h>
#ifndef THREAD_H
#define THREAD_H
#include "queue.c"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#define STACKSIZE 128*1024
char * stackTop ;
queue * q;
static int flag=0;
/* something along these lines needs to be implemented
1. Allocate stack space for each thread 
2. Stack +stacksize is done because s
tack grows downwards
 */
int thread_create(thread_t *thread,void *(*start_routine) (void *), void *arg){
       
        thread->stack =(char *)malloc(STACKSIZE);
        if(thread->stack==NULL){
            printf("%s","NO");
        }
        stackTop = thread->stack + STACKSIZE;
        thread->start_routine=start_routine;
        thread->arg=arg;
        thread->state=RUNNING;
        thread->t_id=clone((int(*)(void *))start_routine,stackTop,CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SYSVSEM
                       | CLONE_SIGHAND | CLONE_THREAD
                       | CLONE_SETTLS | CLONE_PARENT_SETTID
                       | CLONE_CHILD_CLEARTID
                       | 0,thread->arg);
        printf("%d",thread->t_id);
        if(thread->t_id==-1){
            free(thread->stack);
            return errno;
        }
        // if(flag==0){
        //     flag=1;
        //     initq(q);
        // }
        wait(NULL);
        //printf("%d",stackTop[0]);
        //enqueue(q,thread);
}


void * func(){
   fprintf(stderr,"%s","hi");
}

int main(){
    thread_t  * thread;
    thread=(thread_t *)calloc(1,sizeof(thread_t));
    thread_create(thread,func,NULL);
}

#endif