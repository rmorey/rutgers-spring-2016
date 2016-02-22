all: sorted
                                                                                                                                      
sorted:  sorted-list.h sorted-list.o main.c
	gcc -g -Wall -o sorted main.c sorted-list.o 
  
sorted-list.o: sorted-list.c
	gcc -g -Wall -c sorted-list.c
  
clean:
	rm -f *.o sorted
