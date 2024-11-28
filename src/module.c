#include "redismodule.h"
#include "trie.h"

// Declare the global Trie root node
TrieNode *trie_root = NULL;

// Forward declaration for the search command
int TrieSearchCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

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

    return REDISMODULE_OK;
}
