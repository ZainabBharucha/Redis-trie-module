import time
import redis
import random

# Connect to Redis
client = redis.Redis(host='localhost', port=6381, decode_responses=True)

# Benchmark TRIE.ADD
start = time.time()
for i in range(10000):  # Adjust the range for larger datasets
    client.execute_command("TRIE.ADD", f"key{i}", f"value{i}")
end = time.time()
print("TRIE.ADD latency for 10,000 items:", end - start)

# # Benchmark TRIE.SEARCH
start = time.time()
for i in range(10000):
    client.execute_command("TRIE.SEARCH", f"key{i}")
end = time.time()
print("TRIE.SEARCH latency for 10,000 items:", end - start)

list = []
with open("words_alpha.txt") as fp:
    while (True):
        line = fp.readline()
        if not line:
            break
        list.append(line)

print("Test Case Length | HSET_ADD Time         | HSET_SEARCH Time      | TRIE_ADD Time         | TRIE_SEARCH Time")
print(("-" * 17) + "+" + ("-" * 23) + "+" + ("-" * 23) + "+" + ("-" * 23) + "+" + ("-" * 23))

test_case_length_list = [10, 100, 1000, 10000]
for test_case_length in test_case_length_list:
    test_list = random.sample(list, k = 2 * test_case_length)
    key_list = test_list[:test_case_length]
    value_list = test_list[test_case_length:]

    start = time.time()
    for i in range(test_case_length):
        client.hset("hash", f"{key_list[i]}", f"{value_list[i]}")
    end = time.time()
    hset_add_time = end - start
    # print(f"HSET latency for {test_case_length} items: {hset_add_time}")

    start = time.time()
    for i in range(test_case_length):
        client.hget("hash", f"{key_list[i]}")
    end = time.time()
    hset_search_time = end - start
    # print(f"HSET latency for {test_case_length} items: {hset_search_time}")

    start = time.time()
    for i in range(test_case_length):
        client.execute_command("TRIE.ADD", f"{key_list[i]}", f"{value_list[i]}")
    end = time.time()
    trie_add_time = end - start
    # print(f"TRIE latency for {test_case_length} items: {trie_add_time}")

    start = time.time()
    for i in range(test_case_length):
        client.execute_command("TRIE.SEARCH", f"{key_list[i]}")
    end = time.time()
    trie_search_time = end - start
    # print(f"TRIE latency for {test_case_length} items: {trie_search_time}\n")

    print(f"{test_case_length}" + (" " * (16 - len(str(test_case_length)))) + " | " + f"{hset_add_time}" + (" " * (21 - len(str(hset_add_time)))) + " | " + f"{hset_search_time}" + (" " * (21 - len(str(hset_search_time)))) + " | " + f"{trie_add_time}" + (" " * (21 - len(str(trie_add_time)))) + " | " + f"{trie_search_time}" + (" " * (21 - len(str(trie_search_time)))))