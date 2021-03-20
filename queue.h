#include "thread.h"
typedef struct nodeq{
	thread_t * thread;
	struct nodeq * next;
}nodeq;

typedef struct queue{
	nodeq * head;
	nodeq * tail;
}queue;

void enqueue(queue * q,  thread_t * thread); 
thread_t *  dequeue(queue * q);  
int isQfull(queue * q);
int isQempty(queue * q);
void initq(queue * q);
