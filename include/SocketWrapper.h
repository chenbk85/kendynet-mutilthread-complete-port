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

/*
# Text File
# Author:   kenny
# File:     SocketWrapper.h 
# Created:  13:11:56 2008-10-27
# Modified: 13:12:06 2008-10-27
# Brief:    һЩ�׽ӿ�API�İ�װ����.     
*/

#ifndef _SOCK_WRAPPER_H
#define _SOCK_WRAPPER_H

#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */
#include	<sys/time.h>	/* timeval{} for select() */
#include	<time.h>		/* timespec{} for pselect() */
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>		/* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/uio.h>		/* for iovec{} and readv/writev */
#include	<unistd.h>
#include	<sys/wait.h>
#include	<sys/un.h>		/* for Unix domain sockets */
#include    <net/if.h>
#include    <sys/ioctl.h>
#include    <netinet/tcp.h>
#include    <fcntl.h>

#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression)\
   ({ long int __result;\
       do __result = (long int)(expression);\
       while(__result == -1L&& errno == EINTR);\
       __result;})
#endif

enum sock_family
{
	INET = AF_INET,
	INET6 = AF_INET6,
	LOCAL = AF_LOCAL,
	ROUTE = AF_ROUTE,
#ifdef _LINUX
	KEY = AF_KEY,
#endif
};

enum sock_type
{
	STREAM = SOCK_STREAM,//��Э��
	DGRAM  = SOCK_STREAM,//���ݱ�Э��
	SEQPACKET = SOCK_SEQPACKET,
	RAW = SOCK_RAW,//ԭʼ�׽ӿ�
};

enum sock_protocol
{
	TCP = IPPROTO_TCP,
	UDP = IPPROTO_UDP,
	SCTP = IPPROTO_SCTP,
};

//typedef sock_wrapper *socket_t;
#include "KendyNet.h"
HANDLE  OpenSocket(int family,int type,int protocol);

int  CloseSocket(HANDLE);

int Connect(HANDLE sock,const struct sockaddr *servaddr,socklen_t addrlen);

/*
 * brief: �����׽���,��������ĶԶ˽�������.
 * para:  
 *        ip:�Զ˵�IP��ַ
 *        port:�Զ˶˿�
 *        servaddr:
 *        retry: ���connectʧ���Ƿ����³���,ֱ�����ӳɹ��ŷ��� 
 * return: >0 �׽���,-1,ʧ��.
 */
HANDLE Tcp_Connect(const char *ip,unsigned short port,struct sockaddr_in *servaddr,int retry);

int Bind(HANDLE sock,const struct sockaddr *myaddr,socklen_t addrlen);

int Listen(HANDLE sock,int backlog);

/*
 * brief: �����׽���,����,Ȼ���ڴ��׽����ϼ���.
 *
 */
HANDLE Tcp_Listen(const char *ip,unsigned short port,struct sockaddr_in *servaddr,int backlog);


HANDLE Accept(HANDLE,struct sockaddr *sa,socklen_t *salen);

/*
 * brief: ��ȡԶ�����ӵ�IP,�˿ں�.
 */
int getRemoteAddrPort(HANDLE sock,char *buf,unsigned short *port);
int getLocalAddrPort(HANDLE sock,struct sockaddr_in *remoAddr,socklen_t *len,char *buf,unsigned short *port);

/*
ssize_t write_fd(int fd,void *ptr,size_t nbytes,int sendfd);

int create_un_execl(const char *path,const char *child);

ssize_t read_fd(int fd,void *ptr,size_t nbytes,int *recvfd);
*/

struct hostent *Gethostbyaddr(const char *ip,int family);

int setNonblock(HANDLE sock);

#endif



