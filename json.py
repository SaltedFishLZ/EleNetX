
import os
import sys
import json
import logging
import subprocess

if sys.version <= "3.7":
    try:
        from collections.abc import OrderedDict
    except ImportError:
        from collections import OrderedDict
else:
    OrderedDict = dict

import networkx as nx
import matplotlib as mpl
import matplotlib.pyplot as plt 

from .visualize import plot_ele_nx
from .mcnc import *


if __name__ == "__main__":
    
    FORMAT = "[%(filename)s:%(lineno)s - %(funcName)20s() ] %(message)s"
    logging.basicConfig(format=FORMAT, level=logging.INFO)

    for category in PD91_BENCHMARKS:
        for benchmark in PD91_BENCHMARKS[category]:
            if "yal" in PD91_BENCHMARKS[category][benchmark]["format"]:
                print("benchmark: ", benchmark)
                yal_path = os.path.join(PD91_BASE_DIR, category,
                                        benchmark + ".yal")

                G = yal_to_nx(yal_path)

                if benchmark == "struct":

                    jobj = nx.node_link_data(G)
                    jstring = json.dumps(jobj)
                    with open(benchmark + ".json", "w") as fout:
                        fout.write(jstring)
                    print(OrderedDict(G.nodes))