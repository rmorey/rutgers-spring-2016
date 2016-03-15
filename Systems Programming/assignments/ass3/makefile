all: mymalloc.o

all-tests: test0.c test1.c test2.c test3.c test4.c mymalloc.o
	make test0 \
	make test1 \
	make test2 \
	make test3 \
	make test4 \

test0: test0.c mymalloc.o
	gcc -g -Wall -Werror test0.c mymalloc.o -o test0

test1: test1.c mymalloc.o
	gcc -g -Wall -Werror test1.c mymalloc.o -o test1

test2: test2.c mymalloc.o
	gcc -g -Wall -Werror test2.c mymalloc.o -o test2

test3: test3.c mymalloc.o
	gcc -g -Wall -Werror test3.c mymalloc.o -o test3

test4: test4.c mymalloc.o
	gcc -g -Wall -Werror test4.c mymalloc.o -o test4

mymalloc.o: mymalloc.c mymalloc.h
	gcc -g -Wall -Werror -c mymalloc.c

clean:
	rm -f *.o test0 test1 test2 test3 test4 
