#include "feyutils.h"
#include <stdio.h>
#include <unistd.h>
EnableArrayType(int);
void intArray_Print(intArray_t * arr){
	for(int i =0; i<arr->len; i++){
		printf("%d, ", arr->arr[i]);
	}
}
int main(void){
	fey_init_small_arena();
	intArray_t arr = intArray_New(local);
	for(int i =0; i<1024; i++){
		intArray_Push(&arr, i);
	}
	intArray_Print(&arr);
	printf("\n");
	fey_arena_debug(local);
	return 0;
}

