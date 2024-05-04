#!/usr/bin/env python3

from itertools import chain
import subprocess
import re

# Mazākais un lielākais chunks/sizes faila id
MIN_N = 1
MAX_N = 19

subprocess.run(["make", "-q"]) # apklusinām make, lai nepiemēslotu stdout

# Iegūst relatīvo atrašanās vietu chunks/size failam
n_to_chunks = lambda n: f"test_files/{n:03}chunks"
n_to_sizes =  lambda n: f"test_files/{n:03}sizes"

lines = []

for i in range(MIN_N, MAX_N + 1):
    proc = subprocess.Popen(["./a.out", "-c", n_to_chunks(i), "-s", n_to_sizes(i)],
                            stdout=subprocess.PIPE)
    lines.append(proc.stdout.read())


# [:2], jo pēdējais (trešais) elements ir tukšs
lines = map(lambda l: str(l, "utf-8").split("\n")[:2], lines)
lines = chain(*lines) # saplacina sarakstu
lines = list(lines)

table_keys = ["used", "unused", "unalloc", "ticks"]
line_regex = r"\"(.+)\" (\d+) (\d+) (\d+) (\d+)" # algo name + table_keys

res = {}

for l in lines:
    m = re.search(line_regex, l)
    data_list = res.get(m.group(1), [])
    data_list.append(m.groups()[1:])
    res[m.group(1)] = data_list


for algo in res.keys():
    print(f"--- {algo} ---")
    print(*([" id"] + [x.rjust(8) for x in table_keys]))
    for i, row in enumerate(res[algo]):
        print(str(i + MIN_N).rjust(3, '0'), end=" ")
        for item in row:
            print(item.rjust(8), end=" ")
        print("")
    print("")