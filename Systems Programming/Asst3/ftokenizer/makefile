all: libftokenizer.a

libftokenizer.a: ftokenizer.o
	ar rcs libftokenizer.a ftokenizer.o

ftokenizer.o: ftokenizer.c
	gcc -g -Wall -Werror -c -o ftokenizer.o ftokenizer.c

clean:
	rm -f libftokenizer.a ftokenizer.o *.gch
