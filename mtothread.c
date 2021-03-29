#define _GNU_SOURCE
#include <sched.h>
#ifndef THREAD_H
#define THREAD_H
#include "string.h"
#include "queue.c"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <ucontext.h>
#include <sys/time.h>
#include <sys/resource.h>
#include<signal.h>
#define STACKSIZE ((size_t)8192 * 1024)
queue * readyqueue,*completed;
static pid_t id=0;
int first_thread=0;
void initialise();
int gettid(){
    return id++;
}
void  scheduler(){
    /*should handle context switching(saving the context of current and loading 
    the context of the other thread) and getting another thread from ready queue*/
    thread_s * t;
    printf("%s","inside scheduler");
	t=dequeue(readyqueue);
    setcontext(&t->context);
    alarm(2);//need to set timer
}
/*function gets called initially*/
void initialise(){
    //set signal for SIGPROF 
    struct sigaction handler;
    handler.sa_handler=&scheduler;
    sigemptyset(&handler.sa_mask);
    handler.sa_flags =SA_RESTART |SA_NODEFER;
    sigaction(SIGPROF,&handler,NULL);
    alarm(2);
}
int thread_create(thread_t *thread, void *(*start_routine)(void *), void *arg){
        if(first_thread==0){
            first_thread=1;
            readyqueue=(queue *)calloc(1,sizeof(queue));
            initq(readyqueue);
            initialise();//setting the signal handler for the first time
            //timer bhi set karna hai
        }
        sleep(10);
        thread_s * t=(thread_s *)calloc(1,sizeof(thread_s));
        getcontext(&t->context);
        t->arg=arg;
        t->state=RUNNABLE;
        t->t_id=gettid();
        t->start_routine = start_routine;
        t->context.uc_stack.ss_sp=(char *)malloc(STACKSIZE);
        t->context.uc_stack.ss_size=8192 * 1024;
        t->context.uc_stack.ss_flags=0;
        makecontext(&t->context,(void (*)(void))start_routine,(int)t->arg);
        enqueue(readyqueue, t);
       
}
#endif

void * func(void * arg){
    printf("%s","hi");
}
int main(){
    thread_t t1;
    thread_create(&t1,func,NULL);
}