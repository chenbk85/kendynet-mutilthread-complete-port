#ifndef _EPOLL_H
#define _EPOLL_H
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <error.h>


#define EV_IN  EPOLLIN
#define EV_OUT EPOLLOUT
#define EV_ERR EPOLLERR
#define EV_ET  EPOLLET

#include "Engine.h"
#include "KendyNet.h"
#include "Socket.h"
int  epoll_init(engine_t);
void epoll_loop(engine_t);
int  epoll_register(engine_t,socket_t);
int  epoll_unregister(engine_t,socket_t);
#endif
