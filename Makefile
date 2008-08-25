CC = gcc

all: cache.o

lru.o: lru.h lru.c
	$(CC) -c -fPIC -Os $$(pkg-config --cflags glib-2.0) lru.c

cache.o: cache.h cache.c lru.o
	$(CC) -c -fPIC -Os $$(pkg-config --cflags glib-2.0) cache.c

clean:
	rm -f *.o
