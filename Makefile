lru: lru.c
	gcc -Os $$(pkg-config --libs --cflags glib-2.0) lru.c -o lru

clean:
	rm -f lru
