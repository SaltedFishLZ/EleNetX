"""Parsers of ISCAS85 format (.isc files)
"""
import os
try:
    from collections.abc import OrderedDict
except ImportError:
    from collections import OrderedDict

import networkx as nx
import matplotlib.pyplot as plt


NODE_TYPES = ["inpt", "and", "nand", "or", "nor", "xor", "xnor", "buff", "not", "from"]
LINE_TYPES = ["node", "fanin", "fanout"]

class iscNode:
    def __init__(self):
        self.id = None; self.name = None; self.type = None
        self.nfanout = 0; self.fanouts = []
        self.nfanin = 0; self.fanins = []

    def __repr__(self):
        return "[iscNode] id = {}, name = {}, type = {}, fan-ins = {}, fan-outs = {}".format(
            self.id, self.name, self.type,
            self.fanins, self.fanouts
        )


def parse_isc_file(fpath):
    
    # isc nodes contains normal circuit elements (nodes) and fanout nodes
    isc_nodes = []

    with open(fpath) as f:

        # control FSM
        last_line_type = curr_line_type = "node"
        remaining_fanin_lines = 0; remaining_fanout_lines = 0
        last_normal_node_id = None


        for line in f:
            # strip the line by white-space and tab
            words = line.strip().split()
            # print(words)

            # skip comments lines
            # NOTE: hope there is no in-line comments
            if "*" in words[0]:
                continue

            if "node" == curr_line_type:
                # print("CURR LINE:", curr_line_type)
                
                # extract node information
                # error properties ignored
                id, name, ntype, nfanout, nfanin = words[:5]
                id = int(id); nfanin = int(nfanin); nfanout = int(nfanout)

                assert (ntype in NODE_TYPES), TypeError("Invalid node type: {}".format(ntype))
                assert (nfanin >= 0), ValueError("Invalid # fanin {}".format(nfanin))
                assert (nfanout >= 0), ValueError()
                assert (id >= 0), ValueError()

                # assemble and append isc node
                node = iscNode()
                node.id = id; node.name = name; node.type = ntype
                node.nfanin = nfanin; node.fanins = []
                node.nfanout = nfanout; node.fanouts = []
                isc_nodes.append(node)
                last_normal_node_id = node.id

                # print(node)

                remaining_fanin_lines = 1 if nfanin > 0 else 0
                remaining_fanout_lines = nfanout

                last_line_type = "node"
                if (remaining_fanin_lines > 0):
                    curr_line_type = "fanin"
                elif (remaining_fanout_lines > 1):
                    curr_line_type = "fanout"
                else:
                    curr_line_type = "node"

                # print("NEXT LINE:", curr_line_type)

            elif "fanin" == curr_line_type:
                # print("CURR LINE:", curr_line_type)

                fanins = [int(id) for id in words]
                # print("fan=in:", fanins)

                # append fan-ins to the last isc_node
                isc_nodes[-1].fanins = fanins

                remaining_fanin_lines -= 1

                last_line_type = "fanin"
                if (remaining_fanout_lines > 1):
                    curr_line_type = "fanout"
                else:
                    curr_line_type = "node"

                # print("NEXT LINE:", curr_line_type)


            elif "fanout" == curr_line_type:
                # print("CURR LINE:", curr_line_type)

                # extract node information
                id, name, ntype, driver = words[:4]
                id = int(id)

                # NOTE: fanout information are redundant
                node = iscNode()
                node.id = id; node.name = name; node.type = ntype
                node.nfanin = 1; node.fanins = [last_normal_node_id]
                node.nfanout = 0; node.fanouts = []
                isc_nodes.append(node)

                # print(node)

                remaining_fanout_lines -= 1
                assert remaining_fanin_lines >= 0
                assert remaining_fanout_lines >= 0

                if (remaining_fanout_lines == 0):
                    last_line_type = "fanout"
                    curr_line_type = "node"
                else:
                    last_line_type = "fanout"
                    curr_line_type = "fanout"

                # print("NEXT LINE:", curr_line_type)
            else:
                raise ValueError("Invalid line type: {}".format(curr_line_type))


    return isc_nodes


def connect_isc_nodes(isc_node:list) -> tuple:
    """Transform isc nodes to a hypergraph.
    we assume that each netlist has only 1 driver
    """

    # auxilary data: nodes dict indexed by id
    nodes_dict = dict()

    for isc_node in isc_nodes:
        # initialize with empty fanouts
        nodes_dict[isc_node.id] = {
            "name" : isc_node.name,
            "fanins": isc_node.fanins,
            "fanouts" : isc_node.fanouts
        }
        # fanouts should be empty
        assert len(isc_node.fanouts) == 0, ValueError()

    # print(nodes_dict)

    # track fan-in to generate fan-out
    for isc_node in isc_nodes:
        for src in isc_node.fanins:
            driver = nodes_dict[src]
            driver["fanouts"].append(isc_node.id)
    
    # print(nodes_dict)

    # annoate fan-outs back
    for isc_node in isc_nodes:
        isc_node.fanouts = nodes_dict[isc_node.id]["fanouts"]



if __name__ == "__main__":

    script_dir_path = os.path.dirname(__file__)

    circuit_dir = os.path.join("iscas85", "isc")
    circuit_name = "c17"

    isc_path = os.path.join(circuit_dir, circuit_name + ".isc")

    isc_nodes = parse_isc_file(isc_path)
    print(isc_nodes)
    
    connect_isc_nodes(isc_nodes)
    print(isc_nodes)
