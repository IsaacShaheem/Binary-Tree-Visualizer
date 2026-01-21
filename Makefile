CC = clang
CFLAGS = -std=c99 -Wall -pedantic -Iinclude

all: bin/avl

bin/avl: include/avl.h src/avl.c src/mainAVL.c
	$(CC) $(CFLAGS) src/avl.c src/mainAVL.c -o bin/avl

clean:
	rm -f bin/*