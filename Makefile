CC = gcc
CFLAGS = -Wall
LDFLAGS = -lreadline -lpthread

all: servudp cliudp servbeuip clibeuip test_creme biceps

servudp: servudp.c
	$(CC) $(CFLAGS) -o servudp servudp.c

cliudp: cliudp.c
	$(CC) $(CFLAGS) -o cliudp cliudp.c

servbeuip: servbeuip.c
	$(CC) $(CFLAGS) -o servbeuip servbeuip.c

clibeuip: clibeuip.c
	$(CC) $(CFLAGS) -o clibeuip clibeuip.c

creme.o: creme.c creme.h
	$(CC) $(CFLAGS) -c creme.c

gescom.o: gescom.c gescom.h
	$(CC) $(CFLAGS) -c gescom.c

test_creme: test_creme.c creme.o
	$(CC) $(CFLAGS) -o test_creme test_creme.c creme.o

biceps: biceps.c creme.o gescom.o
	$(CC) $(CFLAGS) -o biceps biceps.c creme.o gescom.o $(LDFLAGS)

trace: clean
	$(MAKE) CFLAGS="-Wall -DTRACE" all

clean:
	rm -f servudp cliudp servbeuip clibeuip test_creme biceps *.o