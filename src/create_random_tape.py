import random

size = 1020*4 # 0.5mb of data

with open("../cmake-build-debug/tape.bin", 'wb') as f:
    for _ in range(int(size/4)):
        f.write((random.randint(0,2**16)).to_bytes(4, "little"))
