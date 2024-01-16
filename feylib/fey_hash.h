#pragma once
#include "feyutils.h"
uint64_t hash_charptr(const char * value);
uint64_t hash_fstr(const fstr value);
uint64_t hash_double(double value);
uint64_t hash_long(long value);
