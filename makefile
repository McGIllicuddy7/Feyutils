make: fey_mem.c feylib_fstr.c fey_linked_list.c
	gcc -c fey_mem.c feylib_fstr.c fey_linked_list.c
	ar -rcs feyutils.a fey_mem.o feylib_fstr.o fey_linked_list.o
	rm -rf fey_mem.o feylib_fstr.o fey_linked_list.o