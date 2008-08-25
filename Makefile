CC = gcc

FLAGS = -fPIC -g -Os

all: cache.o protocol.o

lru.o: lru.h lru.c
	$(CC) -c $(FLAGS) $$(pkg-config --cflags glib-2.0) lru.c

cache.o: cache.h cache.c lru.o
	$(CC) -c $(FLAGS) $$(pkg-config --cflags glib-2.0) cache.c

protocol.o: protocol.h protocol.c
	$(CC) -c $(FLAGS) protocol.c

clean:
	rm -f *.o
	rm -rf python/build
	find . -name '*.py[co]' | xargs rm -f
