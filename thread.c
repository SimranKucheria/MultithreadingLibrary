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
static struct thread_lock lock;
/* something along these lines needs to be implemented
1. Allocate stack space for each thread 
2. Stack +stacksize is done because s
tack grows downwards
 */
int thread_create(thread_t *thread, void *(*start_routine)(void *), void *arg){
    int status;
    thread_s * t=(thread_s *)calloc(1,sizeof(thread_s));
    // t->stack = (char *)malloc(getpagesize());
    t->stack= mmap(NULL, STACKSIZE * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (t->stack == NULL){
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
        return EAGAIN;
    }
    // printf("%s", stackTop);
    if (flag==0){
    	flag=1;
    	q=(queue *)calloc(1,sizeof(queue));
    	initq(q);
    }
    enqueue(q, t);
    *thread=t->t_id;
    return 0;
}
int thread_join(thread_t thread, void ** retval){
	thread_s *retthread;
	retthread=getthread(q,thread);
	if (retthread){
		if (retthread->state==TERMINATED){
			return EINVAL;
		}
		// waitpid(thread,NULL,__WCLONE);
        wait(&thread);
		retthread->state=TERMINATED;
		if (retval){
			*retval=retthread->ret;
		}
		//Do we need to remove from queue
	}
	else{
		return ESRCH;
	}
    
}

void *func(){
    sleep(10);
    printf("%d ",getpid());
    printf("%s\n\n\n\n", "hi");
    // sleep(10);
    
}

int main()
{
    
    thread_t thread;
    thread_create(&thread, func, NULL);
    thread_join(thread,NULL);
}

#endif
