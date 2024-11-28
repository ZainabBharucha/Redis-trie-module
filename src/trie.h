#ifndef TRIE_H
#define TRIE_H

#include <stdlib.h>
#include <string.h>
#include "redismodule.h" // Required for RedisModuleString

typedef struct TrieNode {
    char character;
    int is_end_of_word;
    struct TrieNode *children[26];
} TrieNode;

// Function prototypes
TrieNode* createNode(char character);
void addWord(TrieNode *root, const char *word);
int searchWord(TrieNode *root, const char *word);
void collectWordsWithPrefix(TrieNode *node, const char *prefix, char *buffer, int depth, RedisModuleString **result, int *count);

#endif
