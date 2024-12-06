#include <stdlib.h>
#include "trie.h"
#include "redismodule.h"
#include <string.h>
#include "redismodule.h"

TrieNode* createNode(char character) {
    TrieNode *node = malloc(sizeof(TrieNode));
    node->character = character;
    node->is_end_of_word = 0;
    for (int i = 0; i < 26; i++) {
        node->children[i] = NULL;
    }
    return node;
}

void addWord(TrieNode *root, const char *word, const char *value) {
    TrieNode *current = root;
    while (*word) {
        int index = *word - 'a';
        if (index < 0 || index >= 26) {
            return;
        }
        if (current->children[index] == NULL) {
            current->children[index] = createNode(*word);
        }
        current = current->children[index];
        word++;
    }
    current->is_end_of_word = 1;
    if (current->value) {
        free(current->value);
    }
    current->value = strdup(value);
}

char* searchWord(TrieNode *root, const char *word) {
    TrieNode *current = root;
    while (*word) {
        int index = *word - 'a';
        if (index < 0 || index >= 26 || current->children[index] == NULL) {
            return NULL;
        }
        current = current->children[index];
        word++;
    }
    return current->is_end_of_word ? current->value : NULL;
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
        return 0; 
    }

    if (depth == strlen(word)) {
        if (!node->is_end_of_word) {
            return 0; 
        }

        node->is_end_of_word = 0;
        if (node->value) {
            free(node->value);
            node->value = NULL;
        }

        for (int i = 0; i < 26; i++) {
            if (node->children[i] != NULL) {
                return 0; 
            }
        }

        return 1; 
    }

    int index = word[depth] - 'a';
    if (deleteWord(node->children[index], word, depth + 1)) {
        free(node->children[index]);
        node->children[index] = NULL;

        if (!node->is_end_of_word && node->value == NULL) {
            for (int i = 0; i < 26; i++) {
                if (node->children[i] != NULL) {
                    return 0;
                }
            }
            return 1; 
        }
    }

    return 0;
}

int wildcardSearch(TrieNode *node, const char *pattern, char *buffer, int depth, RedisModuleString **result, int *count) {
    if (node == NULL) return 0;

    if (*pattern == '\0') {
        if (node->is_end_of_word) {
            buffer[depth] = '\0';
            char output[1024];
            snprintf(output, sizeof(output), "%s: %s", buffer, node->value);
            result[*count] = RedisModule_CreateString(NULL, output, strlen(output));
            (*count)++;
        }
        return 1;
    }

    if (*pattern == '?') {
        for (int i = 0; i < 26; i++) {
            if (node->children[i]) {
                buffer[depth] = 'a' + i;
                wildcardSearch(node->children[i], pattern + 1, buffer, depth + 1, result, count);
            }
        }
    } else if (*pattern == '*') {
        wildcardSearch(node, pattern + 1, buffer, depth, result, count); 
        for (int i = 0; i < 26; i++) {
            if (node->children[i]) {
                buffer[depth] = 'a' + i;
                wildcardSearch(node->children[i], pattern, buffer, depth + 1, result, count); 
            }
        }
    } else {
        int index = *pattern - 'a';
        if (index >= 0 && index < 26 && node->children[index]) {
            buffer[depth] = *pattern;
            wildcardSearch(node->children[index], pattern + 1, buffer, depth + 1, result, count);
        }
    }

    return 0;
}



