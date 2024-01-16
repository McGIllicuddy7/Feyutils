#include "fey_hash_table.h"
#include "feylib_fstr.h"
#include "feylibcore.h"
#include "feyutils.h"
#include <stdint.h>
#include <stdio.h>
#include "fey_hash.h"
#include "fey_linked_list.h"

fey_hash_table_t create_fey_hash_table(fey_arena_t * arena, size_t size){
    fey_hash_table_t table;
    table.size = size;
    table.bools = fey_arena_alloc(arena, size);
    memset(table.bools, 0, size);
    table.objects = fey_arena_alloc(arena, size*sizeof(fey_hash_table_t));
    for(int i =0; i<size; i++){
        table.objects[i] = NULL;
    }
    table.arena = arena;
    table.exclusion = 0;
    return table;
}
void fey_hash_table_insert(fey_hash_table_t * table, fstr key,void * value){
    feylib_wait_arena_exclusion(&table->exclusion);
    uint64_t idx = hash_fstr(key);
    uint64_t idx0 = idx%table->size;
    //uint64_t density = hash_table_density(table);
    if(!table->bools[idx0]){
        fey_hash_object_t obj = (fey_hash_object_t){key, value};
        void * v = create_linked_list_node(table->arena, obj);
        table->objects[idx0] = v;
        table->bools[idx0] = true;
    }  
    else{
        fey_hash_object_t obj = (fey_hash_object_t){key, value};
        void * v = create_linked_list_node(table->arena, obj);
        ll_append(table->objects[idx0], v); 
    }
    feylib_release_arena_exclusion(&table->exclusion);
}
void * fey_hash_table_get(fey_hash_table_t * table, fstr key){
    uint64_t idx = hash_fstr(key);
    uint64_t idx0 = idx%table->size;
    if(!table->bools[idx0]){
        return NULL;
    }
    fey_linked_list_node_t * l = table->objects[idx0];
    while(l){
        fey_hash_object_t * obj = l->data;
        fstr key1 = obj->key;
        if(fstr_eq(key, key1)){
            return obj->data;
        }
        l = l->next;
    }
    return NULL;
}
