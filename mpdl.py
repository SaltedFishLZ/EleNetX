"""Mixed-Precision Deep Learning SoC benchmark Suite
"""
__all__ = [
    "compose_graph_path",
    "assign_modules",
    "MPDL_BENCHMARKS"
]

MPDL_BENCHMARKS = {
    'mobilenetv2' : {
        'prefix'    :   'mobilenetv2_pareto',
        'num_cfg'   :   1,
    },
    'resnet18' : {
        'prefix'    :   'resnet18_pareto',
        'num_cfg'   :   1,
    },
    'resnet50' : {
        'prefix'    :   'resnet50_pareto',
        'num_cfg'   :   1,
    },
    'yolov5' : {
        'prefix'    :   'yolov5_pareto',
        'num_cfg'   :   1,
    },
}

import os
import sys
import pickle
import logging

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


def compose_graph_path(neural_net:str, id:int) -> str:
    script_dir_path = os.path.dirname(__file__)
    # get file name prefix
    prefix = MPDL_BENCHMARKS[neural_net]['prefix']
    # compose file name
    bench_file = prefix + '_{}.pkl'.format(id)
    # compose the entire path
    gpath = os.path.join(script_dir_path,
                         "mpdl", "pkl", "sample",
                         "v1.0",
                         neural_net, bench_file)
    return gpath


def assign_modules(G:nx.Graph):
    """Visit all nodes and assign modulename to each node
    NOTE: mutable
    """
    for i in G.nodes:
        v = G.nodes[i]
        hw_configs = v['hw_configs']
        name = hw_configs['name']
        v['module'] = name
    return G


if __name__ == "__main__":
    FORMAT = "[%(filename)s:%(lineno)s - %(funcName)20s() ] %(message)s"
    logging.basicConfig(format=FORMAT, level=logging.INFO)

    graphs = []
    gnames = []

    for neural_net in MPDL_BENCHMARKS.keys():
        num_cfg = MPDL_BENCHMARKS[neural_net]['num_cfg']
        # enumerate configs
        for i in range(num_cfg):

            gpath = compose_graph_path(neural_net, i)
            print(gpath)

            with open(gpath, 'rb') as fin:
                G = pickle.load(fin)
            # G = nx.read_gpickle(gpath)

            assign_modules(G)

            graphs.append(G)
            gnames.append(neural_net + ':' + str(i))
    
    G = nx.union_all(graphs, rename=[gname + ":" for gname in gnames])
    print(G.nodes)

    print("=" * 64)
    print('number of connected components:',
          nx.number_connected_components(G))

    fig = plt.figure(figsize=(18, 10))
    ax = plt.axes()

    pos = nx.spring_layout(G, seed=0)
    from networkx.drawing.nx_agraph import graphviz_layout
    pos = graphviz_layout(G)
    plot_ele_nx(G, ax,
                pos=pos,
                node_color_keyword="module")

    plt.show()
    plt.pause(1)