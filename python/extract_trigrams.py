#!/usr/bin/env python3
# extract_trigrams.py - Extracts top N trigrams from binary data

import argparse
import collections
import json
import sys

def extract_trigrams(path, max_count):
    freq = collections.Counter()
    with open(path, 'rb') as f:
        data = f.read()
    for i in range(len(data) - 2):
        a, b, c = data[i], data[i+1], data[i+2]
        freq[(a, b, c)] += 1

    top = freq.most_common(max_count)
    output = [{"x": a, "y": b, "z": c, "count": count} for ((a, b, c), count) in top]
    return output

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("file", help="Input binary file")
    parser.add_argument("--max", type=int, default=10000, help="Max number of trigrams to output")
    args = parser.parse_args()

    trigrams = extract_trigrams(args.file, args.max)
    with open("trigrams.json", "w") as f:
        json.dump(trigrams, f, indent=2)
    print(f"Wrote {len(trigrams)} trigrams to trigrams.json")
