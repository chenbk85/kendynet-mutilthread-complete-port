/*	
    Copyright (C) <2012>  <huangweilook@21cn.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/	
#ifndef _KENDYNET_H
#define _KENDYNET_H
#include "MsgQueue.h"
typedef struct list_node
{
	struct list_node *next;
}list_node;

#define LIST_NODE list_node node;

/*IO�������ɶ���ʹ�õĽṹ*/
typedef struct
{
	LIST_NODE;
	struct iovec *iovec;
	int    iovec_count;
	int    bytes_transfer;
	int    error_code;
}st_io;

//��ʼ������ϵͳ
int      InitNetSystem();

typedef int HANDLE;
struct block_queue;

struct block_queue* CreateEventQ();
void DestroyEventQ(struct block_queue**);

HANDLE   CreateEngine();
void     CloseEngine(HANDLE);
int      EngineRun(HANDLE);

int     Bind2Engine(HANDLE,HANDLE);

//��ȡ��Ͷ���¼���engine�Ķ�����
//int      GetQueueEvent(HANDLE,struct block_queue*,st_io **,int timeout);
int      GetQueueEvent(HANDLE,MsgQueue_t,st_io **,int timeout);
int      PutQueueEvent(HANDLE,st_io *);

int WSASend(HANDLE,st_io*);
int WSARecv(HANDLE,st_io*);

#endif