all: index

index: indexer.o libftokenizer.a
	gcc indexer.o libftokenizer.a -o index

indexer.o: indexer.c
	gcc -c -o indexer.o indexer.c

clean:
	rm -f index *.o
