
import os
import sys
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

from .utils import obj_attr_cat_to_int


def empty_scatterplot_colormap_by_category(ax, cmap,
                                           categories:OrderedDict):
    """Scatter plot some empty points with given color map
    :param ax: a Matplotlib Axes object
    :param cmap: a Matplotlib color map
    :param categories: a map from categories to int indices
    """
    max_index = max(categories.values())
    for category in categories:
        ax.scatter([],[], label=category,
                   c=[cmap(categories[category] / max_index)])


def plot_ele_nx(graph:nx.Graph, ax:mpl.axes.Axes,
                node_color_keyword:str,
                node_color_map=plt.cm.viridis,
                layout:[str]=None, pos=None,
                with_labels:[str]=None,
                node_size=None):
    """Plot a electronic system represented in a NetworkX graph model
    :param graph: system graph
    :param ax: the Matplotlib subgraph we will plot on
    :param layout: NetworkX drawing layout
    :param node_color_keyword: the attribute keyword we use to color
    the nodes 
    :param node_color_map:
    :param node_size:
    """
    nodes = graph.nodes()
    num_nodes = len(nodes)


    if pos is None:
        # generate node layout (should be deterministic)
        if layout == "shell":
            pos = nx.shell_layout(graph)
        elif layout == "spectral":
            pos = nx.spectral_layout(graph)
        else:
            raise NotImplementedError()

    # node label
    if with_labels is None:
        with_labels = True if  num_nodes < 20 else False

    # node color
    color_indices = obj_attr_cat_to_int(nodes, node_color_keyword)
    node_colors = list(color_indices[graph.nodes[_v][node_color_keyword]]
                       for _v in graph.nodes)
    
    color_info = "{} nodes, {} colors".format(num_nodes, len(color_indices))
    logging.info(color_info)
    
    # node size
    if node_size is None:
        node_size = 100  if  num_nodes < 20 else 20

    # plot the main graph
    nx.draw_networkx(graph, ax=ax, pos=pos,
                     node_size=node_size,
                     node_color=node_colors,
                     with_labels=with_labels)    
    
    # make empty plot with correct color and label for each group
    empty_scatterplot_colormap_by_category(ax, node_color_map, color_indices)
    
    ax.legend(); ax.axis('off')
