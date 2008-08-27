.PHONY: all clean tags

CC = gcc

FLAGS = -fPIC -g -Os

all: cogent

cogent: cache.o protocol.o lru.o server.c
	$(CC) -g -Os lru.o cache.o protocol.o $$(pkg-config --cflags --libs glib-2.0) server.c -o cogent

lru.o: lru.h lru.c
	$(CC) -c $(FLAGS) $$(pkg-config --cflags glib-2.0) lru.c

cache.o: cache.h cache.c lru.o
	$(CC) -c $(FLAGS) $$(pkg-config --cflags glib-2.0) cache.c

protocol.o: protocol.h protocol.c
	$(CC) -c $(FLAGS) $$(pkg-config --cflags glib-2.0) protocol.c

tags:
	find . -name '*.[ch]' | ctags -L -

clean:
	-rm -f *.o
	-rm -rf python/build
	-rm -rf client/build
	-find . -name '*.py[co]' | xargs rm -f
	-rm -f cogent
