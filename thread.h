
#include <ucontext.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
typedef enum states{
    RUNNING,RUNNABLE,SLEEPING,WAITING
}states;

typedef struct thread{
    /* unique id for thread*/
    pid_t t_id;

    /*stack for thread*/
    char stack[1024*128];

    /*context of the thread */
    ucontext_t context;

    /*function to be executed */
    void *(*start_routine) (void *);

    /*arg passed to func*/
    void * arg;

    /*state of the thread*/
    states state;

    /*return value of the thread */
    void * ret;
}thread_t;

void get_id(thread_t *);
// void * thread_init();
int thread_create(thread_t *thread,void *(*start_routine) (void *), void *arg); // provide option to use a desired mapping.
// int thread_join(thread_t thread, void **retval);
// void thread_exit(void *retval);
// int thread_lock(); // a spinlock
// int thread_unlock();  // spin-unlock
// int thread_kill(thread_t thread, int sig);