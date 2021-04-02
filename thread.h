
#include <ucontext.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

typedef enum states{
    RUNNING,RUNNABLE,EXITED
}states;

typedef pid_t thread_t;
typedef struct thread_s{
    /* unique id for thread*/
    thread_t t_id;

    /*stack for thread*/
    char * stack;

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
    
    /*Value to wait for thread*/
    pid_t waittid;
}thread_s;

typedef struct threadlock{
    int value;
}threadlock;


int thread_create(thread_t *,void *(*) (void *), void *); // provide option to use a desired mapping.
//int thread_create(thread_t *thread,const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg);//after defining attr structure use this definition
int thread_join(thread_t , void **);
void thread_exit(void *retval);
int initlock(threadlock );
int thread_lock(threadlock );
int thread_unlock(threadlock);  
int thread_kill(thread_t , int );

