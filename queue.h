/* Header file for Queue.c */
#include "thread.h"
typedef struct nodeq{
	thread_s * thread;
	struct nodeq * next;
}nodeq;

typedef struct queue{
	nodeq * head;
	nodeq * tail;
}queue;

void enqueue(queue * q,  thread_s * thread); 
thread_s *  dequeue(queue * q);  
int isQfull(queue * q);
int isQempty(queue * q);
void initq(queue * q);
thread_s * getthread(queue * q, thread_t thread);
int makeheadthread(queue *q,thread_t athread);
