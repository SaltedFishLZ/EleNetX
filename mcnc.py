"""MCNC Circuit Benchmark Suite

Current supported items:
- PDWorkshop 91
"""
__all__ = [
    "PD91_BASE_DIR", "PD91_BENCHMARKS", "PD91_TECHNOLOGIES",
    "yal_to_json_str", "pd91_yal_to_json_file",
    "parent_module_to_netlist",
    "remove_singleton_edge",
    "remove_duplicated_nodes",
    "parse_yal_file", "yal_to_nx",
    "obj_attr_cat_to_int",
    "empty_scatterplot_colormap_by_category",
    "plotting_test"
]

import os
import sys
import json
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

from .hypergraph import hyperedges2edges

DIR_PATH = os.path.dirname(__file__)

YAL2JSON_REL_PATH = os.path.join("yal", "c-parser", "yal2json")
YAL2JSON_ABS_PATH = os.path.join(DIR_PATH, YAL2JSON_REL_PATH)

PD91_BASE_DIR = os.path.join(DIR_PATH, "mcnc", "pd91", "bench")

PD91_BENCHMARKS = {
    # block design
    "block" : {
        "ami33" : {
            "format" : ["yal", "vpnr"],
            "technology" : "ami"
        },
        "ami49" : {
            "format" : ["yal", "vpnr"],
            "technology" : "ami"
        },
        "apte" : {
            "format" : ["yal", "vpnr"],
            # TODO: technology
        },
        "hp" : {
            "format" : ["yal", "vpnr"],
            "technology" : "ami"
        },
        "xerox" : {
            "format" : ["yal", "vpnr"],
            "technology" : "xerox"
        },
    },
    
    # floorplan
    "floorplan" : {
        "fan" : {
            "format" : ["yal", "vpnr"],
            # self-contained technology
        },
        # TODO: xerox
    },
    
    # mixed
    "mixed" : {
        "a3" : {
            "format" : ["yal", "vpnr"],
            # self-contained technology
        },
        "g2" : {
            "format" : ["yal", "vpnr"],
            # self-contained technology
        },
        "t1" : {
            "format" : ["yal", "vpnr"],
            # self-contained technology
        },
    },

    # std cell (benchmark only, no tech lib)
    "stdcell" : {
        "biomed" : {
            "format" : ["yal", "vpnr"],
            "technology" : "db"
        },
        "fract" : {
            "format" : ["yal", "vpnr"],
            "technology" : "db"
        },
        "industry1" : {
            "format" : ["yal", "vpnr"],
            # self-contained technology
        },
        "industry2" : {
            "format" : ["yal", "vpnr"],
            # self-contained technology
        },
        "industry3" : {
            "format" : ["yal", "vpnr"],
            # self-contained technology
        },
        # TODO: primary1 and primary2
        # need fix the YAL c parser to parse HEIGHT and WIDTH
        # "primary1" : {
        #     "format" : ["yal", "vpnr"],
        #     "technology" : "sclib"
        # },
        # "primary2" : {
        #     "format" : ["yal", "vpnr"],
        #     "technology" : "sclib"
        # },
        "struct" : {
            "format" : ["yal", "vpnr"],
            "technology" : "db"
        },
    }
}

PD91_TECHNOLOGIES = {
    "stdcell" : {
        "db" : {
            "format" : ["yal", "vpnr"],
        },
        "sclib" : {
            "format" : ["yal", "vpnr"],
        },
        # TODO: gate array
        # need fix the YAL c parser to parse HEIGHT and WIDTH
    }
}


def yal_to_json_str(src_yal_path:str) -> str:
    """Convert a YAL file to JSON string
    :param src_yal_path: path to the source YAL file
    :return: a JSON string obtained by yal2json
    """
    assert os.path.exists(src_yal_path), ValueError("YAL not found")
    result = subprocess.run([YAL2JSON_ABS_PATH, src_yal_path],
                            capture_output=True, text=True)
    # TODO: error handling
    return result.stdout


