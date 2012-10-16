#include "HandleMgr.h"
#include "sync.h"

static mutex_t  engine_mtx;
static engine_t engine_pool[MAX_ENGINE];
static int current_engine_count = 0;

static mutex_t  socket_mtx;
static socket_t socket_pool[MAX_SOCKET];
static int current_socket_count = 0;

int InitHandleMgr()
{	
	engine_mtx = mutex_create();
	socket_mtx = mutex_create();
	return 0;
	
}

inline socket_t GetSocketByHandle(HANDLE handle)
{
	if(handle >= 0 && handle < current_socket_count && socket_pool[handle]->status != 0)
		return socket_pool[handle];
	return 0;
}

inline engine_t GetEngineByHandle(HANDLE handle)
{
	if(handle >= 0 && handle < current_engine_count && engine_pool[handle]->status != 0)
		return engine_pool[handle];
	return 0;	
}

HANDLE	NewSocketWrapper()
{
	mutex_lock(socket_mtx);
	int i = 0;
	int cur_socket_count = current_socket_count;
	//���Ȳ鿴�Ѵ�����socket���Ƿ��п��õ�
	for( ; i < current_socket_count; ++i)
	{
		if(socket_pool[i]->status == 0)
		{
			socket_pool[i]->status = 1;
			break;
		}
	}
	if(i == current_socket_count && current_socket_count < MAX_SOCKET)
	{
		//��û��������,�²���һ��socket
		socket_pool[current_socket_count] = create_socket();
		if(socket_pool[current_socket_count])
		{
			socket_pool[current_socket_count]->status = 1;
			cur_socket_count = ++current_socket_count;
		}
	}
	mutex_unlock(socket_mtx);
	if(i < cur_socket_count)
		return i;
	return -1;
}


inline static int RemoveBinding(engine_t e, socket_t sock)
{
	return e ? e->UnRegister(e,sock) : -1;
}


int  ReleaseSocketWrapper(HANDLE handle)
{
	int ret = -1;
	mutex_lock(socket_mtx);
	if(handle >= 0 && handle < current_socket_count)
	{
		spin_lock(socket_pool[handle]->mtx);
		if(socket_pool[handle]->status != 0)
		{
			RemoveBinding(socket_pool[handle]->engine,socket_pool[handle]);
			close(socket_pool[handle]->fd);
			socket_pool[handle]->status = 0;
			ret = 0;			
		}
		spin_unlock(socket_pool[handle]->mtx);	
	}
	mutex_unlock(socket_mtx);
	return ret;
} 

HANDLE	NewEngine()
{
	mutex_lock(engine_mtx);
	int i = 0;
	int cur_engine_count = current_engine_count;
	for( ; i < current_engine_count; ++i)
	{
		if(engine_pool[i]->status == 0)
		{
			engine_pool[i]->status = 1;
			break;
		}
	}
	
	if(i == current_engine_count && current_engine_count < MAX_ENGINE)
	{
		//��û��������,�²���һ��engine
		engine_pool[current_engine_count] = create_engine();
		if(engine_pool[current_engine_count])
		{
			engine_pool[current_engine_count]->status = 1;
			cur_engine_count = ++current_engine_count;
		}
	}
    	
	mutex_unlock(engine_mtx);
	
	if(i < cur_engine_count)
		return i;
	return -1;
}

void  ReleaseEngine(HANDLE handle)
{
	mutex_lock(engine_mtx);
	if(handle >= 0 && handle < current_engine_count && engine_pool[handle]->status != 0)
	{
		stop_engine(engine_pool[handle]);
	}
	mutex_unlock(engine_mtx);
} 
