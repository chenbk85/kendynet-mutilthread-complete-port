#include <stdlib.h>
#include <stdio.h>
#include "KendyNet.h"
#include "link_list.h"

typedef struct link_list
{
	int size;
	list_node *head;
	list_node *tail;
	
}link_list;

link_list *create_list()
{
	link_list *l = malloc(sizeof(link_list));
	l->size = 0;
	l->head = l->tail = 0;
	return l;
}

void destroy_list(link_list **l)
{
	free(*l);
	*l = 0;
}

void list_clear(link_list *l)
{
	l->size = 0;
	l->head = l->tail = 0;
}

void list_push_back(link_list *l,list_node *n)
{
	if(n->next)
		return;
	n->next = 0;
	if(0 == l->size)
	{
		l->head = l->tail = n;
	}
	else
	{
		l->tail->next = n;
		l->tail = n;
	}
	++l->size;
}

void list_push_front(link_list *l,list_node *n)
{
	if(n->next)
		return;
	n->next = 0;
	if(0 == l->size)
	{
		l->head = l->tail = n;
	}
	else
	{
		n->next = l->head;
		l->head = n;
	}
	++l->size;

}

list_node* list_pop(link_list *l)
{
	if(0 == l->size)
		return 0;
	list_node *ret = l->head;
	l->head = l->head->next;
	if(0 == l->head)
		l->tail = 0;
	--l->size;
	ret->next = 0;
	return ret;
}

inline int list_is_empty(link_list *l)
{
	return l->size == 0;
}

//阻塞队列,当queue为空时,可选择阻塞在pop操作上,直到push把其唤醒
typedef struct block_queue
{
	LIST_NODE;
	link_list *l;
	mutex_t     mtx;
	condition_t cond;
	volatile int force_wakeup; 
}block_queue;

block_queue* create_block_queue()
{
	block_queue *bq = malloc(sizeof(block_queue));
	bq->l = LIST_CREATE();
	bq->force_wakeup = 0;
	bq->mtx = mutex_create();
	bq->cond = condition_create();
	return bq;
}

void destroy_block_queue(block_queue **bq)
{
	LIST_DESTROY(&(*bq)->l);
	mutex_destroy(&(*bq)->mtx);
	condition_destroy(&(*bq)->cond);
	free(*bq);
	*bq = 0;
}

void block_queue_push(block_queue *bq,list_node *n)
{
	mutex_lock(bq->mtx);
	int isEmpty = LIST_IS_EMPTY(bq->l);
	LIST_PUSH_BACK(bq->l,n);
	mutex_unlock(bq->mtx);
	if(isEmpty)
		condition_signal(bq->cond);
	
}


int block_queue_pop(block_queue *bq,list_node **n,int ms)
{
	int ret_code = POP_SUCCESS;
	*n = 0;
	mutex_lock(bq->mtx);
	while(LIST_IS_EMPTY(bq->l))
	{
		if(ms == 0)
		{
			break;//不等待,马上返回
		}	
		if(ms < 0)	
			condition_wait(bq->cond,bq->mtx);//无限等待
		else if(ms > 0)
		{
			//等待ms时间
			if(0 != condition_timedwait(bq->cond,bq->mtx,ms))
			{
				ret_code = POP_TIMEOUT;
				break;
			}
		}			
		if(bq->force_wakeup)
		{
			bq->force_wakeup = 0;
			ret_code = POP_FORCE_WAKE_UP;
			break;
		}
	}
	if(ret_code == POP_SUCCESS)
		*n = LIST_POP(list_node*,bq->l);
	mutex_unlock(bq->mtx);
	return ret_code;
}

inline int block_queue_is_empty(block_queue *bq)
{
	return LIST_IS_EMPTY(bq->l);
}

void block_queue_force_wakeup(block_queue *bq)
{
	mutex_lock(bq->mtx);
	bq->force_wakeup = 1;
	mutex_unlock(bq->mtx);
	condition_signal(bq->cond);
	
}

void block_queue_clear(block_queue *bq)
{
	mutex_lock(bq->mtx);
	list_clear(bq->l);
	mutex_unlock(bq->mtx);
}
