#include "feyutils.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
static fey_arena_t global;
typedef char * string;
EnableArrayType(string);
static bool global_arena_set_up;
/*
typedef struct{
    void * ptr;
    size_t size;
}arena_chunk_t;
typedef struct{
    byte buffer[FEY_ARENA_SIZE];
    arena_chunk_t free_list[4096];
    size_t num_free;
    arena_chunk_t alloc_list[4096];
    size_t num_allocated;
} fey_arena_t;
*/
void fey_arena_hard_reset(fey_arena_t * arena){
    memset(arena->buffer, 0, arena->buffer_size-1);
    memset(arena->alloc_list, 0, arena->list_size*sizeof(arena_chunk_t));
    memset(arena->free_list, 0, arena->list_size*sizeof(arena_chunk_t));
    arena->num_free =1;
    arena->num_allocated = 0;
    arena->free_list[0] = (arena_chunk_t){arena->buffer, FEY_ARENA_SIZE};
}
void list_insert(arena_chunk_t * array, arena_chunk_t value, size_t * array_sz){
    int index = 0;
    for(int i =0; i<(*array_sz); i++){
        if(value.ptr<array[i].ptr){
            index = i;
            i = (*array_sz);
        }
    }
    for(int i =(*array_sz); i>=index; i--){
        array[i+1] = array[i]; 
    }
    array[index] = value;
    (*array_sz)++;
}
void list_remove(arena_chunk_t * array, void * pointer, size_t * array_sz){
    int index = -1;
    for(int i = 0; i<(*array_sz); i++){
        if(array[i].ptr == pointer){
            index = i;
            i = (*array_sz);
        }
    }
    if(index == -1){
        return;
    }
    for(int i = index+1; i<(*array_sz); i++){
        array[i-1] = array[i];
    }
    (*array_sz)--;
}
void * fey_arena_alloc(fey_arena_t * arena, size_t requested_size){
    if(arena == GLOBAL_ARENA){
        arena = &global;
        if(!global_arena_set_up){
            fey_arena_hard_reset(arena);
            global_arena_set_up = true;
        }
    }
    size_t size = requested_size;
    if(size<1){
        return NULL;
    }
    size = size/8+1;
    if(size<1){
        size = 8;
    }
    size = size*8;
    int index = -1;
    for(int i =0 ; i<arena->num_free; i++){
        if(arena->free_list[i].size>=size){
            index =i;
            i = arena->num_free;
        }
    }
    if(index == -1){
        return NULL;
    }
    void * ptr = arena->free_list[index].ptr;
    size_t new_sz = arena->free_list[index].size-size;
    list_insert(arena->alloc_list, (arena_chunk_t){ptr, size},&arena->num_allocated);
    list_remove(arena->free_list, ptr, &arena->num_free);
    if(new_sz>0){
        list_insert(arena->free_list, (arena_chunk_t){ptr+size, new_sz},&arena->num_free);
    }
    return ptr;
}
void fey_arena_free(fey_arena_t *arena, void * ptr){
    if(arena == GLOBAL_ARENA){
        arena = &global;
        if(!global_arena_set_up){
            fey_arena_hard_reset(arena);
            global_arena_set_up = true;
        }
    }
    int index = -1;
    for(int i = 0; i<arena->num_allocated; i++){
        if(arena->alloc_list[i].ptr == ptr){
            index = i;
            i = arena->num_allocated;
        }
    }
    if(index == -1){
        printf("\n failed\n");
        return;
    }
    list_insert(arena->free_list, arena->alloc_list[index], &arena->num_free);
    list_remove(arena->alloc_list, ptr, &arena->num_allocated);
merged:
    for(int i = 0;i<arena->num_free-1; i++){
        if(arena->free_list[i].ptr+arena->free_list[i].size == arena->free_list[i+1].ptr){
            arena->free_list[i].size+= arena->free_list[i+1].size;
            list_remove(arena->free_list, arena->free_list[i+1].ptr, &arena->num_free);
            goto merged;
        }
    }
}
void * fey_arena_realloc(fey_arena_t * arena, void * ptr, size_t requested_size){
    if(arena == GLOBAL_ARENA){
        arena = &global;
        if(!global_arena_set_up){
            fey_arena_hard_reset(arena);
            global_arena_set_up = true;
        }
    }
    int index = -1;
    for(int i = 0; i<arena->num_allocated; i++){
        if(arena->alloc_list[i].ptr == ptr){
            index = i;
            i = arena->num_allocated;
        }
    }
    if(index == -1){
        return NULL;
    }
    char * out = fey_arena_alloc(arena, requested_size);
    char * optr = arena->alloc_list[index].ptr;
    int sz = arena->alloc_list[index].size;
    if(sz>requested_size){
        sz = requested_size;
    }
    for(int i = 0; i<sz; i++){
        out[i] = optr[i];
    }
    fey_arena_free(arena, ptr);
    return out;
}
void fey_arena_debug(fey_arena_t * arena){
    if(arena == GLOBAL_ARENA){
        arena = &global;
        if(!global_arena_set_up){
            fey_arena_hard_reset(arena);
            global_arena_set_up = true;
        }
    }
    for(int i = 0; i<arena->num_free; i++){
        printf("{ptr: %p, size: %lu, free}", arena->free_list[i].ptr, arena->free_list[i].size);
    }
    for(int i = 0; i<arena->num_allocated; i++){
       printf("{ptr: %p, size: %lu, allocated}", arena->alloc_list[i].ptr, arena->alloc_list[i].size);
    }
    printf("\n");
}
/*
typedef struct{
    const char * data;
    size_t len;
    size_t alloc_len;
}fstr;
*/
fstr subfstr(char * v, int start, int end, fey_arena_t * arena){
    fey_init_small_arena();
	char * out = fey_arena_alloc(local, end-start);
	for(int i = start; i<end; i++){
		out[i-start] = v[i];
	}
	return fstr_fromStr(arena, out);
}
fstr fstr_fromStr(fey_arena_t * arena, char * c){
    fstr out;
    out.arena = arena;
    out.len = strlen(c)+1;
    int actlen = ceil(log2(out.len));
    out.alloc_len = 1;
    for(int i =0 ; i<actlen; i++){
        out.alloc_len *= 2;
    }
    out.data = fey_arena_alloc(arena,out.alloc_len);
    for(int i = 0; i<out.len; i++){
        out.data[i] = c[i];
    }
    return out;
}
void fstr_push(fstr * str, char c){
    fey_arena_t * arena = str->arena;
    if(str->alloc_len<1){
        str->data = fey_arena_alloc(arena, 8);
        str->data[0] = c;
        str->len = 2;
        str->alloc_len = 8;
        return;
    }
    if(str->alloc_len>str->len+1){
        str->data[str->len-1] = c;
        str->data[str->len] = '\0';
        str->len++;
    }
    else{
        str->alloc_len *= 2;
        char * buff = fey_arena_alloc(arena, str->alloc_len);
        assert(buff != NULL);
        for(int i = 0; i<str->len-1; i++){
            buff[i] = str->data[i];
        }
        buff[str->len-1] = c;
        buff[str->len] = '\0';
        str->len++;
        //fey_arena_free(arena,str->data);s
        str->data = buff;
    }
}
fstr fstr_add(fey_arena_t * arena,fstr a, fstr b){
    fstr out = {arena,0,0,0};
    for(int i = 0; i<a.len-1; i++){
        fstr_push(&out, a.data[i]);
    }
    for(int i = 0; i<b.len-1; i++){
        fstr_push(&out, b.data[i]);
    }
    return out;
}
bool fstr_eq(fstr a, fstr b){
    if(a.len != b.len){
        return false;
    }
    for(int i =0 ; i<a.len; i++){
        if(a.data[i] != b.data[i]){
            return false;
        }
    }
    return true;
}


void fstr_cat(fstr * a, char * b){
    int l = strlen(b);
    for(int i =0 ; i<l; i++){
        fstr_push(a, b[i]);
    }
}
void fstr_print(fstr str){
    printf("%s", str.data);
}
void fstr_println(fstr str){
    printf("%s\n",str.data);
}
fstr fstr_new(fey_arena_t * arena){
    fstr out;
    out.arena = arena;
    out.len = 1;
    out.data = fey_arena_alloc(arena,8);
    out.alloc_len = 8;
    out.data[0] = '\0';
    return out;
}
void fstr_delete(fstr str){
    fey_arena_t * arena = str.arena;
    fey_arena_free(arena, str.data);
    str.data = 0;
    str.alloc_len = 0;
    str.len = 0;
}