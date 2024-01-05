#ifndef FEY_UTILS_H
#define FEY_UTILS_H
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#define FEY_ARENA_SIZE 1280
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
} fey_arena_t;
#define SMALL_ARENA_SIZE 8192
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
    fey_arena_t * local = &local_arena;\
    fey_arena_hard_reset(local);
fey_arena_t * create_mmapped_arena(size_t requested_size);
//Returns a pointer to a block of memory the size of the nearest multiple of eight to requested size(minimum 8 bytes), basically malloc for an arena allocator
void * fey_arena_alloc(fey_arena_t * arena, size_t requested_size);
//frees memory allocated an arena allocator, basically free for an arena allocator
void fey_arena_free(fey_arena_t *arena, void * ptr);
//reallocates memory allocated by an arena allocator, basically realloc for an arena allocator
void * fey_arena_realloc(fey_arena_t * arena, void * ptr, size_t requested_size);
//prints the memory address pointed to by every chunk of memory in an arena alloactor, whether or not it is free, and the size of it,
void fey_arena_debug(fey_arena_t * arena);
/*creates a dynamic array type for whatever parameter is passed to the macro which uses an arena allocator for its memory operations. The name of the type is TArray_t where T is the name of the type, TArray_New() returns 
a fully set up array, TArray_Push() adds an element of type T to the highest index of the array, TArray_Pop removes the element with the highest index from the array, TArray_Insert() adds an element at a given position and moves the element currently 
occupting the position and every element above it up one position, TArray_Remove() removes an element at the given index by moving element above that index down one,TArray_Iterate() calls the inputed function on each element of the array. TArray_Free 
frees only the memory that the array owns, e.g if it is an array of pointers it will not free any of the pointers. The convention for arrays of pointers is "typedef T* Tptr" where T is the type you want to have a pointer t*/
#define EnableArrayType(T)\
typedef struct{\
    fey_arena_t * arena;\
    T* arr;\
    size_t len;\
    size_t alloc_len;\
\
}T##Array_t;\
static T##Array_t T##Array_New(fey_arena_t *arena){\
    T* arr = fey_arena_alloc(arena,4*sizeof(T)); \
    return (T##Array_t){.arena = arena,.arr = arr, .len = 0, .alloc_len = 4};\
}\
static void T##Array_Push(T##Array_t*arr, T val){\
    fey_arena_t* arena = arr->arena;\
    if(arr->len+1>arr->alloc_len){\
        arr->alloc_len *=2;\
        T * arr_new = fey_arena_alloc(arena,arr->alloc_len*sizeof(T));\
        for(int i = 0; i<arr->len; i++){\
            arr_new[i] = arr->arr[i];\
        }\
        arr->arr = arr_new;\
    }\
        arr->arr[arr->len] = val;\
        arr->len++;\
}\
static void T##Array_Pop(T##Array_t *arr){\
    arr->len--;\
}\
static void T##Array_Insert(T##Array_t *arr, long index, T val){\
    fey_arena_t* arena = arr->arena;\
    if(arr->len<=index){\
        T##Array_Push(arr, val);\
        return;\
    }\
    if(arr->len+1>=arr->alloc_len){\
        arr->alloc_len *=2;\
        T * arr_new = fey_arena_alloc(arena, arr->alloc_len*sizeof(T));\
        for(int i = 0; i<arr->len; i++){\
            arr_new[i] = arr->arr[i];\
        }\
    }\
    for(int i = arr->len; i>=index; i--){\
        arr->arr[i+1] = arr->arr[i];\
    }\
    arr->arr[index] = val;\
    arr->len++;\
}\
static void T##Array_Remove(T##Array_t *arr, long index){\
    fey_arena_t * arena = arr->arena;\
    if(index<0 || index>=arr->len){\
        return;\
    }\
    for(int i = index+1; i<arr->len; i++){\
         arr->arr[i-1] = arr->arr[i];\
    }\
    arr->len--;\
}\
static void T##Array_Free(T##Array_t * arr){\
    fey_arena_free(arr->arena,arr->arr);\
}\
static void T##Array_Iterate(T##Array_t * arr, void (*func)(T*)){\
    for(int i= 0; i<arr->len; i++){\
        func(&arr->arr[i]);\
    }\
}
typedef struct{
    fey_arena_t * arena;
    size_t len;
    char * data;
    size_t alloc_len;
}fstr;
fstr fstr_new(fey_arena_t * arena);
void fstr_delete(fstr str);
fstr subfstr(char * v, int start, int end, fey_arena_t * arena);
fstr fstr_fromStr(fey_arena_t *arena, char * c);
void fstr_push(fstr * str, char c);
fstr fstr_add(fey_arena_t * arena, fstr a, fstr b);
void fstr_cat(fstr * a, char * b);
bool fstr_eq(fstr a, fstr b);
EnableArrayType(fstr)
fstrArray_t parse_fstr(fey_arena_t * arena, char * string, char * token_seperators);
void fstr_print(fstr str);
void fstr_println(fstr str);
#define fstr_create(name, fmt, ...) fstr name  = fstr_new(local);\
    {\
        int _templ = log2(strlen(fmt)*9);\
        for(int i = 0; i<strlen(fmt); i++){_templ*=2;}\
        name.data = fey_arena_alloc(local, _templ);\
        int l = snprintf(name.data,_templ, fmt,__VA_ARGS__);\
        name.len = l;\
        name.alloc_len = _templ*9;\
    }
#endif