make:
	gcc feyutils.c -c -Wall
	ar -rcs libfeyutils.a feyutils.o
	rm -rf feyutils.o
	mv libfeyutils.a /usr/local/lib
	cp feyutils.h /usr/local/include
t:
	gcc main.c feyutils.c -g3 -Wall
