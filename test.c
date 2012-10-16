#include <stdio.h>
#include <stdlib.h>
#include "KendyNet.h"
#include "thread.h"
#include "SocketWrapper.h"
#include "atomic.h"
#include "SysTime.h"
enum
{
	RECV_FINISH = 0,
	RECV_REQUEST,
	RECV = RECV_REQUEST,
	SEND_FINISH,
	SEND = SEND_FINISH,
};

typedef struct IoContext
{
	st_io m_ioStruct;
	unsigned char    m_opType;
	void             *ud;
}IoContext;

typedef struct _Socket
{
	char send_buf[65536];
	char recv_buf[65536];
	struct iovec recv_iovec;
	struct iovec send_iovec;
	IoContext m_IORecvComp;
    IoContext m_IOSendComp;
	HANDLE    m_sock;
}_Socket;

HANDLE engine;
const char *ip;
long port;

void *ListerRoutine(void *arg)
{
	//thread_t thread = (thread)arg//(thread_t)CUSTOM_ARG(arg);
	struct sockaddr_in servaddr;
	HANDLE listerfd;
	if((listerfd = Tcp_Listen(ip,port,&servaddr,5)) == 0)
	{
		while(/*!is_terminate(thread)*/1)
		{
			struct sockaddr sa;
			socklen_t salen;
			HANDLE sock = Accept(listerfd,&sa,&salen);
			if(sock >= 0)
			{
				setNonblock(sock);
				_Socket *_sock = malloc(sizeof(*_sock));
				_sock->m_sock = sock;
				_sock->m_IORecvComp.m_opType = RECV_REQUEST;
				_sock->m_IOSendComp.m_opType = SEND_FINISH;
				_sock->m_IORecvComp.ud = _sock->m_IOSendComp.ud = _sock;
				_sock->recv_iovec.iov_base = _sock->recv_buf;
				_sock->send_iovec.iov_base = _sock->send_buf;
				_sock->m_IORecvComp.m_ioStruct.iovec = &_sock->recv_iovec;
				_sock->m_IOSendComp.m_ioStruct.iovec = &_sock->send_iovec;
				_sock->m_IOSendComp.m_ioStruct.iovec_count = _sock->m_IORecvComp.m_ioStruct.iovec_count = 1;
				//printf("接到一个连接\n");
				if(0 != Bind2Engine(engine,sock))
				{
					printf("bind出错\n");
					CloseSocket(sock);
					free(_sock);
				}
				else
				{
					//发起第一个读请求
					_sock->m_IORecvComp.m_ioStruct.iovec->iov_len = 65536;
					//请recv请求投递到队列中,由完成线程负责处理
					PutQueueEvent(engine,(st_io*)&(_sock->m_IORecvComp));
				}
			}
			else
			{
				printf("accept出错\n");
			}
		}
		printf("listener 终止\n");
	}
	return 0;
}

#define SEND while(byteTransfer > 0)\
{\
	sock->m_IOSendComp.m_ioStruct.iovec->iov_len = byteTransfer;\
	int ret = WSASend(sock->m_sock,(st_io*)&(sock->m_IOSendComp));\
	if(ret > 0)\
	{\
		byteTransfer -= ret;\
		if(byteTransfer == 0)\
			break;\
		sock->m_IOSendComp.m_ioStruct.iovec->iov_len = byteTransfer;\
	}else if(ret == 0)\
	{\
		byteTransfer = 0;\
		break;\
	}\
	else\
	{\
		byteTransfer = -1;\
		break;\
	}\
}

int total_bytes_recv = 0;

void *IORoutine(void *arg)
{
	st_io* ioComp;
	//struct block_queue *EventQ = CreateEventQ();
	MsgQueue_t EventQ = CreateMsgQ();
	while(1)
	{
		ioComp = 0;
		if(0 > GetQueueEvent(engine,EventQ,&ioComp,-1))
		{
			printf("poller终止\n");
			break;
		}
		if(ioComp)
		{
			/*if(ioComp->bytes_transfer <= 0)
			{
				_Socket *sock = (_Socket*)(((IoContext*)ioComp)->ud);
				if(CloseSocket(sock->m_sock) == 0)
					free(sock);
			}
			else
			{*/
				IoContext *context = (IoContext*)ioComp;
				_Socket *sock = (_Socket*)context->ud;
				if(context->m_opType <= RECV)
				{
					if(context->m_opType == RECV_FINISH)
					{
						if(ioComp->bytes_transfer <= 0)
						{
							_Socket *sock = (_Socket*)(((IoContext*)ioComp)->ud);
							if(CloseSocket(sock->m_sock) == 0)
								free(sock);
							continue;
						}
						//一个读完成通告
						total_bytes_recv += ioComp->bytes_transfer;
					}
					//一直读,直到IO_PENDING
					int byteTransfer = 0;
					//继续读
					context->m_opType = RECV_FINISH;
					while((byteTransfer = WSARecv(sock->m_sock,(st_io*)context)) > 0)
					{
						total_bytes_recv += byteTransfer;
						if(byteTransfer < 0)
						{
							printf("close\n");
							break;
						}
					}

					if(byteTransfer < 0)
					{
						//printf("套接口断开\n");
						if(CloseSocket(sock->m_sock))
							free(sock);
					}					
				}
			//}
		}
	}
	printf("IO end\n");
	DestroyMsgQ(&EventQ);
	return 0;
}

void *EngineRoutine(void *arg)
{
	EngineRun(engine);
	printf("Engine stop\n");
	return 0;
}

int main(int argc,char **argv)
{
	ip = argv[1];
	port = atoi(argv[2]);
	signal(SIGPIPE,SIG_IGN);
	if(InitNetSystem() != 0)
	{
		printf("Init error\n");
		return 0;
	}
	
	engine = CreateEngine();
	thread_t listener = create_thread(0);
	start_run(listener,ListerRoutine,listener);
	
	int complete_count = atoi(argv[3]);
	int i = 0;
	for( ; i < complete_count; ++i)
	{
		thread_t complete_t = create_thread(0);
		start_run(complete_t,IORoutine,0);
	}
	thread_t engine_thread = create_thread(1);
	start_run(engine_thread,EngineRoutine,0);
	getchar();
	CloseEngine(engine);
	join(engine_thread);
	return 0;
}
