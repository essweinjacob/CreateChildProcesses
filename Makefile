CC = gcc
CFLAGS = -g -Wall
TARGET = oss
OBJS = oss.o prime.o
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)
oss.o: oss.c
	$(CC) $(CFLAGS) -c oss.c
prime.o: prime.c
	$(CC) $(CFLAGS) -c prime.c
clean:
	/bin/rm -f *.o $(TARGET)
