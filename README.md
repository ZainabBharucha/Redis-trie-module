
# Trie Module for Redis

This project is a Redis extension that introduces Trie (prefix tree) functionality, enabling efficient prefix-based key-value operations. The module is implemented as a Redis Module using the Redis Modules SDK.

---

## Features

The Trie module supports the following operations:

1. **TRIE.ADD**: Adds a key-value pair to the Trie.

   TRIE.ADD <key> <value>
   ```
   Example:
   ```bash
   TRIE.ADD hello world
   ```

2. **TRIE.SEARCH**: Searches for a key and returns its associated value.
   ```bash
   TRIE.SEARCH <key>
   ```
   Example:
   ```bash
   TRIE.SEARCH hello
   ```

3. **TRIE.PREFIX_SEARCH**: Retrieves all key-value pairs that match a given prefix.
   ```bash
   TRIE.PREFIX_SEARCH <prefix>
   ```
   Example:
   ```bash
   TRIE.PREFIX_SEARCH hel
   ```

4. **TRIE.DELETE**: Removes a key-value pair from the Trie.
   ```bash
   TRIE.DELETE <key>
   ```
   Example:
   ```bash
   TRIE.DELETE hello
   ```

5. **TRIE.WILDCARD_SEARCH**: Searches for keys using wildcard patterns (`*`, `?`).
   ```bash
   TRIE.WILDCARD_SEARCH <pattern>
   ```
   Example:
   ```bash
   TRIE.WILDCARD_SEARCH h*l?o
   ```

---

## Installation

### Prerequisites
- Redis 6.0 or higher installed on your system.
- GCC or another C compiler.
- Redis Modules SDK (comes with Redis).

### Steps to Compile and Run

1. Clone the repository:
   ```bash
   git clone https://github.com/<your-repo>/Redis-trie-module.git
   cd Redis-trie-module
   ```

2. Build the module:
   ```bash
   make clean
   make
   ```

   Ensure the `trie_module.so` file is generated in the project directory.

3. Start the Redis server and load the module:
   ```bash
   redis-server --loadmodule /path/to/trie_module.so --port <port>
   ```

4. Test the module using the Redis CLI:
   ```bash
   redis-cli -p <port>
   TRIE.ADD hello world
   TRIE.SEARCH hello
   ```

---

## Benchmarks

The Trie module has been benchmarked for performance compared to Redis' native HSET commands:

- **TRIE.ADD**: Faster for prefix-based operations compared to HSET.
- **TRIE.SEARCH**: Optimized for key-value retrieval with prefix compression.
- **TRIE.PREFIX_SEARCH**: Efficient traversal for large datasets.

---

## Use Cases

- **Autocomplete**: Suggest words or items based on a given prefix.
- **Pattern Matching**: Search keys using wildcard patterns.
- **Recommendation Systems**: Retrieve suggestions based on shared prefixes.
- **Efficient Storage**: Compress shared prefixes for memory efficiency.

---

## Development

### Project Structure
- **`src/module.c`**: Implements Redis command handlers and module initialization.
- **`src/trie.c`**: Core Trie data structure and logic for add, search, delete, and traversal operations.
- **`redismodule.h`**: Redis Modules API header file.
- **`Makefile`**: Build instructions for the project.

### Extending the Module
- Add new commands by updating `src/module.c` and implementing their logic in `src/trie.c`.
- Rebuild the module using:
   ```bash
   make clean
   make
   ```

---

## Testing

1. Start the Redis server with the module loaded.
2. Run the included benchmark script:
   ```bash
   python redis_test.py
   ```
3. Example results:
   ```
   TRIE.ADD latency for 10,000 items: 0.25 seconds
   TRIE.SEARCH latency for 10,000 items: 0.22 seconds
   ```

---

## Future Enhancements

- Add support for case-insensitive searches.
- Enable persistent storage for Trie data.
- Batch operations for adding or deleting multiple keys at once.

---
