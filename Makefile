CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS = -lreadline -lpthread

all: biceps

biceps: biceps.o creme.o gescom.o
	$(CC) $(CFLAGS) -o biceps biceps.o creme.o gescom.o $(LDFLAGS)

biceps.o: biceps.c gescom.h creme.h
	$(CC) $(CFLAGS) -c biceps.c

creme.o: creme.c creme.h
	$(CC) $(CFLAGS) -c creme.c

gescom.o: gescom.c gescom.h
	$(CC) $(CFLAGS) -c gescom.c

memory-leak: clean
	$(CC) -g -O0 -Wall -Werror -o biceps-memory-leaks biceps.c creme.c gescom.c $(LDFLAGS)

clean:
	rm -f biceps biceps-memory-leaks *.o