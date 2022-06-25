"""Parsers of ISPD98 format (.net and .are files)
"""
import os
try:
    from collections.abc import OrderedDict
except ImportError:
    from collections import OrderedDict

import networkx as nx
import matplotlib.pyplot as plt 

from .hypergraph import hyperedges2edges


def parse_are_file(fpath:str) -> list:
    """Parse the .are file to get the area of each module
    :param fpath: path to the .are file
    :return: a list of nodes (dict with attr: name -> str, size -> float)
    """
    assert isinstance(fpath, str), TypeError()

    nodes = []
    with open(fpath) as f:
        for line in f:
            args = str(line).split()
            assert len(args) == 2, "Invalid input: {}".format(line)
            
            nodes.append({
                "name" : args[0],
                "area" : float(args[1])
                })

    return nodes


def parse_net_file(fname:str) -> list:
    """Parse the .net file to get a hyper-graph with connections only
    """

    def read_header(f):
        """read the header of .net file.
        It will start from the current pointer in the file
        :return: a tuple of meta-data in the header
        """
        # ignored line
        line = f.readline()
        # number of pins
        line = f.readline()
        pins_num = int(str(line))
        # number of nets
        line = f.readline()
        nets_num = int(str(line))
        # number of modules
        line = f.readline()
        modules_num = int(str(line))
        # pad offset
        line = f.readline()
        pad_offset = int(str(line))
        return (pins_num, nets_num, modules_num, pad_offset)

    def read_hyperedges(f, pins_num):
        # Note: we use an ordered dict for each hyperedge
        # key: name of node -> str
        # value: remaining part of each line unknown meaning -> list of str
        hyperedges = []; hyperedge_curr = OrderedDict()

        # scane the input file
        for idx in range(pins_num):
            line = f.readline()
            args = line.split()

            # each data line should be in the following formats
            # (1) <name> s 1
            # (2) <name> l
            # (1) indicates a driver node in a uni-directional net
            # (2) indicates a load node in a uni-directional net
            assert(len(args) == 2 or len(args) == 3)

            # start of a new hyper-edge
            if ("s" == args[1]):
                # push the previous hyper-edge
                if (len(hyperedge_curr) > 0):
                    hyperedges.append(hyperedge_curr)
                # clear the current hyper-edge
                hyperedge_curr = OrderedDict()

            # insert current node to the current hyperedge
            node_name = args[0]; node_attr = args[1 : ]
            hyperedge_curr[node_name] = node_attr

        # push the last hyper-edge
        if (len(hyperedge_curr) > 0):
            hyperedges.append(hyperedge_curr)

        return hyperedges

    pins_num = 0    # number of pins, = \Sigma_i  #(v) if v in e_i 
                    # is the count of all vertices in all hyper-edges
    nets_num = 0    # number of nets /hyper-edges
    modules_num = 0 # number of modules/vertices
    pad_offset = 0  # offset of the pad

    hyperedges = []

    with open(fname) as f:
        # read the header
        (pins_num, nets_num, modules_num, pad_offset) = read_header(f)
        
        # now, read all edges
        hyperedges = read_hyperedges(f, pins_num)
        
        # sanity check
        if (len(hyperedges) != nets_num):
            raise ValueError(
                "In header, there are {} nets, "
                "while extracted {}!".format(
                    nets_num, len(hyperedges)
                ))

    return {
        "hyperedges" : hyperedges,
        "header": {
            "pins_num" : pins_num,
            "nets_num" : nets_num,
            "modules_num" : modules_num,
            "pad_offset" : pad_offset
        }
    }



def parse_ispd98_to_networkx(net_fpath:str, are_fpath:str) -> nx.Graph:
    """
    """
    G = nx.Graph()

    # add nodes with attributes to the graph
    # (1) parse .are file
    parsed_are_file = parse_are_file(are_fpath)
    nodes = [
        # convert each node from dict to tuple
        (node["name"], node["area"])
        for node in parsed_are_file
    ]
    # (2) unzip attributes
    names, areas = zip(*nodes)
    # (3) add nodes to nx.Graph
    G.add_nodes_from(names, area=areas)

    # add edges with attributes to the graph
    # (1) parse .net file
    parsed_net_file = parse_net_file(net_fpath)
    hyperedges = parsed_net_file["hyperedges"]
    # (2) extract pure hyperedges with unit weight
    hyperedges = [list(hyperedge.keys()) for hyperedge in hyperedges]
    # (3) convert hyperedges to normal edges
    edges = hyperedges2edges(hyperedges)
    # (4) add edges to nx.Graph
    for edge in edges:
        G.add_edge(edge["src"], edge["dst"], weight=edge["weight"])

    return G



if __name__ == "__main__":

    script_dir_path = os.path.dirname(__file__)

    circuit_dir = "ispd98"
    circuit_name = "sample"

    are_fpath = os.path.join(script_dir_path, circuit_dir,
                             circuit_name + ".are")
    net_fpath = os.path.join(script_dir_path, circuit_dir,
                             circuit_name + ".net")

    parsed_are = parse_are_file(are_fpath)
    print(parsed_are)

    parsed_net = parse_net_file(net_fpath)
    print(parsed_net)

    import matplotlib.pyplot as plt

    G = parse_ispd98_to_networkx(net_fpath, are_fpath)
    print(G.nodes())
    print(G.edges())

    fig, ax = plt.subplots()
    # do not display node details
    nx.draw(G, with_labels=True)
    plt.show()