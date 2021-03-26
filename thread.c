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
#include <linux/sched.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <ucontext.h>
#define STACKSIZE ((size_t)8192 * 1024)
char *stackTop;
queue *q;
static int flag = 0;
threadlock* lock;
/* something along these lines needs to be implemented
1. Allocate stack space for each thread 
2. Stack +stacksize is done because s
tack grows downwards
 */
int initlock(threadlock lock){
	lock.value=0;
}
void * setretval(thread_s * t){
    //thread_s * t = getthread(q,thread);
//    if(setcontext(&t->context)!=-1){
         //t->start_routine(t->arg);
         //printf("%d",*(int *)t->ret);
        //  setcontext(&t->context);
    // }
}
int thread_lock(threadlock lock){
	while(1){
		if(lock.value==0){
			lock.value=1;
			break;
		}
	}
	return 0;
}
int thread_unlock(threadlock lock){
	if(lock.value==1){
		lock.value=0;
	}
	return 0;
}
int thread_create(thread_t *thread, void *(*start_routine)(void *), void *arg){
    if (flag==0){
    	flag=1;
    	q=(queue *)calloc(1,sizeof(queue));
    	initq(q);
    	lock=(threadlock *)calloc(1,sizeof(threadlock));
    	initlock(*lock);
    }
    thread_lock(*lock);
    int status;
    thread_s * t=(thread_s *)calloc(1,sizeof(thread_s));
    // t->stack = (char *)malloc(getpagesize());
    t->stack= mmap(NULL, STACKSIZE * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (t->stack == NULL){
    	thread_unlock(*lock);
        return EAGAIN;
    }
    stackTop = t->stack + STACKSIZE;
    t->start_routine = start_routine;
    t->arg = arg;
    t->state = RUNNING;
    t->t_id = clone((int(*)(void*))setretval, stackTop,SIGCHLD | CLONE_FS | CLONE_FILES |CLONE_SIGHAND | CLONE_VM ,&t);
    // printf("%d ",getpid());
    
    if (t->t_id == -1)
    {
        free(t->stack);
        free(t);
        thread_unlock(*lock);
        return EAGAIN;
    }
    // printf("%s", stackTop);
    enqueue(q, t);
    *thread=t->t_id;
    thread_unlock(*lock);
    return 0;
}
int thread_join(thread_t thread, void ** retval){
	thread_lock(*lock);
	thread_s *retthread;
    
	retthread=getthread(q,thread);
    //  if(getcontext(&retthread->context)==0){
	if (retthread){
		if (retthread->state==SUSPENDED){
			thread_unlock(*lock);
			return EINVAL;
		}
		// waitpid(thread,NULL,__WCLONE);
        waitpid(thread,NULL,0);
		retthread->state=SUSPENDED;
		if (retval){
            
            // if(getcontext(&retthread->context)==0){
                    *retval=retthread->ret;
            // }
		}
		//Do we need to remove from queue
	}
	else{
		thread_unlock(*lock);
		return ESRCH;
	}
    
}
void thread_exit(void *retval){
        thread_lock(*lock);
        #ifdef SYS_gettid
        thread_t thread = syscall(SYS_gettid);
        #endif
        thread_s *retthread;
        retthread=getthread(q,thread);
        if (retthread){
            retthread->state=EXITED;
            retthread->ret=retval;
        }
        else{
            thread_unlock(*lock);
		    return ;
        }

}
void *func(){
    char *ret;
    if ((ret = (char*) malloc(20)) == NULL) {
        perror("malloc() error");
        exit(2);
    }
    strcpy(ret, "This is a test");
    printf("%s",ret);
    //thread_exit(ret);
}


void *newfunc(){
    //sleep(10);
    printf("%d ",getpid());
    printf("%s\n\n\n\n", "hi3");
    // sleep(10);
    // return (void *)10;
}

int main()
{
    int ret;
    thread_t thread1,thread2;
    thread_create(&thread1, func, NULL);
    thread_create(&thread2, newfunc, NULL);
    //printf("")
    thread_join(thread1,NULL);
    thread_join(thread2,NULL);
   // printf("thread exited with '%d'\n",ret);
    
    //printf("WHy");
}

#endif
