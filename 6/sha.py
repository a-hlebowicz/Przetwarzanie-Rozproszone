import asyncio
import hashlib
import itertools
import sys
import time
CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()"
num_tasks = 8
async def crack_range(prefixes, target_hash, length):
    for prefix in prefixes:
        for combination in itertools.product(CHARS, repeat=length - 1):
            candidate = prefix + ''.join(combination)
            hashed = hashlib.sha256(candidate.encode()).hexdigest()
            if hashed == target_hash:
                return candidate
    return None

async def main():
    if len(sys.argv) != 3:
        print("blad")
        return
    target_hash = sys.argv[1]
    length = int(sys.argv[2])
    start_time = time.time()
    total_prefixes = list(itertools.product(CHARS, repeat=1))#pierwszy znak
    step = len(total_prefixes) // num_tasks  #ile na proces
    tasks = []
    for i in range(num_tasks):
        start = i * step
        end = (i + 1) * step if i != num_tasks - 1 else len(total_prefixes)
        prefix_group = [''.join(p) for p in total_prefixes[start:end]]
        tasks.append(asyncio.create_task(crack_range(prefix_group, target_hash, length)))
    results = await asyncio.gather(*tasks)
    end_time = time.time()
    for result in results:
        if result:
            print(f"Hasło: {result}")
            break
    else:
        print("Hasło nie zostało znalezione")

    print(f"Czas: {end_time - start_time:.2f} s")
    return

if __name__ == "__main__":
    asyncio.run(main())
