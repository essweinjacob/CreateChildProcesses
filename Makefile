CC = gcc
CFLAGS = -g
TARGET1 = prime
TARGET2 = oss
OBJS1 = prime.o
OBJS2 = oss.o

all: prime oss
$(TARGET1): $(OBJS1)
		$(CC) -o $(TARGET1) $(OBJS1)
$(TARGET2): $(OBJS2)
		$(CC) -o $(TARGET2) $(OBJS2)
prime.o: prime.c
		$(CC) $(CFLAGS) -c prime.c ossPrime.h
oss.o: oss.c
		$(CC) $(CFLAGS) -c oss.c ossPrime.h
clean:
		/bin/rm -f *.o $(TARGET1) $(TARGET2) output.txt
