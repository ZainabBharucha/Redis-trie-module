#include "redismodule.h"
#include "trie.h"
#include <string.h>

TrieNode *trie_root = NULL;

int TrieAddCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int TrieSearchCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int TriePrefixSearchCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int TrieDeleteCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int TrieWildcardSearchCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (RedisModule_Init(ctx, "trie", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    if (RedisModule_CreateCommand(ctx, "trie.add", TrieAddCommand, "write", 1, 1, 1) == REDISMODULE_ERR ||
        RedisModule_CreateCommand(ctx, "trie.search", TrieSearchCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR ||
        RedisModule_CreateCommand(ctx, "trie.prefix_search", TriePrefixSearchCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR ||
        RedisModule_CreateCommand(ctx, "trie.delete", TrieDeleteCommand, "write", 1, 1, 1) == REDISMODULE_ERR ||
        RedisModule_CreateCommand(ctx, "trie.wildcard_search", TrieWildcardSearchCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    return REDISMODULE_OK;
}

int TrieAddCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 3) {
        return RedisModule_ReplyWithError(ctx, "ERR wrong number of arguments");
    }

    size_t key_len, value_len;
    const char *key = RedisModule_StringPtrLen(argv[1], &key_len);
    const char *value = RedisModule_StringPtrLen(argv[2], &value_len);

    if (trie_root == NULL) {
        trie_root = createNode('\0'); 
    }

    addWord(trie_root, key, value);
    return RedisModule_ReplyWithSimpleString(ctx, "OK");
}

int TrieSearchCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 2) {
        return RedisModule_ReplyWithError(ctx, "ERR wrong number of arguments");
    }

    size_t key_len;
    const char *key = RedisModule_StringPtrLen(argv[1], &key_len);

    char *value = searchWord(trie_root, key);
    if (value) {
        return RedisModule_ReplyWithStringBuffer(ctx, value, strlen(value));
    } else {
        return RedisModule_ReplyWithNull(ctx);
    }
}


int TriePrefixSearchCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 2) {
        return RedisModule_ReplyWithError(ctx, "ERR wrong number of arguments");
    }

    size_t len;
    const char *prefix = RedisModule_StringPtrLen(argv[1], &len);

    TrieNode *current = trie_root;
    for (size_t i = 0; i < len; i++) {
        int index = prefix[i] - 'a';
        if (current->children[index] == NULL) {
            return RedisModule_ReplyWithArray(ctx, 0); 
        }
        current = current->children[index];
    }

    RedisModuleString *result[1024];
    char buffer[1024];
    int count = 0;
    strcpy(buffer, prefix);
    collectWordsWithPrefix(current, prefix, buffer, len, result, &count);

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

    size_t key_len;
    const char *key = RedisModule_StringPtrLen(argv[1], &key_len);

    if (trie_root == NULL) {
        return RedisModule_ReplyWithSimpleString(ctx, "NO");
    }

    if (deleteWord(trie_root, key, 0)) {
        return RedisModule_ReplyWithSimpleString(ctx, "OK");
    } else {
        return RedisModule_ReplyWithSimpleString(ctx, "NO");
    }
}

int TrieWildcardSearchCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 2) {
        return RedisModule_ReplyWithError(ctx, "ERR wrong number of arguments");
    }

    size_t pattern_len;
    const char *pattern = RedisModule_StringPtrLen(argv[1], &pattern_len);

    RedisModuleString *result[1024]; 
    char buffer[1024];
    int count = 0;

    wildcardSearch(trie_root, pattern, buffer, 0, result, &count);

    RedisModule_ReplyWithArray(ctx, count);
    for (int i = 0; i < count; i++) {
        RedisModule_ReplyWithString(ctx, result[i]);
        RedisModule_FreeString(ctx, result[i]);
    }

    return REDISMODULE_OK;
}



