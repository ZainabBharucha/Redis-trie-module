#include "redismodule.h"
#include "trie.h"
#include <string.h>

// Declare the global Trie root node
TrieNode *trie_root = NULL;

// Forward declarations
int TrieAddCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int TrieSearchCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int TriePrefixSearchCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int TrieDeleteCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);


// Module Initialization
int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (RedisModule_Init(ctx, "trie", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    // Register the TRIE.ADD command
    if (RedisModule_CreateCommand(ctx, "trie.add", TrieAddCommand, "write", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    // Register the TRIE.SEARCH command
    if (RedisModule_CreateCommand(ctx, "trie.search", TrieSearchCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    if (RedisModule_CreateCommand(ctx, "trie.prefix_search", TriePrefixSearchCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    if (RedisModule_CreateCommand(ctx, "trie.delete", TrieDeleteCommand, "write", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }
    return REDISMODULE_OK;
}

// Redis Command: Add a word to the Trie
int TrieAddCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 2) {
        return RedisModule_ReplyWithError(ctx, "ERR wrong number of arguments");
    }

    // Extract the word
    size_t len;
    const char *word = RedisModule_StringPtrLen(argv[1], &len);

    // Initialize the Trie root if it doesn't exist
    if (trie_root == NULL) {
        trie_root = createNode('\0'); // Root node with a null character
    }

    // Add the word to the Trie
    addWord(trie_root, word);

    // Reply to the client
    return RedisModule_ReplyWithSimpleString(ctx, "OK");
}

// Redis Command: Search for a word in the Trie
int TrieSearchCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 2) {
        return RedisModule_ReplyWithError(ctx, "ERR wrong number of arguments");
    }

    // Extract the word
    size_t len;
    const char *word = RedisModule_StringPtrLen(argv[1], &len);

    // Search for the word in the Trie
    if (searchWord(trie_root, word)) {
        return RedisModule_ReplyWithSimpleString(ctx, "YES");
    } else {
        return RedisModule_ReplyWithSimpleString(ctx, "NO");
    }
}

int TriePrefixSearchCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 2) {
        return RedisModule_ReplyWithError(ctx, "ERR wrong number of arguments");
    }

    // Extract the prefix
    size_t len;
    const char *prefix = RedisModule_StringPtrLen(argv[1], &len);

    // Find the node corresponding to the prefix
    TrieNode *current = trie_root;
    for (size_t i = 0; i < len; i++) {
        int index = prefix[i] - 'a';
        if (current->children[index] == NULL) {
            return RedisModule_ReplyWithArray(ctx, 0); // Prefix not found
        }
        current = current->children[index];
    }

    // Collect all words starting from this node
    RedisModuleString *result[1024];
    char buffer[1024];
    int count = 0;
    strcpy(buffer, prefix);
    collectWordsWithPrefix(current, prefix, buffer, len, result, &count);

    // Reply with all matching words
    RedisModule_ReplyWithArray(ctx, count);
    for (int i = 0; i < count; i++) {
        RedisModule_ReplyWithString(ctx, result[i]);
        RedisModule_FreeString(ctx, result[i]);
    }

    return REDISMODULE_OK;
}

int TrieDeleteCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 2) {
        return RedisModule_ReplyWithError(ctx, "ERR wrong number of arguments");
    }

    // Extract the word
    size_t len;
    const char *word = RedisModule_StringPtrLen(argv[1], &len);

    if (trie_root == NULL) {
        return RedisModule_ReplyWithSimpleString(ctx, "NO");
    }

    // Attempt to delete the word
    if (deleteWord(trie_root, word, 0)) {
        return RedisModule_ReplyWithSimpleString(ctx, "OK");
    } else {
        return RedisModule_ReplyWithSimpleString(ctx, "NO");
    }
}