def pd91_yal_to_json_file(base_dir:str):
    """Batch processing utility function: convert all PD91 benchmarks 
    from YAL files into JSON files
    """
    # translate PD91 benchmark YAL files
    for category in PD91_BENCHMARKS:
        for benchmark in PD91_BENCHMARKS[category]:
            if "yal" in PD91_BENCHMARKS[category][benchmark]["format"]:
                print("=" * 64)
                print("translating {} from yal to json".format(benchmark))
                print("=" * 64)
                src_yal_path = os.path.join(PD91_BASE_DIR, category,
                                            benchmark + ".yal")
                dst_json_path = os.path.join(PD91_BASE_DIR, category,
                                             benchmark + ".json")
                subprocess.run([
                    YAL2JSON_ABS_PATH,
                    src_yal_path,
                    dst_json_path
                ])


def parent_module_to_netlist(module:dict) -> (OrderedDict, OrderedDict):
    """Extract netlist in a hyper-graph form from the top module dict
    :param module: input top-level flattened design
    :return: (vertices, hyperedges)
    vertices is an OrderedDict of instances, key=name, value=attributes
    hyperedges is OrderedDict of connections, key=name, value=[instance names]
    """
    # input module should be a parent/top module
    assert "ModType" in module, TypeError()
    assert "Network" in module, TypeError()
    assert (module["ModType"] == "PARENT"), TypeError()
    
    instances = OrderedDict()
    netlist = OrderedDict()

    # (1) extract netlist connection information
    # as well as instance type (which module)
    for instance in module["Network"]:
        instance_name = instance["instancename"]
        module_name = instance["modulename"]
        # insert instance with attributes
        assert instance_name not in instances
        instances[instance_name] = {
            "modulename" : module_name,
            # TODO: other attributes
        }
        # insert signals or update signals with connected instances 
        for signal in instance["signalnames"]:
            if (signal not in netlist) or (netlist[signal] is None):
                # insert the signal with initalization
                netlist[signal] = [instance_name, ]
            else:
                # update connected instances for the signal
                netlist[signal].append(instance_name)
    
    return instances, netlist


def remove_singleton_edge(hyperedges:OrderedDict) -> OrderedDict:
    """Remove signal edge which has only 1 instance connected (e.g., I/O)
    :param hyperedges: input hyperedges (NOTE: will be mutated)
    :return: mutated hyperedges
    """
    # NOTE: we cannot mutate the OrderedDict when iterating it
    # so we maintain old keys and keys to be deleted
    original_ids = hyperedges.keys()
    singleton_ids = []
    for _id in original_ids:
        if len(hyperedges[_id]) <= 1:
            singleton_ids.append(_id)
    for _id in singleton_ids:
        del hyperedges[_id]
    return hyperedges


def remove_duplicated_nodes(hyperedges:OrderedDict, verbose=False):
    """Remove duplicated nodes in the netlist to make sure that each 
    unique node appears only once in each net.
    :param netlist: input netlist, a list of nodes
    :return: output netlist after duplication removal
    """
    _no_dup = OrderedDict()
    
    for key in hyperedges:
        origin_hyperedge = hyperedges[key]
        unique_hyperedge = list(OrderedDict.fromkeys(origin_hyperedge))
        _no_dup[key] = unique_hyperedge

        if (verbose):
            if len(unique_hyperedge) < len(origin_hyperedge):
                num_dup_nodes = len(origin_hyperedge) - len(unique_hyperedge)
                print(key, "has %d duplicated nodes" % num_dup_nodes)

    return _no_dup


def parse_yal_file(yal_path:str) -> dict:
    """Parse YAL file and get vertices and hyperedges
    :param yal_path: path to the YAL file
    :return: parsed dict {vertices, hyperedges}
    """
    vertices = None; hyperedges = None

    # get the parsed JSON string of the YAL file
    json_str = yal_to_json_str(yal_path)
    # load json string to a list of Python dict
    modules = json.loads(json_str)
    
    # search for top modules
    num_top_modules = 0
    for module in modules:
        if (module["ModType"] == "PARENT"):
            num_top_modules += 1
            vertices, hyperedges = parent_module_to_netlist(module)

    # only 1 parent/top module is allowed in a benchmark
    assert num_top_modules == 1, ValueError("More than one top module found")

    return {
        "vertices"      :   vertices,
        "hyperedges"    :   hyperedges,
    }


