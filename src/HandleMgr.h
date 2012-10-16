#ifndef _HANDLEMGR_H
#define _HANDLEMGR_H

#include "KendyNet.h"
#include "Socket.h"
#include "Engine.h"


//typedef socket_wrapper *socket_t;
//typedef engine         *engine_t;


//定义系统支持的最大套接字和engine的数量
#define MAX_ENGINE 1
#define MAX_SOCKET 4096


extern int     InitHandleMgr();

inline extern socket_t GetSocketByHandle(HANDLE);
inline extern engine_t GetEngineByHandle(HANDLE);

extern HANDLE   NewEngine();
extern void     ReleaseEngine(HANDLE);

extern HANDLE   NewSocketWrapper();
extern int      ReleaseSocketWrapper(HANDLE);

#endif