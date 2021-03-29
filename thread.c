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
#include <sys/time.h>
#include <sys/resource.h>
#include <linux/futex.h>
#include <sys/time.h>

#define TGKILL 270

#define STACKSIZE ((size_t)8192 * 1024)
char *stackTop;
queue *q;
static int flag = 0;
threadlock* lock;

int initlock(threadlock lock){
	lock.value=0;
	return 0;
}
void setretval(void * t){
        thread_s * thread;
        thread = (thread_s *)t;
        thread->ret=thread->start_routine(thread->arg);
        return;
        
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
    thread_s * t=(thread_s *)calloc(1,sizeof(thread_s));
   
    //  t->stack = (char *)malloc(STACKSIZE);
    t->stack= mmap(NULL,  STACKSIZE * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (t->stack == NULL){
    	thread_unlock(*lock);
        return EAGAIN;
    }
    stackTop = t->stack + STACKSIZE;
    t->start_routine = start_routine;
    t->arg = arg;
    t->state = RUNNING;
   // t->t_id = clone((int(*)(void*))setretval, stackTop,CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND|SIGCHLD,(void *)t);
     t->t_id = clone((int(*)(void*))setretval, stackTop,CLONE_VM | CLONE_FS | CLONE_FILES |
      CLONE_THREAD |CLONE_SIGHAND | 
      CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID|CLONE_SETTLS,(void *)t,&t->waittid,t,&t->waittid);
    if (t->t_id == -1)
    {
        free(t->stack);
        free(t);
        thread_unlock(*lock);
        return EAGAIN;
    }
  
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
		if (retthread->state==SUSPENDED){
			thread_unlock(*lock);
			return EINVAL;
		}
		while(1){
			if(retthread->waittid==0){
				break;
			}
		}
		retthread->state=SUSPENDED;
		if (retval){
			//printf("%d",(int)retthread->ret);
			*retval=retthread->ret;
			thread_unlock(*lock);
		        return 0;
		}
			//Do we need to remove from queue
	}
	else{
		thread_unlock(*lock);
		return ESRCH;
	}
	return 0;
    
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
            return;
        }
        else{
            thread_unlock(*lock);
	     return ;
        }
}
int thread_kill(thread_t thread, int sig){
    thread_s *retthread;
    retthread=getthread(q,thread);
    pid_t p_id=getpid();
    if(!retthread){
        return  ESRCH;
    }
    else{
        if(sig){
                int ret = syscall(SYS_tgkill, p_id, thread, sig);
                if (ret != 0){
                    perror("kill");
                }
        }
    }

}

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

#endif
