"""Parsers of ISPD98 format (.net and .are files)
"""
import os
import sys

if sys.version <= "3.7":
    try:
        from collections.abc import OrderedDict
    except ImportError:
        from collections import OrderedDict
else:
    OrderedDict = dict

import networkx as nx
import matplotlib.pyplot as plt 

from .hypergraph import hyperedges2edges


def parse_are(file_path:str) -> OrderedDict:
    """Parse the .are file to get the area of each module
    :param file_path: path to the .are file
    :return: parsed nodes
    """
    assert isinstance(file_path, str), TypeError()

    nodes = OrderedDict()
    with open(file_path) as f:
        for line in f:
            args = str(line).split()
            assert len(args) == 2, "Invalid input: {}".format(line)
            
            name, area = args
            area = int(area)

            assert name not in nodes, "Duplicated node: {}".format(name)
            
            nodes[name] = {
                "area" : area
                # no other attribute in ISPD98
            }

    return nodes


def read_net_header(file_handle) -> dict:
    """Read the header of .net file.
    NOTE: it will start from the current pointer in the file
    :param file_handle: opened file handle 
    :return: a tuple of meta-data in the header
    """
    # header format
    # 1) num_pins :     number of pins, = \Sigma_i  #(v) if v in e_i
    #                   is the count of all omstamces in all nets
    # 2) num_nets:      number of nets
    # 3) num_instances: number of instances
    # 4) pad_offset:    offset of the pad

    # ignored line
    line = file_handle.readline()

    # number of pins
    line = file_handle.readline()
    num_pins = int(str(line))

    # number of nets
    line = file_handle.readline()
    num_nets = int(str(line))

    # number of modules
    line = file_handle.readline()
    num_instances = int(str(line))

    # pad offset
    line = file_handle.readline()
    pad_offset = int(str(line))

    return {
        "num_pins"      :   num_pins,
        "num_nets"      :   num_nets,
        "num_instances" :   num_instances,
        "pad_offset"    :   pad_offset,
    }


def read_net_netlist(file_handle, num_pins) -> list:
    """Read the nets of the .net file
    NOTE: it will start from the current pointer in the file
    :param file_handle: opened file handle 
    :return: a list of net objects
    """
    netlist = []

    # Scan the input file
    #
    # each data line should be in the following formats
    # (1) `<name> s 1` or (2) `<name> l`
    # (1) indicates a driver node in a uni-directional net
    # (2) indicates a load node in a uni-directional net
    #
    # NOTE: we use an OrderedDict for each net
    # key: name of connected node (str)
    # value: remaining part of each line unknown meaning
    curr_net = OrderedDict()
    for idx in range(num_pins):
        line = file_handle.readline()
        args = line.split()
        assert(len(args) == 2 or len(args) == 3)
        # start of a new net
        if ("s" == args[1]):
            # push the previous net (if not empty)
            if (len(curr_net) > 0):
                netlist.append(curr_net)
            # reset the current net
            curr_net = OrderedDict()
        # insert current node to the current net
        node = args[0]; attr = args[1 : ]
        curr_net[node] = attr
    # push the last nnet
    if (len(curr_net) > 0):
        netlist.append(curr_net)

    return netlist


def parse_net(file_path:str) -> list:
    """Parse the .net file to get a netlist and a header
    """

    with open(file_path) as f:
        # (1) read the header
        header = read_net_header(f)
        # (2) read all edges
        netlist = read_net_netlist(f, header["num_pins"])
        # sanity check
        num_nets = len(netlist)
        if (num_nets != header["num_nets"]):
            err_msg = ("In header, there should be %d nets,"
                       "while %d extracted."
                       % (header["num_nets"], num_nets))
            raise ValueError(err_msg)

    return {
        "netlist"   :   netlist,
        "header"    :   header
    }


def ispd98_netlist_to_hyperedges(netlist):
    """Convert ISPD98 netlist to hyperedges
    TODO: finalize the format for hyperedges
    """
    hyperedges = [list(net.keys()) for net in netlist]
    return hyperedges


def ispd98_to_nx(net_path:str, are_path:str) -> nx.Graph:
    """Parse ISPD98 file to NetworkX graph
    """
    G = nx.Graph()

    parsed_are = parse_are(are_path)
    parsed_net = parse_net(net_path)

    # add nodes with attributes to the graph
    # currently, only area attribute is available
    nodes = parsed_are
    G.add_nodes_from(nodes.items())

    # add edges with attributes to the graph
    netlist = parsed_net["netlist"]
    hyperedges = ispd98_netlist_to_hyperedges(netlist)
    edges = hyperedges2edges(hyperedges)
    for edge in edges:
        G.add_edge(edge["src"], edge["dst"], weight=edge["weight"])

    return G



if __name__ == "__main__":

    script_dir_path = os.path.dirname(__file__)

    circuit_dir = "ispd98"
    circuit_name = "sample"

    are_path = os.path.join(script_dir_path, circuit_dir,
                            circuit_name + ".are")
    net_path = os.path.join(script_dir_path, circuit_dir,
                            circuit_name + ".net")

    parsed_are = parse_are(are_path)
    print(parsed_are)

    parsed_net = parse_net(net_path)
    print(parsed_net)

    import matplotlib.pyplot as plt

    G = ispd98_to_nx(net_path, are_path)
    print(G.nodes())
    print(G.edges())

    fig, ax = plt.subplots()
    # do not display node details
    nx.draw(G, with_labels=True)
    plt.show()