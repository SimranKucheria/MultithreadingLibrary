#include "queue.h"
#include<stdlib.h>
#include<stdio.h>

void enqueue(queue * q, thread_t * thread){
	node * newnode=(node *)malloc(sizeof(node));
	newnode->thread=thread;
	newnode->next=NULL;
	if(!newnode){
		return;
	}
	else{
		if(q->head==NULL){
			q->head=newnode;
			q->tail=newnode;
			return;
		}
		else{
			q->tail->next=newnode;
			q->tail=newnode;
			return;
		}
	}
}

thread_t *dequeue(queue * q){
	if(q->head==NULL){
		return 0;
	}
	else{
		thread_t * x=0;
		node * temp;
		x=q->head->thread;
		temp=q->head;
		q->head=q->head->next;
		free(temp);
		return x;
	}
}
int isQfull(queue * q){
	return 0;
}

int isQempty(queue * q){
	return q->head==NULL;
}

void initq(queue * q){
	q->head=NULL;
	q->tail=NULL;
}

	
	
