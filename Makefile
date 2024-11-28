NAME=trie_module
SRC=src/module.c src/trie.c
CC=gcc
CFLAGS=-g -fPIC -Wall -O2 -I. # Include current directory for redismodule.h
LDFLAGS=-shared

all:
	$(CC) $(CFLAGS) $(SRC) -o $(NAME).so $(LDFLAGS)

clean:
	rm -f $(NAME).so

