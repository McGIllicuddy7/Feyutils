#include "feylibcore.h"
#include "feylib_fstr.h"
#include <stdio.h>
#include <string.h>
//fstr functions
static inline void feylib_wait_arena_exclusion(atomic_int * exclusion){
    while(*exclusion){
    }
    *exclusion = 1;
}
static inline void feylib_release_arena_exclusion(atomic_int * exclusion){
    *exclusion = 0;
}
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
    feylib_wait_arena_exclusion(&str->exclusion);
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
        fey_arena_free(arena,str->data);
        str->data = buff;
    }
    feylib_release_arena_exclusion(&str->exclusion);
}
fstr fstr_add(fey_arena_t * arena,fstr a, fstr b){
    fstr out = {0,0,0,arena};
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
    out.exclusion = 0;
    return out;
}
void fstr_delete(fstr str){
    fey_arena_t * arena = str.arena;
    fey_arena_free(arena, str.data);
    str.data = 0;
    str.alloc_len = 0;
    str.len = 0;
}
fstr fstr_clone(fstr a){
    fstr out;
    fey_arena_t * arena = a.arena;
    out.data = fey_arena_alloc(arena, a.alloc_len);
    out.arena = arena;
    memcpy(out.data, a.data, a.alloc_len);
    out.alloc_len = a.alloc_len;
    out.len = a.len;
    out.exclusion = 0;
    return out;
}
