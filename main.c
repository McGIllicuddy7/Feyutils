#include "feyutils.h"
#include <stdio.h>
EnableArrayType(int);
int main(void){
	fey_init_small_arena();
	fstr baby  = fstr_fromStr(local, "hello baby ");
	fstr_cat(&baby, "i love you so so so much");
	fstr_print(baby);
    return 0;
}