#ifndef _ENGINE_H
#define _ENGINE_H

#include "sync.h"
//#include "thread.h"
#include "link_list.h"
#include "spinlock.h"

struct socket_wrapper;
typedef struct engine
{
	int  (*Init)(struct engine*);
	void (*Loop)(struct engine*);
	int  (*Register)(struct engine*,struct socket_wrapper*);
	int  (*UnRegister)(struct engine*,struct socket_wrapper*);
	
	spinlock_t mtx;
	volatile int status; /*0:关闭状态,1:开启状态*/
	int poller_fd;
	
	//完成事件相关成员
	struct link_list   *buffering_event_queue;	//当没有线程等待时，事件将被缓存到这个队列中
	struct link_list   *block_thread_queue;    //正在等待完成消息的线程
	
	//thread_t     engine_thread;         //运行engine的线程
	
}*engine_t;

engine_t create_engine();
void   free_engine(engine_t *);
void   stop_engine(engine_t);


#endif
