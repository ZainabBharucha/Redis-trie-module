#ifndef TRIE_H
#define TRIE_H

#include "redismodule.h"

// Trie Node structure with value support
typedef struct TrieNode {
    char character;
    int is_end_of_word;
    struct TrieNode *children[26];
    char *value; // Pointer to store the value associated with the key
} TrieNode;

// Function prototypes
TrieNode* createNode(char character);
void addWord(TrieNode *root, const char *word, const char *value);
char* searchWord(TrieNode *root, const char *word);
void collectWordsWithPrefix(TrieNode *node, const char *prefix, char *buffer, int depth, RedisModuleString **result, int *count);
int deleteWord(TrieNode *node, const char *word, int depth);

#endif
