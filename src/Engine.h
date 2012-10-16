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
	volatile int status; /*0:�ر�״̬,1:����״̬*/
	int poller_fd;
	
	//����¼���س�Ա
	struct link_list   *buffering_event_queue;	//��û���̵߳ȴ�ʱ���¼��������浽���������
	struct link_list   *block_thread_queue;    //���ڵȴ������Ϣ���߳�
	
	//thread_t     engine_thread;         //����engine���߳�
	
}*engine_t;

engine_t create_engine();
void   free_engine(engine_t *);
void   stop_engine(engine_t);


#endif
