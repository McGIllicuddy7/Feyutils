#include "fey_linked_list.h"
#include "feylibcore.h"
#include <string.h>
fey_linked_list_node_t * ll_get_end(fey_linked_list_node_t * list){
    while(list->next){
        list = list->next;
    }
    return list;
}
fey_linked_list_node_t * ll_get_start(fey_linked_list_node_t * list){
    while(list->prev){
        list = list->prev;
    }
    return list;
}
void ll_append(fey_linked_list_node_t * list, fey_linked_list_node_t *value){
    fey_linked_list_node_t * end = ll_get_end(list);
    end->next = value;
    value->prev = end;
    value->next = 0;
}
void ll_pop(fey_linked_list_node_t * list){
    while(list->next){
        list = list->next;
    }
    if(list->prev){
        fey_linked_list_node_t * prev = list->prev;
        prev->next = 0;
    }
    fey_arena_free(list->arena, list->data);
    fey_arena_free(list->arena, list);
}
static void ll_destroy_forward(fey_linked_list_node_t * list){
    if(list->next){
        ll_destroy_forward(list->next);
    }
    fey_arena_free(list->arena, list->data);
    fey_arena_free(list->arena, list);
}
static void ll_destroy_backward(fey_linked_list_node_t * list){
    if(list->prev){
        ll_destroy_backward(list->prev);
    }
    fey_arena_free(list->arena, list->data);
    fey_arena_free(list->arena, list);
}
void ll_destroy(fey_linked_list_node_t * list){
    if(list->next){
        ll_destroy_forward((list->next));
    }
    if(list->prev){
        ll_destroy_backward((list->prev));
    }
    fey_arena_free(list->arena, list->data);
    fey_arena_free(list->arena, list);
}
fey_linked_list_node_t * func_create_linked_list_node(fey_arena_t * arena, void * data, size_t data_size){
    fey_linked_list_node_t * out = fey_arena_alloc(arena, sizeof(fey_linked_list_node_t));
    void * out_data = fey_arena_alloc(arena, data_size);
    memcpy(out_data, data, data_size);
    out->data = out_data;
    out->next = NULL;
    out->prev = NULL;
    out->arena = arena;
    return out;
}
