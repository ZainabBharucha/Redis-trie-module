#include <stdlib.h>
#include "trie.h"
#include "redismodule.h"
#include <string.h>
#include "trie.h"

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
void addWord(TrieNode *root, const char *word, const char *value) {
    TrieNode *current = root;
    while (*word) {
        int index = *word - 'a';
        if (index < 0 || index >= 26) {
            return; // Skip invalid characters
        }
        if (current->children[index] == NULL) {
            current->children[index] = createNode(*word);
        }
        current = current->children[index];
        word++;
    }
    current->is_end_of_word = 1;
    if (current->value) {
        free(current->value); // Free old value if it exists
    }
    current->value = strdup(value); // Store the new value
}


// Search for a word in the Trie
char* searchWord(TrieNode *root, const char *word) {
    TrieNode *current = root;
    while (*word) {
        int index = *word - 'a';
        if (index < 0 || index >= 26 || current->children[index] == NULL) {
            return NULL; // Key not found
        }
        current = current->children[index];
        word++;
    }
    return current->is_end_of_word ? current->value : NULL; // Return value if key exists
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

int deleteWord(TrieNode *node, const char *word, int depth) {
    if (node == NULL) {
        return 0; // Key not found
    }

    if (depth == strlen(word)) {
        if (!node->is_end_of_word) {
            return 0; // Key not found
        }

        node->is_end_of_word = 0;
        if (node->value) {
            free(node->value);
            node->value = NULL;
        }

        for (int i = 0; i < 26; i++) {
            if (node->children[i] != NULL) {
                return 0; // Node has children, cannot delete
            }
        }

        return 1; // Node can be deleted
    }

    int index = word[depth] - 'a';
    if (deleteWord(node->children[index], word, depth + 1)) {
        free(node->children[index]);
        node->children[index] = NULL;

        if (!node->is_end_of_word && node->value == NULL) {
            for (int i = 0; i < 26; i++) {
                if (node->children[i] != NULL) {
                    return 0; // Node still has children
                }
            }
            return 1; // Node can be deleted
        }
    }

    return 0;
}


