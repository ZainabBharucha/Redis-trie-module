import time
import redis

# Connect to Redis
client = redis.Redis(host='localhost', port=6381, decode_responses=True)

# Benchmark TRIE.ADD
start = time.time()
# for i in range(10000):  # Adjust the range for larger datasets
#     client.execute_command("TRIE.ADD", f"key{i}", f"value{i}")
# end = time.time()
# print("TRIE.ADD latency for 10,000 items:", end - start)

# # Benchmark TRIE.SEARCH
# start = time.time()
# for i in range(10000):
#     client.execute_command("TRIE.SEARCH", f"key{i}")
# end = time.time()
# print("TRIE.SEARCH latency for 10,000 items:", end - start)
TRIE.ADD latency for 10,000 items: 0.2531242370605469
TRIE.SEARCH latency for 10,000 items: 0.22985076904296875

HSET latency for 10,000 items: 0.2665598392486572
HGET latency for 10,000 items: 0.24653959274291992

# Benchmark HSET
start = time.time()
for i in range(10000):
    client.hset("hash", f"key{i}", f"value{i}")
end = time.time()
print("HSET latency for 10,000 items:", end - start)

# Benchmark HGET
start = time.time()
for i in range(10000):
    client.hget("hash", f"key{i}")
end = time.time()
print("HGET latency for 10,000 items:", end - start)

