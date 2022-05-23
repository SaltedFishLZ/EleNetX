"""Parsers of ISPD98 format (.net and .are files)
"""
import os


def parse_are_file(fpath):
    """Parse the .are file to get the area of each module
    :param fpath: path to the .are file
    :return: a list of nodes (dict with attr: name -> str, size -> float)
    """
    nodes = []
    with open(fpath) as f:
        for line in f:
            args = str(line).split()
            assert len(args) == 2, "Invalid input: {}".format(line)
            
            nodes.append({"name" : args[0], "size" : float(args[1])})

    return nodes


def parse_net_file(fname):
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
        # we use a dict for each hyperedge
        # key: name of node -> str
        # value: unknown meaning -> str
        hyperedges = []; hyperedge_curr = {}

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
                hyperedge_curr = {}

            # insert current node to the current hyperedge
            node_name = args[0]; node_weight = args[-1]
            hyperedge_curr[node_name] = node_weight

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


def ispd98_networkx():
    pass



if __name__ == "__main__":

    script_dir_path = os.path.dirname(__file__)

    circuit_dir = "ispd98"
    circuit_name = "sample"

    vertices = parse_are_file(os.path.join(script_dir_path,
                                            circuit_dir,
                                            circuit_name + ".are"))
    print(vertices)

    parsed_net = parse_net_file(os.path.join(script_dir_path,
                                            circuit_dir,
                                            circuit_name + ".net"))
    print(parsed_net)