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
#include <linux/futex.h>
#define STACKSIZE ((size_t)8192 * 1024)
queue *readyqueue, *completed;
thread_s *current_thread;
thread_s *main_thread;
static pid_t id = 0;
struct itimerval timer;
int first_thread = 0, main_thread_set = 0;
void initialise();
int main();
int initmutexlock(threadmutexlock *lock)
{
    lock = (threadmutexlock *)calloc(1, sizeof(threadmutexlock));
    lock->value = 0;
    return 0;
}
int thread_mutex_lock(threadmutexlock *lock)
{
    while ((atomic_flag_test_and_set_explicit(&lock->value, 1)) != 0)
    {
        syscall(SYS_futex, lock->value, FUTEX_WAIT, 1, NULL, NULL, 0);
    }
    return 0;
}
int thread_mutex_unlock(threadmutexlock *lock)
{
    if (atomic_fetch_sub(&lock->value, 1) != 1)
    {
        lock->value = 0;
        syscall(SYS_futex, lock->value, FUTEX_WAKE, 1, NULL, NULL, 0);
    }
    return 0;
}
int initlock(threadlock *lock)
{
    lock = (threadlock *)calloc(1, sizeof(threadlock));
    if (!lock)
    {
        return EINVAL;
    }
    atomic_store(&lock->value, 0);
    return 0;
}
int thread_lock(threadlock *lock)
{
    while (atomic_flag_test_and_set_explicit(&lock->value, 1))
        ;
    return 0;
}
int thread_unlock(threadlock *lock)
{
    atomic_store(&lock->value, 0);
    return 0;
}
int gettid()
{
    return id++;
}
void setretval(void)
{
    current_thread->ret = current_thread->start_routine(current_thread->arg);
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
    current_thread->state = EXITED;
    raise(SIGALRM);
}
void scheduler()
{
    /*should handle context switching(saving the context of current and loading 
    the context of the other thread) and getting another thread from ready queue*/
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
    thread_s *t;
    if (current_thread->state != EXITED)
    {
        if ((getcontext(current_thread->context)) == 0)
        {
            if (current_thread->exec)
            {
                current_thread->exec = 0;
                if (current_thread->sig != -1)
                {
                    raise(current_thread->sig);
                }
                timer.it_value.tv_sec = 0;
                timer.it_value.tv_usec = 2000;
                timer.it_interval.tv_sec = 0;
                timer.it_interval.tv_usec = 2000;
                setitimer(ITIMER_REAL, &timer, NULL);
                return;
            }
            else
            {
                current_thread->state = RUNNABLE;
                enqueue(readyqueue, current_thread);
            }
        }
    }
    else
    {
        enqueue(completed, current_thread);
    }
    t = dequeue(readyqueue);
    if (t != NULL)
    {
        current_thread = t;
        current_thread->state = RUNNING;
        current_thread->exec = 1;
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 2000;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 2000;
        setitimer(ITIMER_REAL, &timer, NULL);
        setcontext(current_thread->context);
    }
    else
    {

        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 0;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 0;
        setitimer(ITIMER_REAL, &timer, NULL);
        return;
    }
}
/*function gets called initially*/
void initialise()
{
    struct sigaction handler;
    memset(&handler, 0, sizeof(handler));
    handler.sa_handler = &scheduler;
    sigemptyset(&handler.sa_mask);
    handler.sa_flags = SA_RESTART | SA_NODEFER;
    sigaction(SIGALRM, &handler, NULL);
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 2000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 2000;
    setitimer(ITIMER_REAL, &timer, NULL);
}

