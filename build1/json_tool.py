import sys
import json

with open(sys.argv[1], "r") as f_in:
    with open(sys.argv[1]+".json", "w") as f_out:
        data = eval(f_in.read())
        json.dump(data, f_out, indent=4)
