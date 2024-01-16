#include "fey_hash.h"
uint64_t hash_charptr(const char * value){
    uint64_t total = 0;
    uint64_t mlt = 1;
    int l = strlen(value);
    for(int i = 0; i<l; i++){
        total += value[i]*mlt;
        mlt *= 31;
    }
    return total;
}
uint64_t hash_fstr(const fstr value){
    uint64_t total = 0;
    uint64_t mlt = 1;
    int l = value.len;
    char * v = value.data;
    for(int i = 0; i<l; i++){
        total += v[i]*mlt;
        mlt *= 17;
    }
    return total;
}
uint64_t hash_double(double value){
    unsigned long total = *(unsigned long *)&value;
    total ^= total>> 7;
    total ^= total<< 9;
    total ^= total >> 13;
    total ^= total>> 3;
    total ^= total<< 5;
    total ^= total >> 7;
    return total;
}
uint64_t hash_long(long value){
    unsigned long total = value;
    total ^= total>> 7;
    total ^= total<< 9;
    total ^= total >> 13;
    total ^= total>> 3;
    total ^= total<< 5;
    total ^= total >> 7;
    return total;
}