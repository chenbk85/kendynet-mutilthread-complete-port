#ifndef _SOCKETWRAPPER_H
#define _SOCKETWRAPPER_H

#include "Engine.h"
#include "spinlock.h"
typedef struct socket_wrapper
{
	//mutex_t  mtx;//��֤ReleaseSocketWrapperֻ�ᱻ����ִ��һ��
	spinlock_t mtx;
	volatile int status;//0:δ����;1:����;
	engine_t  engine;
		
	volatile int readable;
	volatile int writeable;
	volatile int active_read_count;
	volatile int active_write_count;
	int fd;
	
	//������/�����޷��������ʱ,������Ͷ����������������,
	//���׽ӿڿɶ�/��дʱ���������
	struct link_list *pending_send;//��δ����ķ�����
	struct link_list *pending_recv;//��δ����Ķ�����
	
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
