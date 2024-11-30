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

int deleteWord(TrieNode *node, const char *word, int depth) {
    if (node == NULL) {
        return 0; // Word not found
    }

    // Base case: end of the word
    if (depth == strlen(word)) {
        if (!node->is_end_of_word) {
            return 0; // Word not found
        }

        node->is_end_of_word = 0; // Mark as non-terminal
        // Check if the node has no children
        for (int i = 0; i < 26; i++) {
            if (node->children[i] != NULL) {
                return 0; // Node has children, cannot delete
            }
        }

        return 1; // Node can be deleted
    }

    // Recursive case: traverse to the next character
    int index = word[depth] - 'a';
    if (deleteWord(node->children[index], word, depth + 1)) {
        free(node->children[index]); // Free the child node
        node->children[index] = NULL;

        // Check if the current node is now a leaf
        if (!node->is_end_of_word) {
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

