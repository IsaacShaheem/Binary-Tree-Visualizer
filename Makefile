CC = clang
CFLAGS = -std=c99 -Wall -Wextra -pedantic -fPIC -Ic

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	LIB_EXT = dylib
	SHARED_FLAGS = -dynamiclib
else
	LIB_EXT = so
	SHARED_FLAGS = -shared
endif

LIB = build/libtree.$(LIB_EXT)
TEST_BIN = build/test_tree

.PHONY: all clean test legacy

all: $(LIB)

$(LIB): c/tree.c c/tree.h | build
	$(CC) $(CFLAGS) $(SHARED_FLAGS) c/tree.c -o $(LIB)

$(TEST_BIN): tests/test_tree.c c/tree.c c/tree.h | build
	$(CC) $(CFLAGS) tests/test_tree.c c/tree.c -o $(TEST_BIN)

build:
	mkdir -p build

test: $(TEST_BIN)
	$(TEST_BIN)

legacy: bin/avl

bin/avl: include/avl.h src/avl.c src/mainAVL.c | bin
	$(CC) -std=c99 -Wall -pedantic -Iinclude src/avl.c src/mainAVL.c -o bin/avl

bin:
	mkdir -p bin

clean:
	rm -rf build
