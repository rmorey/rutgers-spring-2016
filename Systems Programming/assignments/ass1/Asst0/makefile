all: tokenizer

tokenizer:  header.h tokenizer.o

tokenizer.o: tokenizer.c
	gcc -g -Wall -c tokenizer.c

clean:
	rm -f *.o tokenizer
