CC = gcc
CFLAGS = -lpthread -o avadsk
SOURCE = avadsk.c
TARGET = avadsk

all:
	$(CC) $(SOURCE) $(CFLAGS)

clean:
	rm -f $(TARGET)

debug:
	$(CC) -DDEBUG $(SOURCE) $(CFLAGS)