def yal_to_nx(yal_path:str, verbose=False) -> nx.Graph:
    """Parse YAL file and obtain a correpsonding NetworkX graph
    """
    # parse YAL file to get a dict
    results = parse_yal_file(yal_path)

    # use OrderedGraph to make plot reproduciable
    if sys.version >= "3.7":
        G = nx.Graph()
    else:
        G = nx.OrderedGraph()

    # extract and add nodes
    nodes = results["vertices"]
    G.add_nodes_from(nodes.items())
    # sanity check
    for _v in G.nodes:
        assert G.nodes[_v] == nodes[_v]

    # extract hyperedges
    hyperedges = results["hyperedges"]

    # remove duplicated nodes in each net
    if verbose:
        print("Removing duplicated nodes...")
    hyperedges = remove_duplicated_nodes(hyperedges)
    
    # remove singleton hyperedges/edges
    if verbose:
        print("Removing singleton hyperedge...")
    hyperedges = remove_singleton_edge(hyperedges)

    # transform hyperedges to edges
    if verbose:
        print("Transforming hyperedges to edges...")
    hyperedges = hyperedges.values()
    edges = hyperedges2edges(hyperedges)

    for edge in edges:
        G.add_edge(edge["src"], edge["dst"], weight=edge["weight"])

    if verbose:
        print("NetworkX graph constructed")

    return G


def obj_attr_cat_to_int(objects:OrderedDict, category:str) -> OrderedDict:
    """Map objects' categorical attributes to int
    :param objects: all objects to be processed
    :param category: category keyword in attributes
    :param: a map from category keyword to int
    """
    categories = [objects[_id][category] for _id in objects]
    int_map = OrderedDict.fromkeys(categories)
    # fill in color id by enumeration
    for i, c in enumerate(int_map):
        int_map[c] = i
    return int_map


def empty_scatterplot_colormap_by_category(ax, cmap, categories:OrderedDict):
    """Scatter plot some empty points with given color map
    :param ax:
    :param cmap:
    :param categories: a map from categories to int indices
    """
    max_index = max(categories.values())
    for category in categories:
        ax.scatter([],[], label=category,
                   c=[cmap(categories[category] / max_index)])


def plotting_test(G:nx.Graph, verbose=False):
    num_nodes = len(G.nodes)
    num_edges = len(G.edges)
    if (verbose):
        print("Num of nodes: ", num_nodes)
        print("Num of edges: ", num_edges)

    with_labels = True if  num_nodes < 20 else False
    node_size = 100  if  num_nodes < 20 else 20

    # get module colors & colormap
    nodes = G.nodes()
    module_indices = obj_attr_cat_to_int(nodes, "modulename")
    node_colors = [module_indices[G.nodes[_v]["modulename"]] for _v in G.nodes]
    if verbose:
        if (len(module_indices) < len(nodes)):
            print("%d nodes, %d kinds of modules"
                  % (len(nodes), len(module_indices)))
            print("module cm", module_indices)
    cmap = plt.cm.viridis

    # plot with shell layout
    fig = plt.figure(figsize=(18, 10))
    ax = plt.axes()
    pos = nx.shell_layout(G) # deterministic node layout 
    nx.draw_networkx(G, ax=ax, pos=pos,
                     node_size=node_size,
                     node_color=node_colors,
                     with_labels=with_labels)    
    # make empty plot with correct color and label for each group
    empty_scatterplot_colormap_by_category(ax, cmap, module_indices)
    plt.legend(); plt.axis('off')
    plt.show()

    # plot with spectral layout
    fig = plt.figure(figsize=(18, 10))
    ax = plt.axes()
    pos = nx.spectral_layout(G)
    nx.draw_networkx(G, ax=ax, pos=pos,
                     node_size=node_size,
                     node_color=node_colors,
                     with_labels=with_labels)
    # make empty plot with correct color and label for each group
    empty_scatterplot_colormap_by_category(ax, cmap, module_indices)
    plt.legend(); plt.axis('off')
    plt.show()


if __name__ == "__main__":
    
    for category in PD91_BENCHMARKS:
        for benchmark in PD91_BENCHMARKS[category]:
            if "yal" in PD91_BENCHMARKS[category][benchmark]["format"]:
                print("benchmark: ", benchmark)
                yal_path = os.path.join(PD91_BASE_DIR, category,
                                        benchmark + ".yal")

                G = yal_to_nx(yal_path)

                if benchmark == "struct":
                    plotting_test(G)