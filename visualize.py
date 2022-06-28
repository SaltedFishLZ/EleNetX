
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
    cints = categories.values()
    norm = mpl.colors.Normalize(vmin=min(cints), vmax=max(cints))
    for category in categories:
        cint = categories[category]
        ax.scatter([],[], label=category,
                   c=[cmap(norm(cint))])


def color_ele_nx_nodes(graph:nx.Graph,
                       node_color_by:str=None,
                       node_color_map:OrderedDict=None,
                       cmap=plt.cm.viridis):
    """
    NOTE: mutate the original graph
    """
    nodes = graph.nodes()
    num_nodes = len(nodes)

    # default node color
    for _v in nodes:
        nodes[_v]["color"] = "#1f78b4"

    # color all nodes by category
    if node_color_by is not None:
        # get color index for each category
        cat_int_map = obj_attr_cat_to_int(nodes, node_color_by)
        cints = cat_int_map.values()
        norm = mpl.colors.Normalize(vmin=min(cints), vmax=max(cints))

        for _v in nodes:
            _v_cint = cat_int_map[nodes[_v][node_color_by]]
            nodes[_v]["color"] = cmap(norm(_v_cint))

        color_info = "{} nodes, {} colors".format(num_nodes, len(cat_int_map))
        logging.info(color_info)

    # overriding rules
    if node_color_map is not None:
        for _v in node_color_map:
            nodes[_v]["color"] = node_color_map[_v]


def plot_ele_nx(graph:nx.Graph, ax:mpl.axes.Axes, layout:str,
                node_size=None,
                node_color_by:str=None,
                node_color_map:OrderedDict=None,
                cmap=plt.cm.viridis,
                *args, **kwargs):
    """Plot a electronic system represented in a NetworkX graph model
    :param graph: system graph
    :param ax: the Matplotlib subgraph we will plot on
    :param layout: NetworkX drawing layout
    :param node_color_by: the attribute keyword we use to color
    the nodes 
    :param node_color_map: manually specified color for certain nodes.
    it is used to override the automatic node color setting
    :param node_size:
    """
    show_legend = False

    nodes = graph.nodes()
    num_nodes = len(nodes)

    # node layout (should be deterministic)
    if layout == "shell":
        pos = nx.shell_layout(graph)
    elif layout == "spectral":
        pos = nx.spectral_layout(graph)
    elif layout == "spring":
        pos = nx.spring_layout(graph)
    else:
        raise NotImplementedError()

    # node label
    with_labels = True if num_nodes < 20 else False

    # node color
    node_color = None
    # get node color from attr
    color_attrs = nx.get_node_attributes(graph, "color")
    if len(color_attrs) == num_nodes:
        node_color = list(color_attrs.values())
    # color nodes by rules
    if (node_color_by is not None or 
        node_color_map is not None):
        # color nodes
        color_ele_nx_nodes(graph,
                           node_color_by=node_color_by,
                           node_color_map=node_color_map)
        node_color = [nodes[_v]["color"] for _v in nodes]    
        # make empty plot with correct color and label for each group
        cat_int_map = obj_attr_cat_to_int(nodes, node_color_by)
        empty_scatterplot_colormap_by_category(ax, cmap, cat_int_map)
        show_legend = True

    # node size
    if node_size is None:
        node_size = 100  if  num_nodes < 20 else 20

    # plot the main graph
    nx.draw_networkx(graph, ax=ax, pos=pos,
                     node_size=node_size,
                     node_color=node_color,
                     with_labels=with_labels,
                     *args, **kwargs)    


    if show_legend:
        ax.legend()
    
    ax.axis('off')

    return ax
