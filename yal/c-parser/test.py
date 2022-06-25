import sys
import json


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: test.py <name>.json")
    else:
        with open(sys.argv[1]) as f:
            string = f.read()
            bench = json.loads(string)
            print(len(bench))
