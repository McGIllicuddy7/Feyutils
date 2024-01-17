#pragma once
#include "feylibcore.h"
#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
/*creates a dynamic array type for whatever parameter is passed to the macro which uses an arena allocator for its memory operations. The name of the type is TArray_t where T is the name of the type, TArray_New() returns 
a fully set up array, TArray_Push() adds an element of type T to the highest index of the array, TArray_Pop removes the element with the highest index from the array, TArray_Insert() adds an element at a given position and moves the element currently 
occupting the position and every element above it up one position, TArray_Remove() removes an element at the given index by moving element above that index down one,TArray_Iterate() calls the inputed function on each element of the array. TArray_Free 
frees only the memory that the array owns, e.g if it is an array of pointers it will not free any of the pointers. The convention for arrays of pointers is "typedef T* Tptr" where T is the type you want to have a pointer t*/
static inline void feylib_wait_arena_exclusion(atomic_int * exclusion){
    while(*exclusion){
    }
    *exclusion = 1;
}
static inline void feylib_release_arena_exclusion(atomic_int * exclusion){
    *exclusion = 0;
}
#define EnableArrayType(T)\
typedef struct{\
    T* arr;\
    size_t len;\
    size_t alloc_len;\
    fey_arena_t * arena;\
    atomic_int exclusion;\
\
}T##Array_t;\
static T##Array_t T##Array_New(fey_arena_t *arena){\
    T* arr = fey_arena_alloc(arena,8*sizeof(T)); \
    return (T##Array_t){.arena = arena,.arr = arr, .len = 0, .alloc_len = 8, .exclusion = 0};\
}\
static void T##Array_Push(T##Array_t*arr, T val){\
    feylib_wait_arena_exclusion(&arr->exclusion);\
    fey_arena_t* arena = arr->arena;\
    if((arr->len+1)>(arr->alloc_len)){\
        arr->alloc_len *=2;\
        T * arr_new = (T*)fey_arena_alloc(arena,arr->alloc_len*sizeof(T));\
        for(int i = 0; i<arr->len; i++){\
            arr_new[i] = arr->arr[i];\
        }\
        T * old = arr->arr;\
        arr->arr = arr_new;\
        fey_arena_free(arena,old);\
    }\
    arr->arr[arr->len] = val;\
    arr->len++;\
    feylib_release_arena_exclusion(&arr->exclusion);\
}\
static void T##Array_Pop(T##Array_t *arr){\
    feylib_wait_arena_exclusion(&arr->exclusion);\
    arr->len--;\
    feylib_release_arena_exclusion(&arr->exclusion);\
}\
static void T##Array_Insert(T##Array_t *arr, long index, T val){\
    fey_arena_t* arena = arr->arena;\
    if(arr->len<=index){\
        T##Array_Push(arr, val);\
        return;\
    }\
    feylib_wait_arena_exclusion(&arr->exclusion);\
    if(arr->len+1>=arr->alloc_len){\
        arr->alloc_len *=2;\
        T * arr_new = (T*)fey_arena_alloc(arena, arr->alloc_len*sizeof(T));\
        for(int i = 0; i<arr->len; i++){\
            arr_new[i] = arr->arr[i];\
        }\
    }\
    for(int i = arr->len; i>=index; i--){\
        arr->arr[i+1] = arr->arr[i];\
    }\
    arr->arr[index] = val;\
    arr->len++;\
    feylib_release_arena_exclusion(&arr->exclusion);\
}\
static void T##Array_Remove(T##Array_t *arr, long index){\
    feylib_wait_arena_exclusion(&arr->exclusion);\
    if(index<0 || index>=arr->len){\
        return;\
    }\
    for(int i = index+1; i<arr->len; i++){\
         arr->arr[i-1] = arr->arr[i];\
    }\
    arr->len--;\
    feylib_release_arena_exclusion(&arr->exclusion);\
}\
static void T##Array_Reserve(T##Array_t * arr, size_t new_size){\
    feylib_wait_arena_exclusion(&arr->exclusion);\
    size_t old_len = arr->alloc_len;\
    while(arr->alloc_len<new_size){\
        arr->alloc_len *= 2;\
    }\
    if(arr->alloc_len == old_len){\
        return;\
    }\
    T * arena_new = fey_arena_alloc(arr->arena, arr->alloc_len*sizeof(T));\
    memcpy(arena_new, arr->arena, arr->len*sizeof(T));\
    feylib_release_arena_exclusion(&arr->exclusion);\
}\
static void T##Array_Free(T##Array_t * arr){\
    feylib_wait_arena_exclusion(&arr->exclusion);\
    fey_arena_free(arr->arena,arr->arr);\
    feylib_release_arena_exclusion(&arr->exclusion);\
}\
static void T##Array_Iterate(T##Array_t * arr, void (*func)(T*)){\
    for(int i= 0; i<arr->len; i++){\
        func(&arr->arr[i]);\
    }\
}

static int int_cmp(const void* a, const void *b){
    return ( *(int*)a - *(int*)b );
}
static int long_cmp(const void* a, const void *b){
    return ( *(long*)a - *(long*)b );
}
static int double_cmp(const void* a, const void *b){
   return ( *(double*)a - *(double*)b );
}
static int float_cmp(const void* a, const void *b){
    return ( *(float*)a - *(float*)b );
}

EnableArrayType(float);
EnableArrayType(int);
EnableArrayType(double);
EnableArrayType(long);

static void floatArray_Sort(floatArray_t * array){
    qsort(array->arr, array->len, sizeof(float),float_cmp);
}
static void intArray_Sort(intArray_t * array){
    qsort(array->arr, array->len, sizeof(int),int_cmp);
}
static void doubleArray_Sort(doubleArray_t * array){
    qsort(array->arr, array->len, sizeof(double),double_cmp);
}
static void longArray_Sort(longArray_t * array){
    qsort(array->arr, array->len, sizeof(long),long_cmp);
}
static void floatArray_Print(floatArray_t * array){
    printf("{ ");
    for(int i =0; i<array->len; i++){
        if(i<array->len-1){
            printf("%f, ",array->arr[i]);
        }
        else{
            printf("%f",array->arr[i]);
        }
    }
    printf("}\n");
}
static void doubleArray_Print(doubleArray_t * array){
    printf("{ ");
    for(int i =0; i<array->len; i++){
        if(i<array->len-1){
            printf("%lf, ",array->arr[i]);
        }
        else{
            printf("%lf",array->arr[i]);
        }
    }
    printf("}\n");
}
static void intArray_Print(intArray_t * array){
    printf("{ ");
    for(int i =0; i<array->len; i++){
        if(i<array->len-1){
            printf("%d, ",array->arr[i]);
        }
        else{
            printf("%d",array->arr[i]);
        }
    }
    printf("}\n");
}
static void longArray_Print(longArray_t * array){
    printf("{ ");
    for(int i =0; i<array->len; i++){
        if(i<array->len-1){
            printf("%ld, ",array->arr[i]);
        }
        else{
            printf("%ld",array->arr[i]);
        }
    }
    printf("}\n");
}