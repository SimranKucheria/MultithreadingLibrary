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
#include <sys/time.h>
#include <stdatomic.h>

#define STACKSIZE ((size_t)8192 * 1024)
char *stackTop;
queue *q;
static int flag = 0;

int initlock(threadlock lock){
	threadlock *t = (threadlock *)calloc(1, sizeof(threadlock));
	lock=*t
	atomic_store(&(lock.value),0);
	return 0;
}
void setretval(void *t){
	thread_s *thread;
	thread = (thread_s *)t;
	thread->ret = thread->start_routine(thread->arg);
	return;
}
int thread_lock(threadlock lock){
	while (1){
		if (atomic_load(&(lock.value))==0){
			atomic_store(&(lock.value),1);
			break;
		}
	}
	return 0;
}
int thread_unlock(threadlock lock){
	if (atomic_load(&(lock.value))==1){
		atomic_store(&(lock.value),0);
	}
	return 0;
}
int thread_create(thread_t *thread, void *(*start_routine)(void *), void *arg){
	if (flag == 0){
		flag = 1;
		q = (queue *)calloc(1, sizeof(queue));
		initq(q);
	}
	thread_s *t = (thread_s *)calloc(1, sizeof(thread_s));
	//  t->stack = (char *)malloc(STACKSIZE);
	t->stack = mmap(NULL, STACKSIZE * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | MAP_STACK, -1, 0);
	if (t->stack == NULL){
		return EAGAIN;
	}
	stackTop = t->stack + STACKSIZE;
	t->start_routine = start_routine;
	t->arg = arg;
	t->state = RUNNING;
	// t->t_id = clone((int(*)(void*))setretval, stackTop,CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND|SIGCHLD,(void *)t);
	t->t_id = clone((int (*)(void *))setretval, stackTop, CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD | CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID | CLONE_SETTLS, (void *)t, &t->waittid, t, &t->waittid);
	if (t->t_id == -1){
		free(t->stack);
		free(t);
		return EAGAIN;
	}

	enqueue(q, t);
	*thread = t->t_id;
	return 0;
}
int thread_join(thread_t thread, void **retval){
	thread_s *retthread;
	retthread = getthread(q, thread);
	if (retthread){
		if (retthread->state == EXITED){
			return EINVAL;
		}
		while (1){
			if (retthread->waittid == 0){
				break;
			}
		}
		retthread->state = EXITED;
		if (retval){
			//printf("%d",(int)retthread->ret);
			*retval = retthread->ret;
			return 0;
		}
		//Do we need to remove from queue
	}
	else{
		return ESRCH;
	}
	return 0;
}
void thread_exit(void *retval){
	#ifdef SYS_gettid
	thread_t thread = syscall(SYS_gettid);
	#endif
	thread_s *retthread;
	retthread = getthread(q, thread);
	if (retthread){
		retthread->state = EXITED;
		retthread->ret = retval;
	}
	return;
}
int thread_kill(thread_t thread, int sig){
	pid_t p_id = getpid();
	if (sig){
		int ret = syscall(SYS_tgkill, p_id, thread, sig);
		if (ret != 0){
			return errno;
		}
	}
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
void *func()
{
	printf("%s", "hi");
	// char *ret;
	//if ((ret = (char*) malloc(20)) == NULL) {
	//   perror("malloc() error");
	//    exit(2);
	//}
	//strcpy(ret, "This is a test");
	//printf("%s",ret);
	//thread_exit(ret);
}

void *newfunc()
{
	// sleep(10);
	printf("%d ", getpid());
	printf("%s\n\n\n\n", "hi3");

	// sleep(10);
	return (void *)50;
}
void *thread(void *arg)
{
	char *ret;

	if ((ret = (char *)malloc(20)) == NULL)
	{
		perror("malloc() error");
		exit(2);
	}
	strcpy(ret, "This is a test");
	thread_exit(ret);
}

int main()
{
	void *status;
	thread_t thread1, thread2;
	thread_create(&thread1, newfunc, NULL);
	thread_create(&thread2, func, NULL);
	thread_join(thread1, &status);
	thread_join(thread2, NULL);
	printf("%d", (int)status);
	// thread_kill(thread1,SIGINT);
	return 0;
}
#endif
