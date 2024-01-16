#pragma once
#include "feylibcore.h"
//smh
//owns it's memory so if the list is owned the memory in it is freed as wel
//none of this is threadsafe have fun :)l
typedef struct{
	void * prev;
	void * next;
	void * data;
	fey_arena_t * arena;
}fey_linked_list_node_t;
fey_linked_list_node_t * ll_get_end(fey_linked_list_node_t *list);
fey_linked_list_node_t * ll_get_start(fey_linked_list_node_t * list);
void ll_append(fey_linked_list_node_t *list, fey_linked_list_node_t * value);
void ll_pop(fey_linked_list_node_t *list);
void ll_destroy(fey_linked_list_node_t *list);
fey_linked_list_node_t * func_create_linked_list_node(fey_arena_t * arena, void * data, size_t data_size);
#define create_linked_list_node(arena, T) func_create_linked_list_node(arena, &T, sizeof(T)) 
