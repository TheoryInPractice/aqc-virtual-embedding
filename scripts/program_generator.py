"""
Script for generating experiment input graphs.
"""


import networkx as nx
import os
import random
from utilities import Directory


# Graph generation methods

def _random_subset(seq, m):
    """Taken from NetworkX
    Given a sequence seq, return a ransom subset of size m.

    Parameters
    ----------
    seq : list
        The population
    m : int
        The sample size

    Returns
    -------
    set
        The sample from seq of size m
    """
    #    Copyright (C) 2004-2016 by
    #    Aric Hagberg <hagberg@lanl.gov>
    #    Dan Schult <dschult@colgate.edu>
    #    Pieter Swart <swart@lanl.gov>
    #    All rights reserved.
    #    BSD license.
    targets = set()
    while len(targets) < m:
        x = random.choice(seq)
        targets.add(x)
    return targets


def barabasi_albert_graph(n, m, seed=None):
    """Taken from NetworkX.
    The Barabasi-Albert graph generator.
    This function was modified to start with a non-empty graph.

    Parameters
    ----------
    n : int
        Number of nodes
    m : int
        The number of edges added per new node

    Returns
    -------
    NetworkX Graph
        The generated Barabasi-Albert graph
    """
    #    Copyright (C) 2004-2016 by
    #    Aric Hagberg <hagberg@lanl.gov>
    #    Dan Schult <dschult@colgate.edu>
    #    Pieter Swart <swart@lanl.gov>
    #    All rights reserved.
    #    BSD license.
    if m < 1 or m >= n:
        raise nx.NetworkXError(
            "Barabasi-Albert network must have m>=1 and m<n, m=%d,n=%d"
            % (m, n))
    if seed is not None:
        random.seed(seed)

    # Add m initial nodes (m0 in barabasi-speak)
    G = nx.gnp_random_graph(n=m, p=(float(m) / n), seed=seed)
    G.name = "barabasi_albert_graph(%s,%s)" % (n, m)
    # Target nodes for new edges
    targets = list(range(m))
    # List of existing nodes, with nodes repeated once for each adjacent edge
    repeated_nodes = []
    # Start adding the other n-m nodes. The first node is m.
    source = m
    while source < n:
        # Add edges to m nodes from the source.
        G.add_edges_from(zip([source] * m, targets))
        # Add one node to the list for each new edge just created.
        repeated_nodes.extend(targets)
        # And the new node "source" has m edges to add to the list.
        repeated_nodes.extend([source] * m)
        # Now choose m unique nodes from the existing nodes
        # Pick uniformly from repeated_nodes (preferential attachement)
        targets = _random_subset(repeated_nodes, m)
        source += 1
    return G


def random_noisy_bipartite_graph(n, p, seed):
    """Generates a base bipartite graph with evently split nodes.
    Adds a bipatite edge with probability p.
    Add every edge with probability 1/10.

    Parameters
    ----------
    n : int
        Number of nodes
    p : double
        Probability of bipartite edge
    seed : int
        Seed for pseudorandom number generator

    Returns
    -------
    NeworkX Graph
        The random noisy bipartite graph
    """
    random.seed(seed)
    noisy_p = p / 5
    G = nx.Graph()
    for i in range(n):
        G.add_node(i)
    for i in range(n):
        for j in range(i+1, n):
            # Random bipartite edge
            temp1 = random.random()
            if temp1 < p and (i - j) % 2 == 1:
                G.add_edge(i, j)
            # Noisy edge
            temp2 = random.random()
            if temp2 < noisy_p:
                G.add_edge(i, j)
    return G


# Graph write methods

def write_graph(G, graph_dir, seed):
    """Write graph G into a file in graph_dir,
    named using the seed G was generated with.

    Parameters
    ----------
    G : NetworkX Graph
        The graph to write
    graph_dir : string
        The filename/location to write the graph
    seed : int
        The seed used to generate G

    Returns
    -------
    Nothing
    """
    if not os.path.exists(graph_dir):
        os.makedirs(graph_dir)
    with open(os.path.join(graph_dir, "%d.graph") % (seed), "w") as outfile:
        outfile.write("%d\n" % G.order())
        for v in G.nodes():
            outfile.write("%d\n" % v)
        nx.write_edgelist(G, outfile, data=False)


def write_regular(n, density, seed=42):
    """Write a regular graph on n vertices with the specified density.

    Parameters
    ----------
    n : int
        Number of nodes
    density : str
        An edge density from "low", "medium", "high"
    seed : int
        Seed for pseudorandom number generator

    Returns
    -------
    Nothing
    """
    k = int(regular_density_conversion[density]*n)
    G = nx.random_regular_graph(d=k, n=n, seed=seed)
    graph_dir = os.path.join(Directory.INPUT_DIR, "reg_%d_%s" % (n, density))
    write_graph(G, graph_dir, seed)


