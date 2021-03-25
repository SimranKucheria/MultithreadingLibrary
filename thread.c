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
#include <linux/sched.h>
#include <errno.h>
#include <sys/mman.h>

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
    t->t_id = clone((int (*)(void *))start_routine, stackTop,SIGCHLD | CLONE_FS | CLONE_FILES |CLONE_SIGHAND | CLONE_VM , t->arg);
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
	if (retthread){
		if (retthread->state==TERMINATED){
			thread_unlock(*lock);
			return EINVAL;
		}
		// waitpid(thread,NULL,__WCLONE);
        waitpid(thread,NULL,0);
		retthread->state=TERMINATED;
		if (retval){
			*retval=retthread->ret;
		}
		//Do we need to remove from queue
	}
	else{
		thread_unlock(*lock);
		return ESRCH;
	}
    
}

void *func(){
    //sleep(10);
    printf("%d ",getpid());
    printf("%s\n\n\n\n", "hi");
    // sleep(10);
    
}


void *newfunc(){
    //sleep(10);
    printf("%d ",getpid());
    printf("%s\n\n\n\n", "hi3");
    // sleep(10);
    
}

int main()
{
    
    thread_t thread1,thread2;
    thread_create(&thread1, func, NULL);
    thread_create(&thread2, newfunc, NULL);
    thread_join(thread1,NULL);
    thread_join(thread2,NULL);
    printf("WHy");
}

#endif
