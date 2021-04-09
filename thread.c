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
#include <linux/futex.h>
#define STACKSIZE ((size_t)8192 * 1024)
char *stackTop;
queue *q;
static int flag = 0;
int initmutexlock(threadmutexlock * lock){
    lock  = (threadmutexlock *)calloc(1, sizeof(threadmutexlock));
	lock->value=0;
	return 0;
}
int thread_mutex_lock(threadmutexlock * lock){
      while ((atomic_flag_test_and_set_explicit(&lock->value,1)) != 0) 
    {
            syscall(SYS_futex, lock->value, FUTEX_WAIT, 1, NULL, NULL, 0);

    }
}
int thread_mutex_unlock(threadmutexlock * lock){
    if(atomic_fetch_sub(&lock->value, 1) != 1)  {
        lock->value = 0;
        syscall(SYS_futex, lock->value, FUTEX_WAKE, 1, NULL, NULL, 0);
   }
   return 0;
}

void setretval(void *t){
	thread_s *thread;
	thread = (thread_s *)t;
	thread->ret = thread->start_routine(thread->arg);
	return;
}
int initlock(threadlock * lock){
	lock= (threadlock *)calloc(1, sizeof(threadlock));
	lock->value=0;
	return 0;
}
int thread_lock(threadlock * lock){
	while (atomic_flag_test_and_set_explicit(&lock->value,1));
	return 0;
}
int thread_unlock(threadlock * lock){
	atomic_store(&lock->value,0);
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
#include <string.h>
#include <unistd.h>
static volatile int glob = 0;
static threadlock splock;
static int useMutex = 0;
static int numOuterLoops;
static int numInnerLoops;

static void *
threadFunc(void *arg)
{
    int s;

    for (int j = 0; j < numOuterLoops; j++) {
       
        s = thread_lock(&splock);
        if (s != 0)
                //errExitEN(s, "pthread_spin_lock");
         printf("hiS");
    

        for (int k = 0; k < numInnerLoops; k++)
            glob++;

        s = thread_unlock(&splock);
        if (s != 0)
                //errExitEN(s, "pthread_spin_unlock");
        printf("hi");
        
    }

    return NULL;
}

static void
usageError(char *pname)
{
    fprintf(stderr,
            "Usage: %s [-s] num-threads "
            "[num-inner-loops [num-outer-loops]]\n", pname);
    fprintf(stderr,
            "    -q   Don't print verbose messages\n");
    fprintf(stderr,
            "    -s   Use spin locks (instead of the default mutexes)\n");
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    int opt, s;
    int numThreads;
    thread_t *thread;
    int verbose;

    /* Prevent runaway/forgotten process from burning up CPU time forever */

    alarm(120);         /* Unhandled SIGALRM will kill process */

    useMutex = 0;
    verbose = 1;
    while ((opt = getopt(argc, argv, "qs")) != -1) {
        switch (opt) {
        case 'q':
            verbose = 0;
            break;
        case 's':
            printf("here");
            useMutex = 0;
            break;
        default:
            usageError(argv[0]);
        }
    }

    if (optind >= argc)
        usageError(argv[0]);

    numThreads = atoi(argv[optind]);
    numInnerLoops = (optind + 1 < argc) ? atoi(argv[optind + 1]) : 1;
    numOuterLoops = (optind + 2 < argc) ? atoi(argv[optind + 2]) : 10000000;

    if (verbose) {
        printf("Using %s\n", useMutex ? "mutexes" : "spin locks");
        printf("\tthreads: %d; outer loops: %d; inner loops: %d\n",
                numThreads, numOuterLoops, numInnerLoops);
    }

    thread = calloc(numThreads, sizeof(thread_t));
    if (thread == NULL)
        printf("calloc");


    s = initlock(&splock);
    if (s != 0)
           // errExitEN(s, "pthread_spin_init");
        printf("hi");
    

    for (int j = 0; j < numThreads; j++) {
        s = thread_create(&thread[j], threadFunc, NULL);
        if (s != 0)
           // errExitEN(s, "pthread_create");
            printf("hi");
    }

    for (int j = 0; j < numThreads; j++) {
        s = thread_join(thread[j], NULL);
        if (s != 0)
            printf("hi");
            //errExitEN(s, "pthread_join");
    }

    if (verbose){
        printf("glob = %d\n", glob);
    }
    exit(EXIT_SUCCESS);
}

#endif
