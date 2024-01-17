#include "feyutils.h"
#include <stdatomic.h>
#include <stdint.h>
#include "fey_linked_list.h"
#include "fey_hash.h"
#define EnableHashTableType(T,hash_func, eq_func)\
typedef struct{\
	T key;\
	void * data;\
}T##_hash_object_t;\
typedef struct{\
	bool * bools;\
	fey_linked_list_node_t ** objects;\
	size_t size;\
	fey_arena_t * arena;\
	atomic_int exclusion;\
}T##_hash_table_t;\
static T##_hash_table_t T##_hash_table_create(fey_arena_t * arena, size_t size){\
    T##_hash_table_t table;\
    table.size = size;\
    table.bools = fey_arena_alloc(arena, size);\
    memset(table.bools, 0, size);\
    table.objects = fey_arena_alloc(arena, size*sizeof(T##_hash_table_t));\
    for(int i =0; i<size; i++){\
        table.objects[i] = NULL;\
    }\
    table.arena = arena;\
    table.exclusion = 0;\
    return table;\
}\
static void T##_hash_table_insert(T##_hash_table_t * table, T key,void * value){\
    if(!value){\
        return;\
    }\
    feylib_wait_arena_exclusion(&table->exclusion);\
    uint64_t idx = hash_func(key);\
    uint64_t idx0 = idx%table->size;\
    if(!table->bools[idx0]){\
        T##_hash_object_t obj = (T##_hash_object_t){key, value};\
        void * v = create_linked_list_node(table->arena, obj);\
        table->objects[idx0] = v;\
        table->bools[idx0] = true;\
    }  \
    else{\
        T##_hash_object_t obj = (T##_hash_object_t){key, value};\
        void * v = create_linked_list_node(table->arena, obj);\
        ll_append(table->objects[idx0], v); \
    }\
    feylib_release_arena_exclusion(&table->exclusion);\
}\
static void * T##_hash_table_get(T##_hash_table_t * table, T key){\
    uint64_t idx = hash_func(key);\
    uint64_t idx0 = idx%table->size;\
    if(!table->bools[idx0]){\
        return NULL;\
    }\
    fey_linked_list_node_t * l = table->objects[idx0];\
    while(l){\
        T##_hash_object_t * obj = l->data;\
        T key1 = obj->key;\
        if(eq_func(key, key1)){\
            return obj->data;\
        }\
        l = l->next;\
    }\
    return NULL;\
}