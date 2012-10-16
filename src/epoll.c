#include "epoll.h"
#include "Socket.h"
#include "SocketWrapper.h"
#include "HandleMgr.h"
#include <assert.h>

int  epoll_init(engine_t e)
{
	assert(e);
	e->poller_fd = TEMP_FAILURE_RETRY(epoll_create(MAX_SOCKET));
    return 	e->poller_fd >= 0 ? 0:-1; 
}

int epoll_register(engine_t e, socket_t s)
{
	assert(e);assert(s);
	int ret = -1;	
	struct epoll_event ev;	
	ev.data.ptr = s;
	ev.events = EV_IN | EV_OUT | EV_ET;
	TEMP_FAILURE_RETRY(ret = epoll_ctl(e->poller_fd,EPOLL_CTL_ADD,s->fd,&ev));
	return ret;
}


inline int epoll_unregister(engine_t e,socket_t s)
{
	assert(e);assert(s);
	struct epoll_event ev;int ret;
	TEMP_FAILURE_RETRY(ret = epoll_ctl(e->poller_fd,EPOLL_CTL_DEL,s->fd,&ev));
	return ret;
}
extern int total_bytes_recv;
#include "SysTime.h"
void epoll_loop(engine_t n)
{
	assert(n);
	struct epoll_event events[MAX_SOCKET];
	memset(events,0,sizeof(events));
	uint32_t tick = GetSystemMs();
	while(n->status)
	{
		int nfds = TEMP_FAILURE_RETRY(epoll_wait(n->poller_fd,events,MAX_SOCKET,100));
		if(nfds < 0)
		{
			printf("error:%d\n",errno);
			break;
		}
		int i;
		for(i = 0 ; i < nfds ; ++i)
		{	
			socket_t sock = (socket_t)events[i].data.ptr;
			if(sock)
			{
				//套接口可读
				if(events[i].events & EPOLLIN)
					on_read_active(sock);
				//套接口可写
				if(events[i].events & EPOLLOUT)
					on_write_active(sock);	
			
				if(events[i].events & EPOLLERR)
				{
					//套接口异常
				}
			}
		}
		
		
		uint32_t now = GetSystemMs();
		if(now - tick > 1000)
		{
			printf("recv:%dmb\n",total_bytes_recv/1024/1024);
			tick = now;
			total_bytes_recv = 0;
		}
	}
	close(n->poller_fd);
}
