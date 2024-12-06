#ifndef TRIE_H
#define TRIE_H

#include "redismodule.h"


typedef struct TrieNode {
    char character;
    int is_end_of_word;
    struct TrieNode *children[26];
    char *value; 
} TrieNode;

TrieNode* createNode(char character);
void addWord(TrieNode *root, const char *word, const char *value);
char* searchWord(TrieNode *root, const char *word);
void collectWordsWithPrefix(TrieNode *node, const char *prefix, char *buffer, int depth, RedisModuleString **result, int *count);
int deleteWord(TrieNode *node, const char *word, int depth);
int wildcardSearch(TrieNode *node, const char *pattern, char *buffer, int depth, RedisModuleString **result, int *count);


#endif
