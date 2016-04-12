all: libftokenizer.a

libftokenizer: ftokenizer.o
	ar rcs libftokenizer.a ftokenizer.o

ftokenizer.o: ftokenizer.c
	gcc -g -Wall -Werror -c -o ftokenizer.o

clean:
	rm -f libftokenizer.a ftokenizer.o *.gch
