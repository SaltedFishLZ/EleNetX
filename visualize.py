
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


def empty_scatterplot_colormap_by_category(ax, node_cmap,
                                           categories:OrderedDict):
    """Scatter plot some empty points with given color map
    :param ax: a Matplotlib Axes object
    :param node_cmap: a Matplotlib color map
    :param categories: a map from categories to int indices
    """
    cints = categories.values()
    norm = mpl.colors.Normalize(vmin=min(cints), vmax=max(cints))
    for category in categories:
        cint = categories[category]
        ax.scatter([],[], label=category,
                   c=[node_cmap(norm(cint))])


def autocolor_nodes(G:nx.Graph,
                    color_by:[str]=None,
                    cmap=plt.cm.viridis):
    """Color nodes in a graph according to a field
    Will cast v[color_by] from a category to a int first,
    then cast to a color by cmap
    NOTE: mutate the input graph
    """
    nodes = G.nodes()
    num_nodes = len(nodes)
    # default nx node color
    for _v in nodes:
        nodes[_v]["color"] = "#1f78b4"

    # auto color all nodes by category
    if color_by is not None:
        # get color index for each category
        cat_int_map = obj_attr_cat_to_int(nodes, color_by)
        cints = cat_int_map.values()
        norm = mpl.colors.Normalize(vmin=min(cints), vmax=max(cints))
        # add node color attr
        for _v in nodes:
            _v_cint = cat_int_map[nodes[_v][color_by]]
            nodes[_v]["color"] = cmap(norm(_v_cint))
        # log
        color_info = "{} nodes, {} colors".format(num_nodes, len(cat_int_map))
        logging.info(color_info)    


def automark_nodes(G:nx.Graph,
                   mark_by:[str]=None,
                   markers='osv^<>phPX+*'):
    """
    NOTE: mutate the input graph
    """
    nodes = G.nodes()
    num_nodes = len(nodes)
    # default nx node marker
    for _v in nodes:
        nodes[_v]["marker"] = 'o'

    # auto mark all nodes by category
    if mark_by is not None:
        # get marker index for each category
        cat_int_map = obj_attr_cat_to_int(nodes, mark_by)
        assert(len(cat_int_map) <= len(markers)), ValueError("Num categories > Num markers!")
        # add node marker attr
        for _v in nodes:
            _v_mint = cat_int_map[nodes[_v][mark_by]]
            nodes[_v]["marker"] = markers[_v_mint]
        # log
        color_info = "{} nodes, {} markers".format(num_nodes, len(cat_int_map))
        logging.info(color_info)    



def color_ele_nx_nodes(graph:nx.Graph,
                       node_autocolor_by:str=None,
                       node_color_map:OrderedDict=None,
                       node_cmap=plt.cm.viridis):
    """
    NOTE: mutate the original graph
    """
    nodes = graph.nodes()
    num_nodes = len(nodes)

    autocolor_nodes(graph, color_by=node_autocolor_by, cmap=node_cmap)
    automark_nodes(graph, mark_by="color")
    for v in nodes:
        print(nodes[v]['marker'])

    # overriding rules
    if node_color_map is not None:
        assert isinstance(node_color_map, OrderedDict), TypeError()
        for _v in node_color_map:
            nodes[_v]["color"] = node_color_map[_v]


def plot_ele_nx(graph:nx.Graph, ax:mpl.axes.Axes,
                node_size=None,
                layout:[str]=None, pos=None,
                with_labels:[str]=None,
                node_color_map:[OrderedDict]=None,
                node_autocolor_by:[str]=None,
                node_cmap=plt.cm.viridis,
                *args, **kwargs):
    """Plot a electronic system represented in a NetworkX graph model
    :param graph: system graph
    :param ax: the Matplotlib subgraph we will plot on
    :param layout: NetworkX drawing layout
    :param node_autocolor_by: the attribute keyword we use to color
    the nodes 
    :param node_color_map: manually specified color for certain nodes.
    it is used to override the automatic node color setting
    :param node_size:
    """
    show_legend = False

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

    # obtain node color if existing
    node_color = None
    # get node color from existing attr
    color_attrs = nx.get_node_attributes(graph, "color")
    if len(color_attrs) == num_nodes:
        node_color = list(color_attrs.values())
    # color nodes by rules
    if (node_autocolor_by is not None or 
        node_color_map is not None):
        # color nodes
        color_ele_nx_nodes(graph,
                           node_autocolor_by=node_autocolor_by,
                           node_color_map=node_color_map)
        node_color = [nodes[_v]["color"] for _v in nodes]    
        # make empty plot with correct color and label for each group
        cat_int_map = obj_attr_cat_to_int(nodes, node_autocolor_by)
        empty_scatterplot_colormap_by_category(ax, node_cmap, cat_int_map)
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
