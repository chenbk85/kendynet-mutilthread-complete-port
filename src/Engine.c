#include "Engine.h"
#include "link_list.h"
#include "KendyNet.h"
#include <stdlib.h>
#include "epoll.h"
#include <assert.h>
#include "MsgQueue.h"

engine_t create_engine()
{
	engine_t e = malloc(sizeof(*e));
	if(e)
	{
		e->mtx = spin_create();//mutex_create();
		e->status = 0;
		e->buffering_event_queue = LIST_CREATE();
		e->block_thread_queue = LIST_CREATE();
		e->Init = epoll_init;
		e->Loop = epoll_loop;
		e->Register = epoll_register;
		e->UnRegister = epoll_unregister;
	}
	return e;
}

void   free_engine(engine_t *e)
{
	assert(e);
	assert(*e);
	//mutex_destroy(&(*e)->mtx);
	spin_destroy(&(*e)->mtx);
	LIST_DESTROY(&(*e)->buffering_event_queue);
	LIST_DESTROY(&(*e)->block_thread_queue);
	free(*e);
	*e = 0;
}

int put_event(engine_t e,st_io *io)
{
	assert(e);
	assert(io);
	/*
	mutex_lock(e->mtx);
	struct block_queue *EventQ = LIST_POP(struct block_queue*,e->block_thread_queue);
	if(!EventQ)
	{
		//没有等待的线程，先缓冲事件
		LIST_PUSH_BACK(e->buffering_event_queue,io);
		io = 0;
	}
	mutex_unlock(e->mtx);	
	if(io)
		BLOCK_QUEUE_PUSH(EventQ,io);
	*/
	//printf("putevent\n");
	spin_lock(e->mtx);
	MsgQueue_t msgQ = LIST_POP(MsgQueue_t,e->block_thread_queue);
	if(!msgQ)
	{
		//没有等待的线程，先缓冲事件
		LIST_PUSH_BACK(e->buffering_event_queue,io);
		io = 0;		
	}
	spin_unlock(e->mtx);
	if(io)
		PutMsg(msgQ,&io,sizeof(io));
	
	return 0;
}

void   stop_engine(engine_t e)
{
	assert(e);
/*	
	mutex_lock(e->mtx);
	e->status = 0;
	//强制唤醒所有等待在完成队列上的线程 
	struct block_queue *queue = 0;
	/*唤醒所有等待在完成队列的线程* /
	while(queue = LIST_POP(struct block_queue *,e->block_thread_queue))
	{
		BLOCK_QUEUE_FORCE_WAKEUP(queue);
	}	
	mutex_unlock(e->mtx);
*/
	spin_lock(e->mtx);
	e->status = 0;
	//强制唤醒所有等待在完成队列上的线程 
	MsgQueue_t msgQ = 0;
	/*唤醒所有等待在完成队列的线程*/
	while(msgQ = LIST_POP(MsgQueue_t,e->block_thread_queue))
	{
		//发送一个0消息
		void *tmp = 0;
		PutMsg(msgQ,&tmp,sizeof(tmp));
	}	
	spin_unlock(e->mtx);
}
