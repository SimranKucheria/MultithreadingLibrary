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
#define STACKSIZE ((size_t)8192 * 1024)
queue *readyqueue, *completed;
thread_s *current_thread;
static pid_t id = 0;
int first_thread = 0;
void initialise();
int gettid()
{
    return id++;
}
void setretval(void *t)
{
    thread_s *thread;
    thread = (thread_s *)t;
    thread->ret = thread->start_routine(thread->arg);
    thread_exit(thread->ret);
}
void scheduler()
{
    /*should handle context switching(saving the context of current and loading 
    the context of the other thread) and getting another thread from ready queue*/
    alarm(0);
    thread_s *t;
    //printf("%s", "inside scheduler");

    if (current_thread)
    {
        if (current_thread->state != EXITED)
        {
            current_thread->state = RUNNABLE;
            enqueue(readyqueue, current_thread);
        }
        else
        {
            enqueue(completed, current_thread);
        }
    }
    t = dequeue(readyqueue);
    if (t != NULL)
    {
        current_thread = t;
        t->state = RUNNING;
        setcontext(&t->context);

        alarm(2); //need to set timer
    }
    else
    {
        exit(0);
    }
}
/*function gets called initially*/
void initialise()
{
    //set signal for SIGPROF
    struct sigaction handler;
    handler.sa_handler = &scheduler;
    sigemptyset(&handler.sa_mask);
    handler.sa_flags = SA_RESTART | SA_NODEFER;
    sigaction(SIGALRM, &handler, NULL);
    alarm(2);
}
int thread_create(thread_t *thread, void *(*start_routine)(void *), void *arg)
{
    if (first_thread == 0)
    {
        first_thread = 1;
        thread_s *current_thread = (thread_s *)calloc(1, sizeof(thread_s));
        readyqueue = (queue *)calloc(1, sizeof(queue));
        initq(readyqueue);
        completed = (queue *)calloc(1, sizeof(queue));
        initq(completed);
        initialise(); //setting the signal handler for the first time
        //timer bhi set karna hai
    }
    // sleep(10);
    alarm(0);
    thread_s *t = (thread_s *)calloc(1, sizeof(thread_s));
    getcontext(&t->context);
    t->arg = arg;
    t->state = RUNNABLE;
    t->t_id = gettid();
    t->start_routine = start_routine;
    t->context.uc_stack.ss_sp = (char *)malloc(STACKSIZE);
    t->context.uc_stack.ss_size = 8192 * 1024;
    t->context.uc_stack.ss_flags = 0;
    alarm(2);
    enqueue(readyqueue, t);
    makecontext(&t->context, (void (*)(void))setretval, 1, (void *)t);
    *thread = t->t_id;
    return 0;
}
int thread_join(thread_t thread, void **retval)
{
    alarm(0);
    thread_s *retthread;

    if (current_thread && current_thread->t_id == thread)
    {
        alarm(2);
        return EDEADLK;
    }

    retthread = getthread(readyqueue, thread);
    if (retthread)
    {
        if (retthread->state == EXITED)
        {
            return EINVAL;
        }
        alarm(2);
        while (1)
        {
            if (retthread->state == EXITED)
            {
                break;
            }
        }

        if (retval)
        {
            //printf("%d",(int)retthread->ret);
            *retval = retthread->ret;

            return 0;
        }
        //Do we need to remove from queue
    }
    else
    {
        alarm(2);
        return ESRCH;
    }
    alarm(2);
    return 0;
}
void thread_exit(void *retval)
{
    alarm(0);
#ifdef SYS_gettid
    thread_t thread = syscall(SYS_gettid);
#endif
    thread_s *retthread;
    retthread = getthread(readyqueue, thread);
    if (retthread)
    {
        retthread->state = EXITED;
        retthread->ret = retval;
    }
    raise(SIGALRM);
    return;
}
#endif

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
    thread_t thread1;
    thread_create(&thread1, newfunc, NULL);
    thread_join(thread1, &status);
    printf("%d", (int)status);
    // thread_kill(thread1,SIGINT);
    return 0;
}
