CC = gcc
CFLAGS = -Wall -Wextra -g

VPATH = src:test

vector.o : vector.c vector.h 
			$(CC) -c $(CFLAGS) src/vector.c -o vector.o

vector_multi.o: vector.c vector.h
						$(CC) -pthread -c $(CFLAGS) -DMULTITHREADED src/vector.c -o vector_multi.o

vector_test.o : vector_test.c vector.h 
		$(CC) -c $(CFLAGS) test/vector_test.c -o vector_test.o

		
vector_multi_test.o : vector_test.c vector.h 
		$(CC) -pthread -c $(CFLAGS) test/vector_multi_test.c -o vector_multi_test.o

vector_test : vector_test.o vector.o 
		$(CC) -o vector_test vector_test.o vector.o

vector_multi_test : vector_multi_test.o vector_multi.o
		$(CC) -pthread -o vector_multi_test vector_multi_test.o vector_multi.o

all : vector.o vector_multi.o vector_test.o vector_multi_test.o vector_test vector_multi_test