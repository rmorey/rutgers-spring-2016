CC=gcc
CFLAGS=-c -g -Wall -Werror
LDFLAGS=
SOURCES=main.c sorted-list.c
SLLIB=libsl.a
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=sl

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): main.o $(SLLIB)
	$(CC) $(LDFLAGS) $< $(SLLIB) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

$(SLLIB): sorted-list.o
	ar rcs $@ $<

clean:
	rm $(OBJECTS) $(EXECUTABLE) $(SLLIB)
