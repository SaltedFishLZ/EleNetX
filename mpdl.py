"""Mixed-Precision Deep Learning SoC benchmark Suite
"""
__all__ = ["assign_modules"]

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


def assign_modules(G:nx.Graph):
    """Visit all nodes and assign modulename to each node
    NOTE: mutable
    """
    for i in G.nodes:
        v = G.nodes[i]
        hw_configs = v['hw_configs']
        name = hw_configs['name']
        prec = hw_configs['input_dtype']
        # we have make the "module" hasable
        # so we use tuples
        if (name == 'ShiDianNao'):
            v['module'] = (
                ('name', name),
                # ('prec', prec)
            )
        elif (name == 'TPU') or (name == 'Gemmini'):
            v['module'] = (
                ('name', 'TPU'),
                # ('prec', prec)
            )
        elif (name == 'Eyeriss'):
            v['module'] = (
                ('name', name),
                # ('prec', prec)
            )
        else:
            raise ValueError()
    return G


if __name__ == "__main__":
    FORMAT = "[%(filename)s:%(lineno)s - %(funcName)20s() ] %(message)s"
    logging.basicConfig(format=FORMAT, level=logging.INFO)

    script_dir_path = os.path.dirname(__file__)

    graphs = []
    gnames = []

    for i in range(10, 13, 2):

        # bfile = 'new_resnet50_pareto_{}.pkl'.format(i)
        # print(bfile)

        # gpath = os.path.join(script_dir_path,
        #                     "mpdl", "pkl",
        #                     "resnet50", bfile)

        bfile = 'new_mobilenetv2_pareto_{}.pkl'.format(i)
        print(bfile)

        gpath = os.path.join(script_dir_path,
                            "mpdl", "pkl", "sample", "v0.2",
                            "mobilenetv2", bfile)
        G = nx.read_gpickle(gpath)

        assign_modules(G)

        graphs.append(G)
        gnames.append(r'$G^{(' + str(i) + r')}$')
    
    G = nx.union_all(graphs, rename=[gname + ":" for gname in gnames])
    print(G.nodes)

    print(nx.number_connected_components(G))

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