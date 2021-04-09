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
#include <signal.h>
#include <stdatomic.h>
#define STACKSIZE ((size_t)8192 * 1024)
queue *readyqueue, *completed;
thread_s *current_thread;
thread_s *main_thread;
static pid_t id = 0;
int first_thread = 0,main_thread_set=0;
void initialise();
int main();
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
int gettid(){
	return id++;
}
void setretval(void){
	current_thread->ret = current_thread->start_routine(current_thread->arg);
	ualarm(0,0);
	current_thread->state=EXITED;
	raise(SIGALRM);
}
void scheduler(){
    /*should handle context switching(saving the context of current and loading 
    the context of the other thread) and getting another thread from ready queue*/
    ualarm(0,0);
    thread_s *t;
    if (current_thread->state != EXITED){
	if((getcontext(&current_thread->context))==0){
    		if(current_thread->exec){
	        	current_thread->exec=0;
	        	if(current_thread->sig!=-1){
	        		raise(current_thread->sig);
	        	}
	        	ualarm(2,0);
    			return;
    		}	
    		else{	    
    			current_thread->state = RUNNABLE;
			enqueue(readyqueue, current_thread);
		}
	}
   }
   else{
   	enqueue(completed, current_thread);
   }
   t = dequeue(readyqueue);
   if (t != NULL){
	current_thread = t;
	current_thread->state = RUNNING;
	current_thread->exec=1;
	ualarm(2,0);
	setcontext(&current_thread->context);
   }
   else{
    	ualarm(0,0);
    	return;
   }
}
/*function gets called initially*/
void initialise(){
    //set signal for SIGALARM
    struct sigaction handler;
    handler.sa_handler = &scheduler;
    sigemptyset(&handler.sa_mask);
    handler.sa_flags = SA_RESTART | SA_NODEFER;
    sigaction(SIGALRM, &handler, NULL);
    ualarm(2,0);
    return;
}

int thread_create(thread_t *thread, void *(*start_routine)(void *), void *arg){
    if (first_thread == 0)
    {
        ualarm(0,0);
        first_thread = 1;
        thread_s *current_thread = (thread_s *)calloc(1, sizeof(thread_s));
        completed = (queue *)calloc(1, sizeof(queue));
        readyqueue = (queue *)calloc(1, sizeof(queue));
        initq(readyqueue);
        initq(completed);
        initialise(); //setting the signal handler for the first time
        //timer bhi set karna hai
    }
    ualarm(0,0);
    thread_s *t = (thread_s *)calloc(1, sizeof(thread_s));
    if(getcontext(&t->context)==0){
	    t->arg = arg;
	    t->state = RUNNABLE;
	    t->t_id = gettid();
	    t->start_routine = start_routine;
	    t->context.uc_stack.ss_sp = (char *)malloc(STACKSIZE);
	    t->context.uc_stack.ss_size = 8192 * 1024;
	    t->context.uc_stack.ss_flags = 0;
	    t->exec=1;
	    t->sig=-1;
	    makecontext(&t->context, (void (*)(void))setretval, 1, (void *)t);
	    enqueue(readyqueue, t);
	    *thread = t->t_id;
    }
    else
    	return errno;
    if(!main_thread_set){
        main_thread_set=1;
        thread_s *main_thread = (thread_s *)calloc(1, sizeof(thread_s));
        main_thread->context.uc_stack.ss_sp = (char *)malloc(STACKSIZE);
        main_thread->context.uc_stack.ss_size = 8192 * 1024;
        main_thread->context.uc_stack.ss_flags = 0;
        main_thread->state=RUNNABLE;
        main_thread->t_id=gettid();
        main_thread->exec=0;
        main_thread->sig=-1;
        current_thread=main_thread;
        if((getcontext(&main_thread->context))==0){
        	if(main_thread->exec){
	                main_thread->exec=0;
	                if(main_thread->sig!=-1){
	        		raise(main_thread->sig);
	        	}
        	        ualarm(2,0);
        		return 0;
        	}
        	else{
        		raise(SIGALRM);
        	}
        }
        else
        	return errno;
    }
    ualarm(2,0);
    return 0;
}
int thread_join(thread_t thread, void **retval){
    ualarm(0,0);
    thread_s *retthread;
    if (current_thread && current_thread->t_id == thread)
    {
        ualarm(2,0);
        return EDEADLK;
    }
    retthread = getthread(readyqueue, thread);
    if (retthread)
    {
        if (retthread->state == EXITED)
        {
            ualarm(2,0);
            return EINVAL;
        }
        while (1)
        {
            if (retthread->state == EXITED)
            {
                break;
            }
            raise(SIGALRM);
        }
        if (retval)
        {
            *retval = retthread->ret;
        }
        ualarm(2,0);
        return 0;
        //Do we need to remove from queue
    }
    else
    {
    	retthread = getthread(completed, thread);
    	if (retthread)
    	{
            if(retval)
            {
	            *retval = retthread->ret;
            }
            ualarm(2,0);
            return 0;
        }
        else
        {
        	ualarm(2,0);
        	return ESRCH;
        }
    }
    //return 0;
}
void thread_exit(void *retval){
    ualarm(0,0);
    if (current_thread)
    {
        current_thread->state = EXITED;
        current_thread->ret = retval;
    }
    raise(SIGALRM);
    return;
}

int thread_kill(thread_t thread, int sig){
	ualarm(0,0);
	if(sig){
		if (current_thread->t_id==thread){
			if(raise(sig)==0){
				ualarm(2,0);
				return 0;
			}
		}
		else{
			if(makeheadthread(readyqueue,thread)){
				thread_s *retthread;
				retthread=getthread(readyqueue,thread);
				retthread->sig=sig;
				raise(SIGALRM);
				ualarm(2,0);
				return 0;
			}
			else{
				return EINVAL;
			}
		}
	}
	ualarm(2,0);
	return 0;
}
#endif
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