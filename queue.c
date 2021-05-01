/*Linked List implementation of Queue */

#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

/*initialises queue*/
void initq(queue *q)
{
	q->head = NULL;
	q->tail = NULL;
}

/*Enqueues threads into a queue data structure*/
void enqueue(queue *q, thread_s *thread)
{
	nodeq *newnode = (nodeq *)malloc(sizeof(nodeq));
	newnode->thread = thread;
	newnode->next = NULL;
	if (!newnode)
	{
		return;
	}
	else
	{
		if (q->head == NULL)
		{
			q->head = newnode;
			q->tail = newnode;
			return;
		}
		else
		{
			q->tail->next = newnode;
			q->tail = newnode;
			return;
		}
	}
}
/*Dequeues threads from a queue data structure*/
thread_s *dequeue(queue *q)
{
	if (q->head == NULL)
	{
		return 0;
	}
	else
	{
		thread_s *x = 0;
		nodeq *temp;
		x = q->head->thread;
		temp = q->head;
		q->head = q->head->next;
		free(temp);
		return x;
	}
}
/* Checks if the queue is full  */
int isQfull(queue *q)
{
	if (q)
		return 0;
	return -1;
}
/* Checks if the queue is empty  */
int isQempty(queue *q)
{
	return q->head == NULL;
}

/* Takes a thread id and returns a pointer pointing to the thread structure */
thread_s *getthread(queue *q, thread_t thread)
{
	nodeq *temp;
	temp = q->head;
	if (temp == NULL)
	{
		return 0;
	}
	if (!isQempty(q))
	{
		while (temp != q->tail)
		{
			if (thread == temp->thread->t_id)
			{
				return temp->thread;
			}
			temp = temp->next;
		}
		if (thread == temp->thread->t_id)
		{
			return temp->thread;
		}
	}
	return NULL;
}
/*Takes a thread ID and makes the thread structure the head of the queue*/
int makeheadthread(queue *q, thread_t athread)
{
	nodeq *temp;
	nodeq *t1;
	nodeq *prev;
	temp = q->head;
	if (temp == NULL)
	{
		return 0;
	}
	if (!isQempty(q))
	{
		while (temp != q->tail)
		{
			if (athread == temp->thread->t_id)
			{
				t1 = temp;
				break;
			}
			prev = temp;
			temp = temp->next;
		}
		if (temp == q->tail && athread == temp->thread->t_id)
		{
			t1 = temp;
		}
	}
	if (t1 == q->head)
	{
		return 1;
	}
	if (t1 == NULL)
	{
		return 0;
	}
	if (t1->next == NULL)
	{
		prev->next = NULL;
		q->tail = prev;
		t1->next = q->head;
		q->head = t1;
		return 1;
	}
	if (t1->next)
	{
		prev->next = t1->next;
		t1->next = q->head;
		q->head = t1;
		return 1;
	}
	return 0;
}
