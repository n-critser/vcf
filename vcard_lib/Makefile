#
#  https://gcc.gnu.org/onlinedocs/ RTFM
#https://stackoverflow.com/questions/28037124/how-to-use-makefile-to-compile-and-update-all-files-in-one-folder-independently
CC=gcc     #++ -std=c++11
CFLAGS=-g3 
XFLAGS=-Q # this flag will output alot !https://gcc.gnu.org/onlinedocs/gcc-2.95.2/gcc_2.html#SEC9
LDFLAGS=
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
TESTS=$(wildcard *_test.c)
EXECUTABLE=$(TESTS:.c=)

all: 	$(EXECUTABLE)




%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(EXECUTABLE): % : %.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ vcard.c eprintf.c

.PHONY : clean test setup

clean :
	rm $(EXECUTABLE)
	rm *.o

test :	all
	bash test_weaksearch.sh
setup :
	@ printf "hello,world\n"
	@ if [ ! -d bin ]; then  mkdir bin; fi;
	ls -alt