def write_gnp(n, density, seed=42):
    """Write an Erdos-Reyni graph on n vertices with the specified density.

    Parameters
    ----------
    n : int
        Number of nodes
    density : str
        An edge density from "low", "medium", "high"
    seed : int
        Seed for pseudorandom number generator

    Returns
    -------
    Nothing
    """
    p = gnp_density_conversion[density]
    G = nx.gnp_random_graph(n, p, seed=seed)
    graph_dir = os.path.join(Directory.INPUT_DIR, "gnp_%d_%s" % (n, density))
    write_graph(G, graph_dir, seed)


def write_powerlaw(n, density, seed=42):
    """Write a Barabasi-Albert graph on n vertices with the specified density.

    Parameters
    ----------
    n : int
        Number of nodes
    density : str
        An edge density from "low", "medium", "high"
    seed : int
        Seed for pseudorandom number generator

    Returns
    -------
    Nothing
    """
    m = max(int(powerlaw_density_conversion[density] * n), 1)
    G = barabasi_albert_graph(n, m, seed=seed)
    graph_dir = os.path.join(Directory.INPUT_DIR,
                             "powerlaw_%d_%s" % (n, density))
    write_graph(G, graph_dir, seed)


def write_noisy_bipartite(n, density, seed=42):
    """Write a noisy bipartite graph on n vertices and the desired density.

    Parameters
    ----------
    n : int
        Number of nodes
    density : str
        An edge density from "low", "medium", "high"
    seed : int
        Seed for pseudorandom number generator

    Returns
    -------
    Nothing
    """
    p = noisy_bipartite_density_conversion[density]
    G = random_noisy_bipartite_graph(n=n, p=p, seed=seed)
    graph_dir = os.path.join(Directory.INPUT_DIR,
                             "nb_%d_%s" % (n, density))
    write_graph(G, graph_dir, seed)


def write_chimera(L, M, N):
    """Write a Chimera(L, M, N) graph.

    Parameters
    ----------
    L : int
        The Chimera cell height
    M : int
        The number of Chimera rows
    N : int
        The number of Chimera columns

    Returns
    -------
    Nothing
    """
    G = nx.Graph()

    cell_size = 2*L
    row_size = cell_size*N
    num_vertices = row_size*M
    for i in range(num_vertices):
        G.add_node(i)

    # Inner cell edges
    for cell in range(0, num_vertices, cell_size):
        for left_vertex in range(0, L):
            for right_vertex in range(L, cell_size):
                G.add_edge(cell+left_vertex, cell+right_vertex)

    # Vertical edges
    for column in range(0, N):
        for row in range(0, M-1):
            for vertex_index_in_cell in range(0, L):
                G.add_edge(column*(cell_size)
                           + row*(row_size)
                           + vertex_index_in_cell,
                           column*(cell_size)
                           + (row+1)*(row_size)
                           + vertex_index_in_cell)

    # Horizontal edges
    for column in range(0, N-1):
        for row in range(0, M):
            for vertex_index_in_cell in range(L, cell_size):
                G.add_edge(column*(cell_size)
                           + row*(row_size)
                           + vertex_index_in_cell,
                           (column+1)*(cell_size)
                           + row*(row_size)
                           + vertex_index_in_cell)

    graph_dir = os.path.join(Directory.INPUT_DIR, "hardware")
    if not os.path.exists(graph_dir):
        os.makedirs(graph_dir)
    with open(os.path.join(graph_dir, "chimera_%d_%d_%d.graph"
                           % (L, M, N)), "w") as outfile:
        outfile.write("%d %d \n" % (G.order(), nx.diameter(G)))
        for v in G.nodes():
            outfile.write("%d\n" % v)
        nx.write_edgelist(G, outfile, data=False)


# Lookup tables
generator_lookup = {"reg": write_regular,
                    "gnp": write_gnp,
                    "powerlaw": write_powerlaw,
                    "nb": write_noisy_bipartite}
regular_density_conversion = {"low": 0.25, "medium": 0.50, "high": 0.75}
gnp_density_conversion = {"low": 0.25, "medium": 0.50, "high": 0.75}
powerlaw_density_conversion = {"low": 0.25, "medium": 0.50, "high": 0.75}
hypercube_density_conversion = {"low": 0.3, "medium": 0.6, "high": 0.9}
noisy_bipartite_density_conversion = {"low": 0.25, "medium": 0.5, "high": 0.75}
