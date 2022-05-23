try:
    from collections.abc import Sequence
except ImportError:
    from collections import Sequence


def hyperedge2edges(hyperedge:Sequence, weight:float=1.0, model="clique") -> list:
    """Convert a single hyperedge to a list of normal edges
    :param hyperedge: a hyper edge (a Python Sequence of node:str)
    :param weight: weight of the hyperedge
    :return: a list of weighted undirected edges -> list of dict
    """
    assert isinstance(hyperedge, Sequence), TypeError("Type {} is not Sequence".format(type(hyperedge)))

    n_nodes = len(hyperedge); assert n_nodes > 1, ValueError()

    edges = []
    for i1 in range(n_nodes):
        for i2 in range(i1 + 1, n_nodes):
            edges.append({
                "src" : hyperedge[i1],
                "dst" : hyperedge[i2],
                "weight" : weight / (n_nodes - 1)
            })
    
    return edges


def hyperedges2edges(hyperedges:Sequence, weight:float=1.0, model="clique") -> list:
    """Convert hyperedges to edges
    """
    edges = []
    for hyperedge in hyperedges:
        edges += hyperedge2edges(hyperedge, weight=weight, model=model)
    return edges






if __name__ == "__main__":
    edges = hyperedge2edges(["1", "2", "3"])
    print(edges)