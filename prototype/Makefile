#
#  https://gcc.gnu.org/onlinedocs/ RTFM
#https://stackoverflow.com/questions/28037124/how-to-use-makefile-to-compile-and-update-all-files-in-one-folder-independently
CC=gcc
CFLAGS=-g3 
SOURCES=$(wildcard *.c)
TESTS=$(wildcard *_proto.c)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=$(TESTS:.c=)
all: 	$(EXECUTABLE)
%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@ 
$(EXECUTABLE): % : %.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@  eprintf.c
.PHONY : clean test
clean :
	rm *.o
	rm $(EXECUTABLE)
test :
	ls -A

