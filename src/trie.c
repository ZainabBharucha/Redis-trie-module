#include <stdlib.h>
#include "trie.h"
#include "redismodule.h"
#include <string.h>

// Create a new Trie node
TrieNode* createNode(char character) {
    TrieNode *node = malloc(sizeof(TrieNode));
    node->character = character;
    node->is_end_of_word = 0;
    for (int i = 0; i < 26; i++) {
        node->children[i] = NULL;
    }
    return node;
}

// Add a word to the Trie
void addWord(TrieNode *root, const char *word) {
    TrieNode *current = root;
    while (*word) {
        int index = *word - 'a';
        if (current->children[index] == NULL) {
            current->children[index] = createNode(*word);
        }
        current = current->children[index];
        word++;
    }
    current->is_end_of_word = 1;
}

// Search for a word in the Trie
int searchWord(TrieNode *root, const char *word) {
    TrieNode *current = root;
    while (*word) {
        int index = *word - 'a';
        if (current->children[index] == NULL) {
            return 0;
        }
        current = current->children[index];
        word++;
    }
    return current->is_end_of_word;
}

void collectWordsWithPrefix(TrieNode *node, const char *prefix, char *buffer, int depth, RedisModuleString **result, int *count) {
    if (node == NULL) return;

    if (node->is_end_of_word) {
        buffer[depth] = '\0';
        result[*count] = RedisModule_CreateString(NULL, buffer, strlen(buffer));
        (*count)++;
    }

    for (int i = 0; i < 26; i++) {
        if (node->children[i]) {
            buffer[depth] = 'a' + i;
            collectWordsWithPrefix(node->children[i], prefix, buffer, depth + 1, result, count);
        }
    }
}
