#include "feyutils.h"
#include <stdatomic.h>
#include <stdint.h>
#include "fey_linked_list.h"
typedef struct{
	fstr key;
	void * data;
}fey_hash_object_t;
typedef struct{
	bool * bools;
	fey_linked_list_node_t ** objects;
	size_t size;
	fey_arena_t * arena;
	atomic_int exclusion;
}fey_hash_table_t;
fey_hash_table_t create_fey_hash_table(fey_arena_t * arena, size_t size);
void fey_hash_table_insert(fey_hash_table_t * table, fstr key,void * value);
void * fey_hash_table_get(fey_hash_table_t *table, fstr key);
