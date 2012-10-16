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
#ifndef _MSGQUEUE_H
#define _MSGQUEUE_H
//一个消息队列,unix下使用管道实现

typedef struct MsgQueue *MsgQueue_t;


extern MsgQueue_t CreateMsgQ();
extern void       DestroyMsgQ(MsgQueue_t*);
extern void       PutMsg(MsgQueue_t,void*,int msgLen);
extern int        GetMsg(MsgQueue_t,void*,int msgLen,int timeout);

#endif