#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "SocketWrapper.h"
#include "MsgQueue.h"
#include "epoll.h"
#include "KendyNet.h"
#include <errno.h>

struct MsgQueue
{
	LIST_NODE;
	int  p[2];
	int  epollfd;
};

MsgQueue_t CreateMsgQ()
{
	MsgQueue_t q = malloc(sizeof(*q));
	q->p[1] = q->p[0]=0;
	if(pipe(q->p) == 0)
	{
		/*q->epollfd = TEMP_FAILURE_RETRY(epoll_create(1));
		struct epoll_event ev;	
		ev.data.ptr = q->pipe_for_read;
		ev.events = EV_IN;
		TEMP_FAILURE_RETRY(epoll_ctl(q->epollfd,EPOLL_CTL_ADD,(*q)->pipe_for_read,&ev));		
		*/
		return q;
	}
	free(q);
	return 0;
}

void       DestroyMsgQ(MsgQueue_t *q)
{
	close((*q)->p[0]);
	close((*q)->p[1]);
	//close((*q)->epollfd);
	free(*q);
	*q = 0;
}

void       PutMsg(MsgQueue_t q,void *msg,int msgLen)
{
	int len = msgLen;
	while(len > 0)
	{
		int ret = TEMP_FAILURE_RETRY(write(q->p[1],msg,len));
		if(ret > 0)
		{
			len -= ret;
			msg += ret;
		}
		else
		{
			printf("PutMsg error:%d\n",errno);
		}
	}
	//printf("put finish\n");
}

int  GetMsg(MsgQueue_t q,void *msgout,int msgLen,int timeout)
{
	int len = msgLen;

	while(len > 0)
	{
		int ret = TEMP_FAILURE_RETRY(read(q->p[0],msgout,len));
		if(ret > 0)
		{
			len -= ret;
			//msgout += ret;
		}
		else
		{
			printf("GetMsg error:%d\n",errno);
			return -1;
		}
	}
	return 0;

}