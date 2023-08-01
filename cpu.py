

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



def node_name(module, id):
    return str(module + ':' + str(id))

def core_name(id):
    return node_name('core', id)

def cache_name(id):
    return node_name('cache', id)

def ddr_name(id):
    return node_name('ddr', id)

def io_name(id):
    return node_name('io', id)


def get_general_cpu(n_cores, n_caches, n_ddrs, n_ios):
    G = nx.Graph()
    for i in range(n_cores):
        G.add_node(core_name(i), module='core', module_id=0)
    for i in range(n_caches):
        G.add_node(cache_name(i), module='cache', module_id=1)
    for i in range(n_ddrs):
        G.add_node(ddr_name(i), module='ddr', module_id=2)
    for i in range(n_ios):
        G.add_node(io_name(i), module='io', module_id=3)
    return G


def get_xbar_cpu(n_cores=2, n_caches=2, n_ddrs=1, n_ios=1):
    # create G without connections
    G = get_general_cpu(n_cores=n_cores, n_caches=n_caches, n_ddrs=n_ddrs, n_ios=n_ios)

    # fixed node positions
    fixed_nodes = []
    fixed_positions = OrderedDict()

    # connect cores to caches
    for i in range(n_cores):
        for j in range(n_caches):
            G.add_edge(core_name(i),
                       cache_name(j),
                       weight=4.0)

    # initial set of fixed nodes
    for i in range(n_cores):
        fixed_nodes.append(core_name(i))
    for i in range(n_caches):
        fixed_nodes.append(cache_name(i))
    pos = nx.multipartite_layout(G, subset_key="module_id")
    fixed_positions = dict((v, pos[v]) for v in fixed_nodes)

    # connect cores to ddr
    for i in range(n_cores):
        for j in range(n_ddrs):
            G.add_edge(core_name(i),
                       ddr_name(j),
                       weight=1.0)
    # connect cores to IOs
    for i in range(n_cores):
        for j in range(n_ios):
            G.add_edge(core_name(i),
                       io_name(j),
                       weight=0.5)

    return G, fixed_positions



def get_ring_cpu(n_cores=8, n_ddrs=2, n_ios=2):
    # create G without connections
    G = get_general_cpu(n_cores=n_cores, n_caches=n_cores, n_ddrs=n_ddrs, n_ios=n_ios)

    # fixed node positions
    fixed_nodes = []
    fixed_positions = OrderedDict()

    # connect cores
    for i in range(n_cores):
        j = (i + 1) % n_cores
        G.add_edge(core_name(i),
                   core_name(j),
                   weight=2.0)    

    # initial set of fixed nodes
    for i in range(n_cores):
        fixed_nodes.append(core_name(i))
    pos = nx.spring_layout(G, seed=0)
    fixed_positions = dict((v, pos[v]) for v in fixed_nodes)

    # connect cores and caches
    for i in range(n_cores):
        G.add_edge(core_name(i),
                    cache_name(i),
                    weight=3.0)
    # connect cores to ddr
    for i in range(n_cores):
        for j in range(n_ddrs):
            G.add_edge(core_name(i),
                       ddr_name(j),
                       weight=1.0)
    # connect cores to IOs
    for i in range(n_cores):
        for j in range(n_ios):
            G.add_edge(core_name(i),
                       io_name(j),
                       weight=0.5)

    return G, fixed_positions


def get_mesh_cpu(n_cores_x=4, n_cores_y=4,
                 n_ddrs=4, n_ios=4):
    # create G without connections
    n_cores=n_cores_y * n_cores_x
    G = get_general_cpu(n_cores=n_cores,
                        n_caches=n_cores,
                        n_ddrs=n_ddrs,
                        n_ios=n_ios)

    # fixed node positions
    fixed_nodes = []
    fixed_positions = OrderedDict()

    # connect cores
    for j in range(n_cores_y):
        for i in range(n_cores_x - 1):
            G.add_edge(core_name(j * n_cores_x + i),
                    core_name(j * n_cores_x + i + 1),
                    weight=2.0)
    for j in range(n_cores_y - 1):
        for i in range(n_cores_x):
            G.add_edge(core_name(j * n_cores_x + i),
                    core_name((j + 1) * n_cores_x + i),
                    weight=2.0)
    
    # initial set of fixed nodes
    pos = nx.spring_layout(G, seed=0)
    for j in range(n_cores_y):
        for i in range(n_cores_x):
            fixed_nodes.append(core_name(j * n_cores_x + i))
    fixed_positions = dict((v, pos[v]) for v in fixed_nodes)

    # connect cores and caches
    for i in range(n_cores):
        G.add_edge(core_name(i),
                    cache_name(i),
                    weight=3.0)
    # connect cores to ddr
    for i in range(n_cores):
        for j in range(n_ddrs):
            G.add_edge(core_name(i),
                       ddr_name(j),
                       weight=1)
    # connect cores to IOs
    for i in range(n_cores):
        for j in range(n_ios):
            G.add_edge(core_name(i),
                       io_name(j),
                       weight=1)

    return G, fixed_positions





if __name__ == "__main__":
    FORMAT = "[%(filename)s:%(lineno)s - %(funcName)20s() ] %(message)s"
    logging.basicConfig(format=FORMAT, level=logging.INFO)

    
    G, fixed_positions = get_xbar_cpu()

    fig = plt.figure(figsize=(18, 10))
    ax = plt.axes()

    
    # pos = nx.multipartite_layout(G, subset_key="module_id")
    pos = nx.spring_layout(G, seed=0, pos=fixed_positions, fixed=fixed_positions.keys())
    # pos = nx.spectral_layout(G)
    
    plot_ele_nx(G, ax,
                pos=pos,
                node_color_keyword="module")

    plt.show()
    plt.pause(1)