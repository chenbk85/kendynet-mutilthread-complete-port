#include "KendyNet.h"
#include "Engine.h"
#include "Socket.h"
#include "link_list.h"
#include "HandleMgr.h"
#include <assert.h>
#include "MsgQueue.h"

int InitNetSystem()
{
	return InitHandleMgr();
}

int EngineRun(HANDLE engine)
{
	engine_t e = GetEngineByHandle(engine);
	if(!e)
		return -1;
	e->Loop(e);	
	return 0;
}

HANDLE CreateEngine()
{
	HANDLE engine = NewEngine();
	if(engine >= 0)
	{
		engine_t e = GetEngineByHandle(engine);
		LIST_CLEAR(e->buffering_event_queue);
		LIST_CLEAR(e->block_thread_queue);
		if(0 != e->Init(e))
		{
			CloseEngine(engine);
			engine = -1;
		}
	}
	return engine;
}

void CloseEngine(HANDLE handle)
{
	ReleaseEngine(handle);
}

int Bind2Engine(HANDLE e,HANDLE s)
{
	engine_t engine = GetEngineByHandle(e);
	socket_t sock   = GetSocketByHandle(s);
	if(!engine || ! sock)
		return -1;
	if(engine->Register(engine,sock) == 0)
	{
		sock->engine = engine;
		return 0;
	}
	return -1;
}

int	GetQueueEvent(HANDLE handle, MsgQueue_t EventQ,st_io **io ,int timeout)
{
	assert(EventQ);assert(io);

	engine_t e = GetEngineByHandle(handle);
	if(!e)
		return -1;
	spin_lock(e->mtx);
	if(e->status == 0)
	{
		spin_unlock(e->mtx);
		return -1;
	}
	
	if(*io = LIST_POP(st_io*,e->buffering_event_queue))
	{
		spin_unlock(e->mtx);
		return 0;
	}
	//插入到等待队列中，阻塞
	LIST_PUSH_FRONT(e->block_thread_queue,EventQ);
	spin_unlock(e->mtx);

	GetMsg(EventQ,io,sizeof(*io),0);
	if(*io == 0)
		return -1;
	
	return 0;	
}

int	PutQueueEvent(HANDLE handle,st_io *io)
{
	assert(io);
	engine_t e = GetEngineByHandle(handle);
	if(!e)
		return -1;
	
	return put_event(e,io);
}

extern int put_event(engine_t e,st_io *io);

int WSASend(HANDLE sock,st_io *io)
{
	assert(io);
	socket_t s = GetSocketByHandle(sock);
	if(!s)
		return -1;
	
	int active_send_count = -1;
	int ret = 0;
	
	spin_lock(s->send_mtx);
	//为保证执行顺序与请求的顺序一致,先将请求插入队列尾部,再弹出队列首元素
	LIST_PUSH_BACK(s->pending_send,io);
	io = 0;
	if(s->writeable)
	{
		io = LIST_POP(st_io*,s->pending_send);
		active_send_count = s->active_write_count;
	}
	spin_unlock(s->send_mtx);
	if(io)
		ret =  _send(s,io,active_send_count,SLIENT_ON_COMPLETE);
	return ret;
}

int WSARecv(HANDLE sock,st_io *io)
{
	assert(io);
	socket_t s = GetSocketByHandle(sock);
	if(!s)
		return -1;

	int active_recv_count = -1;
	int ret = 0;
	
	spin_lock(s->recv_mtx);
	//为保证执行顺序与请求的顺序一致,先将请求插入队列尾部,再弹出队列首元素
	LIST_PUSH_BACK(s->pending_recv,io);
	io  = 0;
	if(s->readable)
	{
		io = LIST_POP(st_io*,s->pending_recv);
		active_recv_count = s->active_read_count;
	}
	spin_unlock(s->recv_mtx);
	
	if(io)
		ret =  _recv(s,io,active_recv_count,SLIENT_ON_COMPLETE);
	return ret;
}





