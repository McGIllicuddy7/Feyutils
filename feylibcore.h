#pragma once
#include <stdlib.h>
#include <stdatomic.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#define GLOBAL_ARENA 0
typedef unsigned char byte;
//the struct for handling memory allocation in arenas, you do not have to worry about this just please don't mess with it.
typedef struct{
    void * ptr;
    size_t size;
}arena_chunk_t;
//the struct for arena memory allocators, all memory allocated using an arena is automatically freed when the arena goes out of scope. you can manually request memory up to 1,280,000 bytes by using fey_arena_alloc and passing the arena as the a parameter
typedef struct{
    byte* buffer;
    size_t buffer_size;
    arena_chunk_t *free_list;
    size_t num_free;
    arena_chunk_t* alloc_list;
    size_t num_allocated;
    size_t list_size;
    volatile atomic_int access_flag;
} fey_arena_t;
#define SMALL_ARENA_SIZE 4096
#define SMALL_ARENA_LIST_SIZE 64
#define MEDIUM_ARENA_SIZE 65536
#define MEDIUM_ARENA_LIST_SIZE 2048
#define LARGE_ARENA_SIZE 1048576
#define LARGE_ARENA_LIST_SIZE 8192
//hard resets an arena, sets every bit in it to zero, and then resets all memory allocation done by that arena. the setting every bit to zero is for your convenience as it should become immediately obvious when this happens that something is wrong
void fey_arena_hard_reset(fey_arena_t * arena);
/*initializes an arena and creates a reference to it called local, pass the arena to functions that need to allocate memory by using "local" as the first parameter, if you want an arena to be passed as a parameter to a function the convention is 
"fey_arena_t * arena" as the first parameter. any function that calls for a fey_arena_t pointer will allocate or deallocate memory and that memory will be gone when the stack frame the arena is in is closed*/
#define fey_init_small_arena()\
    byte local_arena_buffer[SMALL_ARENA_SIZE];\
    arena_chunk_t local_arena_free_list[SMALL_ARENA_LIST_SIZE];\
    arena_chunk_t local_arena_alloc_list[SMALL_ARENA_LIST_SIZE];\
    fey_arena_t local_arena;\
    local_arena.buffer = local_arena_buffer;\
    local_arena.alloc_list = local_arena_alloc_list;\
    local_arena.free_list = local_arena_free_list;\
    local_arena.buffer_size = SMALL_ARENA_SIZE;\
    local_arena.list_size = SMALL_ARENA_LIST_SIZE;\
    local_arena.access_flag = 0;\
    fey_arena_t * local = &local_arena;\
    fey_arena_hard_reset(local);
#define fey_init_medium_arena()\
    byte local_arena_buffer[MEDIUM_ARENA_SIZE];\
    arena_chunk_t local_arena_free_list[MEDIUM_ARENA_LIST_SIZE];\
    arena_chunk_t local_arena_alloc_list[MEDIUM_ARENA_LIST_SIZE];\
    fey_arena_t local_arena;\
    local_arena.buffer = local_arena_buffer;\
    local_arena.alloc_list = local_arena_alloc_list;\
    local_arena.free_list = local_arena_free_list;\
    local_arena.buffer_size = MEDIUM_ARENA_SIZE;\
    local_arena.list_size = MEDIUM_ARENA_LIST_SIZE;\
    local_arena.access_flag = 0;\
    fey_arena_t * local = &local_arena;\
    fey_arena_hard_reset(local);
#define fey_init_large_arena()\
    byte local_arena_buffer[LARGE_ARENA_SIZE];\
    arena_chunk_t local_arena_free_list[LARGE_ARENA_LIST_SIZE];\
    arena_chunk_t local_arena_alloc_list[LARGE_ARENA_LIST_SIZE];\
    fey_arena_t local_arena;\
    local_arena.buffer = local_arena_buffer;\
    local_arena.alloc_list = local_arena_alloc_list;\
    local_arena.free_list = local_arena_free_list;\
    local_arena.buffer_size = LARGE_ARENA_SIZE;\
    local_arena.list_size = LARGE_ARENA_LIST_SIZE;\
    local_arena.access_flag = 0;\
    fey_arena_t * local = &local_arena;\
    fey_arena_hard_reset(local);
#define create_named_arena(name)\
    byte name##_arena_buffer[LARGE_ARENA_SIZE];\
    arena_chunk_t name##_arena_free_list[LARGE_ARENA_LIST_SIZE];\
    arena_chunk_t name##_arena_alloc_list[LARGE_ARENA_LIST_SIZE];\
    fey_arena_t name##_arena;\
    name##_arena.buffer = name##_arena_buffer;\
    name##_arena.alloc_list = name##_arena_alloc_list;\
    name##_arena.free_list = name##_arena_free_list;\
    name##_arena.buffer_size = LARGE_ARENA_SIZE;\
    name##_arena.list_size = LARGE_ARENA_LIST_SIZE;\
    name##_arena.access_flag = 0;\
    fey_arena_t * name = &name##_arena;\
    fey_arena_hard_reset(name);
fey_arena_t * create_mmapped_arena(size_t requested_size);
void destroy_mmapped_arena(fey_arena_t * arena);
//Returns a pointer to a block of memory the size of the nearest multiple of eight to requested size(minimum 8 bytes), basically malloc for an arena allocator
void * fey_arena_alloc(fey_arena_t * arena, size_t requested_size);
//frees memory allocated an arena allocator, basically free for an arena allocator
void fey_arena_free(fey_arena_t *arena, void * ptr);
//reallocates memory allocated by an arena allocator, basically realloc for an arena allocator
void * fey_arena_realloc(fey_arena_t * arena, void * ptr, size_t requested_size);
//prints the memory address pointed to by every chunk of memory in an arena alloactor, whether or not it is free, and the size of it,
void fey_arena_debug(fey_arena_t * arena);