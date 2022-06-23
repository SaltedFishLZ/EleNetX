
import numpy as np
import networkx as nx
import matplotlib.pyplot as plt


def random_colored_graph(n, p, c, directed=False, seed=None):
    """Generate a random colored graph
    First we generate a random graph from Erdos-Renyi model.
    The graph we obtained might be unconnected.
    We will pick the largest connected component.
    Then, we will color the graph randomly using given numbers of colors.
    
    :param n: number o nodes in the Erdos-Renyi graph
    :param p: edge probability in the Erdos-Renyi graph
    :param c: number of colors
    :return: a NetworkX graph
    """
    # generate a random graph by Erdos-Renyi model
    G = nx.gnp_random_graph(n=n, p=p, seed=seed, directed=directed)
    
    # find the largest connected component (get a list of nodes)
    largest_cc = max(nx.connected_components(G), key=len)
    G = G.subgraph(largest_cc)
    
    # color the nodes with a uniform probability distribution
    rng = np.random.RandomState(seed=seed)
    for _v in G.nodes:
        G.nodes[_v]['color'] = rng.randint(0, c)

    return G



if __name__ == "__main__":
    
    n = 10; p = 0.25; c = 4
    random_seed = 1

    G = random_colored_graph(n=n, p=p, c=c,
                             seed=random_seed)

    # plot the figure
    fig, ax = plt.subplots()
    pos = nx.shell_layout(G)
    colors = [G.nodes[_v]["color"] for _v in G.nodes]
    print(colors)
    nx.draw(G, pos=pos, with_labels=True,
            node_color=colors, cmap=plt.cm.viridis)
    plt.show()