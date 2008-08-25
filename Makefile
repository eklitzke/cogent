lru.o: lru.h lru.c
	gcc -c -Os $$(pkg-config --cflags glib-2.0) lru.c

lru: lru.c
	gcc -Os $$(pkg-config --libs --cflags glib-2.0) lru.c -o lru

clean:
	rm -f lru lru.o
