#ifndef _SOCKETWRAPPER_H
#define _SOCKETWRAPPER_H

#include "Engine.h"
#include "spinlock.h"
typedef struct socket_wrapper
{
	//mutex_t  mtx;//保证ReleaseSocketWrapper只会被正常执行一次
	spinlock_t mtx;
	volatile int status;//0:未开启;1:正常;
	engine_t  engine;
		
	volatile int readable;
	volatile int writeable;
	volatile int active_read_count;
	volatile int active_write_count;
	int fd;
	
	//当发送/接收无法立即完成时,把请求投入下面两个队列中,
	//当套接口可读/可写时再完成请求
	struct link_list *pending_send;//尚未处理的发请求
	struct link_list *pending_recv;//尚未处理的读请求
	
	spinlock_t   recv_mtx;
	spinlock_t   send_mtx;
	
}*socket_t;


void on_read_active(socket_t);
void on_write_active(socket_t);
socket_t create_socket();
void free_socket(socket_t*);

enum
{
	NOTIFY_ON_COMPLETE = 1,
	SLIENT_ON_COMPLETE = 0,
};

int _recv(socket_t,st_io*,int,int notify);
int _send(socket_t,st_io*,int,int notify);


#endif
