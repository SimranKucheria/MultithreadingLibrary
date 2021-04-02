#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

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
int isQfull(queue *q)
{
	return 0;
}

int isQempty(queue *q)
{
	return q->head == NULL;
}

void initq(queue *q)
{
	q->head = NULL;
	q->tail = NULL;
}

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
