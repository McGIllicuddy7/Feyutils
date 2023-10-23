#include "feyutils.h"
#include <stdio.h>
void parse_input(fey_arena_t * arena, char * buffer){
	fstrArray_t out = parse_fstr(arena,buffer, "r -> * + - /");
    printf("\n\n");
	for(int i =0; i<out.len; i++){
		printf("%s,",out.arr[i].data);
	}
	return;
}
void repl(void){
	fey_arena_init();
	printf("enter operation: ");
	char buffer[100];
	fgets(buffer, 100, stdin);
	parse_input(local,buffer);
	return;
error:
	printf("usage; rn(operator)(some value)->rm");
}

int main(void){
  	//repl();
	fey_arena_init();
	fstr_create(bruh, "%f,", M_PI);
	fstr_print(bruh);
    return 0;
}