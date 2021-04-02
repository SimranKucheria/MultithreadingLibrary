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
thread_s * current_thread;
static pid_t id=0;
int first_thread=0;
void initialise();
int gettid(){
    return id++;
}
void setretval(void * t){
        thread_s * thread;
        thread = (thread_s *)t;
        thread->ret=thread->start_routine(thread->arg);
        mthread_exit(thread->ret);
        
}
void  scheduler(){
    /*should handle context switching(saving the context of current and loading 
    the context of the other thread) and getting another thread from ready queue*/
    alarm(0);
    thread_s * t;
    printf("%s","inside scheduler");

    if(current_thread){
        current_thread->state=RUNNABLE;
        enqueue(readyqueue,current_thread);
    }
	t=dequeue(readyqueue);
    if(t!=NULL){
        current_thread=t;
        t->state=RUNNING;
        setcontext(&t->context);

        alarm(2);//need to set timer
    }
    else{
            exit(0);
    }
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
            thread_s * current_thread=(thread_s *)calloc(1,sizeof(thread_s));
            readyqueue=(queue *)calloc(1,sizeof(queue));
            initq(readyqueue);
            initialise();//setting the signal handler for the first time
            //timer bhi set karna hai
        }
        // sleep(10);
        alarm(0);
        thread_s * t=(thread_s *)calloc(1,sizeof(thread_s));
        getcontext(&t->context);
        t->arg=arg;
        t->state=RUNNABLE;
        t->t_id=gettid();
        t->start_routine = start_routine;
        t->context.uc_stack.ss_sp=(char *)malloc(STACKSIZE);
        t->context.uc_stack.ss_size=8192 * 1024;
        t->context.uc_stack.ss_flags=0;
        enqueue(readyqueue, t);
        makecontext(&t->context,(void (*)(void))setretval,t);
        alarm(2);
       
}
int thread_join(thread_t thread, void ** retval){
    alarm(0);
	thread_s *retthread;
   
    if(current_thread->t_id == thread) {
        alarm(2);
        return EDEADLK;
    }

	retthread=getthread(readyqueue,thread);
	if (retthread){
		if (retthread->state==EXITED){
            alarm(2);
			return EINVAL;
		}
		while(1){
			if(retthread->state==EXITED){
				break;
			}
		}
    
		if (retval){
			//printf("%d",(int)retthread->ret);
			*retval=retthread->ret;
			
		        return 0;
		}
			//Do we need to remove from queue
	}
	else{
        alarm(2);
		return ESRCH;
	}
    alarm(2);
	return 0;
    
}
void thread_exit(void *retval){
        alarm(0);
        current_thread->state=EXITED;
        current_thread->ret=retval;
        raise(SIGPROF);
}
#endif
