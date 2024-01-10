#pragma once
#include "feylibcore.h"
typedef struct{
    char * data;
    size_t len;
    size_t alloc_len;
    fey_arena_t * arena;
}fstr;
fstr fstr_new(fey_arena_t * arena);
void fstr_delete(fstr str);
fstr subfstr(char * v, int start, int end, fey_arena_t * arena);
fstr fstr_fromStr(fey_arena_t *arena, char * c);
void fstr_push(fstr * str, char c);
fstr fstr_add(fey_arena_t * arena, fstr a, fstr b);
void fstr_cat(fstr * a, char * b);
bool fstr_eq(fstr a, fstr b);
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