int thread_create(thread_t *thread, void *(*start_routine)(void *), void *arg)
{
    if (first_thread == 0)
    {
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 0;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 0;
        setitimer(ITIMER_REAL, &timer, NULL);
        first_thread = 1;
        current_thread = (thread_s *)calloc(1, sizeof(thread_s));
        completed = (queue *)calloc(1, sizeof(queue));
        readyqueue = (queue *)calloc(1, sizeof(queue));
        if (!current_thread || !completed || !readyqueue)
        {
            return EINVAL;
        }
        initq(readyqueue);
        initq(completed);
        initialise(); //setting the signal handler for the first time
        //timer bhi set karna hai
    }
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
    thread_s *t = (thread_s *)calloc(1, sizeof(thread_s));
    if (!t)
    {
        return EINVAL;
    }
    t->context = (ucontext_t *)calloc(1, sizeof(ucontext_t));
    if (!t->context)
    {
        return EINVAL;
    }
    if (getcontext(t->context) == 0)
    {
        t->arg = arg;
        t->state = RUNNABLE;
        t->t_id = gettid();
        t->start_routine = start_routine;
        t->context->uc_stack.ss_sp = (char *)calloc(1, STACKSIZE);
        if (!t->context->uc_stack.ss_sp)
            return EINVAL;
        t->context->uc_stack.ss_size = 8192 * 1024;
        t->context->uc_stack.ss_flags = 0;
        t->exec = 0;
        t->sig = -1;
        makecontext(t->context, (void (*)(void))setretval, 1, (void *)t);
        enqueue(readyqueue, t);
        *thread = t->t_id;
    }
    else
        return errno;
    if (!main_thread_set)
    {
        main_thread_set = 1;
        main_thread = (thread_s *)calloc(1, sizeof(thread_s));
        if (!main_thread)
            return EINVAL;
        main_thread->context = (ucontext_t *)calloc(1, sizeof(ucontext_t));
        if (!main_thread->context)
        {
            return EINVAL;
        }
        main_thread->context->uc_stack.ss_sp = (char *)calloc(1, STACKSIZE);
        if (!main_thread->context->uc_stack.ss_sp)
            return EINVAL;
        main_thread->context->uc_stack.ss_size = 8192 * 1024;
        main_thread->context->uc_stack.ss_flags = 0;
        main_thread->state = RUNNABLE;
        main_thread->t_id = gettid();
        main_thread->exec = 0;
        main_thread->sig = -1;
        current_thread = main_thread;
        raise(SIGALRM);
        // fprintf(stderr,"going to main");
        return 0;
    }
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 2000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 2000;
    setitimer(ITIMER_REAL, &timer, NULL);
    return 0;
}
int thread_join(thread_t thread, void **retval)
{
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
    thread_s *retthread;
    if (current_thread && current_thread->t_id == thread)
    {
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 2000;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 2000;
        setitimer(ITIMER_REAL, &timer, NULL);
        return EDEADLK;
    }
    retthread = getthread(readyqueue, thread);
    if (retthread)
    {
        if (retthread->state == EXITED)
        {
            timer.it_value.tv_sec = 0;
            timer.it_value.tv_usec = 2000;
            timer.it_interval.tv_sec = 0;
            timer.it_interval.tv_usec = 2000;
            setitimer(ITIMER_REAL, &timer, NULL);
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
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 2000;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 2000;
        setitimer(ITIMER_REAL, &timer, NULL);
        return 0;
    }
    else
    {
        retthread = getthread(completed, thread);
        if (retthread)
        {
            if (retval)
            {
                *retval = retthread->ret;
            }
            timer.it_value.tv_sec = 0;
            timer.it_value.tv_usec = 2000;
            timer.it_interval.tv_sec = 0;
            timer.it_interval.tv_usec = 2000;
            setitimer(ITIMER_REAL, &timer, NULL);
            return 0;
        }
        else
        {
            timer.it_value.tv_sec = 0;
            timer.it_value.tv_usec = 2000;
            timer.it_interval.tv_sec = 0;
            timer.it_interval.tv_usec = 2000;
            setitimer(ITIMER_REAL, &timer, NULL);
            return ESRCH;
        }
    }
    //return 0;
}
void thread_exit(void *retval)
{
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
    if (current_thread)
    {
        current_thread->state = EXITED;
        current_thread->ret = retval;
    }
    raise(SIGALRM);
    return;
}

int thread_kill(thread_t thread, int sig)
{
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
    if (sig > 0 && sig < 32)
    {
        if (current_thread->t_id == thread)
        {
            if (raise(sig) == 0)
            {
                timer.it_value.tv_sec = 0;
                timer.it_value.tv_usec = 2000;
                timer.it_interval.tv_sec = 0;
                timer.it_interval.tv_usec = 2000;
                setitimer(ITIMER_REAL, &timer, NULL);
                return 0;
            }
        }
        else
        {
            if (makeheadthread(readyqueue, thread))
            {
                thread_s *retthread;
                retthread = getthread(readyqueue, thread);
                retthread->sig = sig;
                raise(SIGALRM);
                timer.it_value.tv_sec = 0;
                timer.it_value.tv_usec = 2000;
                timer.it_interval.tv_sec = 0;
                timer.it_interval.tv_usec = 2000;

                setitimer(ITIMER_REAL, &timer, NULL);
                return 0;
            }
            else
            {
                return ESRCH;
            }
        }
    }
    if (sig != 0)
    {
        return EINVAL;
    }

    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 2000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 2000;
    setitimer(ITIMER_REAL, &timer, NULL);
    return 0;
}

#endif
