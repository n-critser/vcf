#
#  https://gcc.gnu.org/onlinedocs/ RTFM
#https://stackoverflow.com/questions/28037124/how-to-use-makefile-to-compile-and-update-all-files-in-one-folder-independently
CC=gcc     #++ -std=c++11
CFLAGS=-g3 
XFLAGS=-Q # this flag will output alot !https://gcc.gnu.org/onlinedocs/gcc-2.95.2/gcc_2.html#SEC9
CPPFLAGS=-Werror -Wall -Wpedantic -Wextra    -Wshadow  -Wmissing-noreturn -Wmissing-declarations -Wconversion
PROGS = numbers isPrime
LDFLAGS=
SOURCES=$(wildcard *.c)
TESTS=$(wildcard *_proto.c)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=$(TESTS:.c=)
all: 	$(EXECUTABLE)
%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@ 
$(EXECUTABLE): % : %.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@  ../eprintf.c
.PHONY : clean test setup
clean :
	rm *.o
	rm $(EXECUTABLE)
test :
	ls -A

setup :
	@ printf "hello,world\n"
	@ if [ ! -d bin ]; then  mkdir bin; fi;
	ls -alt
