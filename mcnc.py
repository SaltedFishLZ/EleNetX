
import os
import json
import subprocess
try:
    from collections.abc import OrderedDict
except ImportError:
    from collections import OrderedDict

import networkx as nx
import matplotlib.pyplot as plt 

from hypergraph import hyperedges2edges



PD91_BASE_DIR = os.path.join("mcnc", "pd91", "bench")

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
    YAL2JSON_TRANSLATOR_PATH = os.path.join("yal", "c-parser", "yal2json")
    result = subprocess.run([YAL2JSON_TRANSLATOR_PATH, src_yal_path],
                            capture_output=True, text=True)
    # TODO: error handling
    return result.stdout

def pd91_yal_to_json_file(base_dir:str):
    """Convert all PD91 benchmarks from YAL files into JSON files
    """
    YAL2JSON_TRANSLATOR_PATH = os.path.join("yal", "c-parser", "yal2json")
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
                    YAL2JSON_TRANSLATOR_PATH,
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
    
    # remove singleton nets in netlist
    netlist = remove_singleton_net(netlist)
    
    return instances, netlist

def remove_singleton_net(netlist:OrderedDict) -> OrderedDict:
    """Remove signal net which has only 1 instance connected (e.g., I/O)
    :param netlist: input netlist (NOTE: will be mutated)
    """
    # NOTE: we cannot mutate the OrderedDict when iterating it
    # so we maintain old keys and keys to be deleted
    original_net_names = netlist.keys()
    singleton_net_names = []
    for net in original_net_names:
        if len(netlist[net]) <= 1:
            singleton_net_names.append(net)
    for net in singleton_net_names:
        del netlist[net]
    return netlist

def parse_yal_file(modules:list) -> dict:
    """Parse YAL file and get 
    TODO: output json to stdout pipe instead of a file so that a benchmark
    with different input formats (e.g., .vpnr) won't have file conflicts
    :return: parsed dict {vertices, hyperedges}
    """
    vertices = None; hyperedges = None

    # search for top modules
    num_top_modules = 0
    for module in modules:
        if (module["ModType"] == "PARENT"):
            num_top_modules += 1
            vertices, hyperedges = parent_module_to_netlist(module)
            # remove singleton nets
            hyperedges = remove_singleton_net(hyperedges) 

    # only 1 parent/top module is allowed in a benchmark
    assert num_top_modules == 1, ValueError("More than one top module found")

    return {
        "vertices"      :   vertices,
        "hyperedges"    :   hyperedges,
    }


if __name__ == "__main__":
    # pd91_yal_to_json_file(base_dir=PD91_BASE_DIR)
    
    for category in PD91_BENCHMARKS:
        for benchmark in PD91_BENCHMARKS[category]:
            if "yal" in PD91_BENCHMARKS[category][benchmark]["format"]:
                print("benchmark: ", benchmark)
                src_yal_path = os.path.join(PD91_BASE_DIR, category,
                                            benchmark + ".yal")
                json_path = os.path.join(PD91_BASE_DIR, category,
                                         benchmark + ".json")
                
                json_str = yal_to_json_str(src_yal_path)

                modules = json.loads(json_str)
                results = parse_yal_file(modules)

                if benchmark == "struct":
                    # use OrderedGraph to make plot reproduciable
                    G = nx.OrderedGraph()

                    nodes = results["vertices"].keys()
                    G.add_nodes_from(nodes)
                    # print(G.nodes())

                    hyperedges = results["hyperedges"]
                    # print(hyperedges)
                    hyperedges = hyperedges.values()
                    edges = hyperedges2edges(hyperedges)
                    for edge in edges:
                        G.add_edge(edge["src"], edge["dst"], weight=edge["weight"])
                    
                    # print(G.edges())

                    fig, ax = plt.subplots()
                    # make node layout deterministic
                    pos = nx.spectral_layout(G)
                    nx.draw(G, pos=pos, with_labels=True)
                    plt.show()