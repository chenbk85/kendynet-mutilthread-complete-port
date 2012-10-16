#ifndef _LINK_LIST_H
#define _LINK_LIST_H


#include "sync.h"
#include "KendyNet.h"
//typedef struct list_node list_node;

struct link_list;
struct block_queue;


void list_push_back(struct link_list*,list_node*);

void list_push_front(struct link_list*,list_node*);

list_node* list_pop(struct link_list*);

int list_is_empty(struct link_list*);

struct link_list *create_list();

void destroy_list(struct link_list**);

void list_clear(struct link_list*);

#define LIST_PUSH_FRONT(L,N) list_push_front(L,(list_node*)N)

#define LIST_PUSH_BACK(L,N) list_push_back(L,(list_node*)N)

#define LIST_POP(T,L) (T)list_pop(L)

#define LIST_IS_EMPTY(L) list_is_empty(L)

#define LIST_CREATE() create_list()

#define LIST_DESTROY(L) destroy_list(L)

#define LIST_CLEAR(L) list_clear(L)


struct block_queue* create_block_queue();
void destroy_block_queue(struct block_queue**);
void block_queue_push(struct block_queue*,list_node*);
enum
{
	POP_SUCCESS = 0,
	POP_TIMEOUT = 1,
	POP_FORCE_WAKE_UP = 2,
};
int block_queue_pop(struct block_queue*,list_node**,int timeout);
void block_queue_force_wakeup(struct block_queue*);
void block_queue_clear(struct block_queue*);

#define BLOCK_QUEUE_PUSH(L,N) block_queue_push(L,(list_node*)N)

#define BLOCK_QUEUE_POP(L,N,M) block_queue_pop(L,(list_node**)N,M)

#define BLOCK_QUEUE_CREATE() create_block_queue()

#define BLOCK_QUEUE_DESTROY(L) destroy_block_queue(L)

#define BLOCK_QUEUE_CLEAR(L)  block_queue_clear(L)

#define BLOCK_QUEUE_FORCE_WAKEUP(L) block_queue_force_wakeup(L)


#endif
