CC = gcc

all: cache

lru.o: lru.h lru.c
	$(CC) -c -Os $$(pkg-config --cflags glib-2.0) lru.c

#cache.o: cache.h cache.c
#	$(CC) -c -Os $$(pkg-config --cflags glib-2.0) cache.c

cache: cache.h cache.c lru.o
	$(CC) -Os $$(pkg-config --cflags --libs glib-2.0) lru.o cache.c -o cache

clean:
	rm -f lru lru.o
