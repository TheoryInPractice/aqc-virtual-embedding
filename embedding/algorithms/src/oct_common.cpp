#include "../include/oct_common.h"

/*
Compute an s-t separating set by using shortest augmenting path max flow
and choosing appropriate vertices.
Populate sep and return its size
  n :            Number of vertices in the adjacency matrix
  adjMatrix :  Graph's adjacency matrix

*/
void compute_st_sep(Graph &g,
                    int subgraph_n,
                    std::vector<int> vertices,
                    std::vector<int> &left_partition,
                    std::vector<int> &right_partition,
                    std::vector<int> &sep)
{
    /* Make a new (n+2)x(n+2) graph with s,t */
    const int s = g.get_n();
    const int t = s+1;
    /* Graph on n+2 vertices */
    Graph flow_graph(t+1);

    /* Add source edges */
    for (int v : left_partition)
    {
        flow_graph.add_edge(s, v);
    }

    /* Add sink edges */
    for (int v : right_partition)
    {
        flow_graph.add_edge(v,t);
    }

    /* Copy everything else over */
    int u, v;
    for (int i = 0; i < subgraph_n; i++)
    {
        u = g.get_ordering(i);
        for (int j = 0; j < subgraph_n; j++)
        {
            v = g.get_ordering(j);
            flow_graph.set_edge(u, v, g.has_edge(u, v));
        }
    }

    vertices.push_back(s);
    vertices.push_back(t);

    std::vector<int> path;
    std::vector<int> s_vertices;
    std::vector<int> t_vertices;

    /* Compute the max flow residual graph */
    while (compute_st_path(flow_graph, vertices, s, t, path, s_vertices))
    {
        /* As we put flow on edges, we'll replace them with
           the residual edges. Given that this is a 0/1-capacity
           graph, we will have only (u,v) xor (v,u) at a given point
        */

        /* The path will be t-..-s */
        for (int v = 1; v < path.size(); v++)
        {
            u = v-1;
            flow_graph.add_edge(path.at(u), path.at(v));
            flow_graph.remove_edge(path.at(v), path.at(u));
        }
        s_vertices.clear();
        path.clear();
    }

    /* Compute the min cut for each vertex */
    for (int u : vertices) {
        /* if the vertex is not reachable by s, it's reachable by t */
        if (find(s_vertices.begin(), s_vertices.end(), u) == s_vertices.end())
        {
            t_vertices.push_back(u);
        }
    }

    /* for all edges t-reachable-vertex to s-reachable-vertex */
    for (int u : t_vertices)
    {
        for (int v : s_vertices)
        {
            if (flow_graph.has_edge(u, v))
            {
                if (v == s)
                {
                    sep.push_back(u);
                }
                else
                {
                    sep.push_back(v);
                }
            }
        }
    }
}


/*
Compute BFS to find an s-t path if it exists
Populate path
Assumes distance, parents are length vertices.size()
  n :                        Number of vertices in the adjacency matrix
  adjMatrix :             Graph's adjacency matrix
  vertices :               The set of vertices in the bipartite graph
  disance, parents :    The data we want from the BFS
*/
bool compute_st_path(Graph &g,
                     std::vector<int> &vertices,
                     int s,
                     int t,
                     std::vector<int> &path,
                     std::vector<int> &s_vertices)
{
     const int INFTY = std::numeric_limits<int>::max();
     const int NIL = -1;
     int *distance;
     int *parent;
     distance = new int[g.get_n()];
     parent = new int[g.get_n()];

     /* Init distance/parent */
    for (int v : vertices)
    {
        distance[v] = INFTY;
        parent[v] = NIL;
    }

    std::queue<int> q;
    int u;

    distance[s] = 0;
    q.push(s);

    while (!q.empty())
    {
        u = q.front();
        q.pop();
        s_vertices.push_back(u);
        for (int v : vertices)
        {
            if (g.has_edge(u,v) && distance[v] == INFTY)
            {
                if (v == t)
                {
                    path.push_back(v);
                    int internal_vertex = u;
                    while (internal_vertex != NIL)
                    {
                        path.push_back(internal_vertex);
                        internal_vertex = parent[internal_vertex];
                    }
                    delete[] distance;
                    delete[] parent;
                    return true;
                }
                else
                {
                    distance[v] = distance[u]+1;
                    parent[v] = u;
                    q.push(v);
                }
            }
        }
    }

    delete[] distance;
    delete[] parent;
    return false;
}

/*
  x :                            Int representing a partition when in base-2
  partition_initial :        The set that we're partitioning
  partition1/partition2 :   The sets we're partitioning into
*/
void compute_partition(int x,
                       std::vector<int> &partition_initial,
                       std::vector<int> &partition1,
                       std::vector<int> &partition2)
{
    int i = 0;
    for (int v : partition_initial)
    {
        int j = x >> i;
        if (j & 1)
        {
            partition1.push_back(v);
        }
        else
        {
            partition2.push_back(v);
        }
        i++;
    }
}

/*
  n :            Number of vertices in the adjacency matrix
  adjMatrix :  Graph's adjacency matrix
  vertices :   The set of vertices we're checking is an independent set
*/
bool is_independent(Graph &g, std::vector<int> &vertices)
{
    for (std::vector<int>::iterator it1 = vertices.begin();
         it1 != vertices.end();
         it1++)
    {
        for (std::vector<int>::iterator it2 = next(it1);
             it2 != vertices.end();
             it2++)
        {
            if (g.has_edge(*it1, *it2))
            {
                return false;
            }
        }
    }
    return true;
}

/*
Given a bipartite graph, partition its vertices into A union B
(Basically a BFS)
Assumption: We're putting all roots in A, but we could decide per component
  n :            Number of vertices in the adjacency matrix
  adjMatrix :  Graph's adjacency matrix
  vertices :   The set of vertices in the bipartite graph
  a,b :        The two partites
*/
void compute_bipartitions(Graph &g,
                          std::vector<int> &vertices,
                          std::vector<int> &a,
                          std::vector<int> &b)
{
    if (vertices.size() == 0)
    {
        return void();
    }

    int *distance;
    int *parent;
    distance = new int[g.get_n()];

    breadth_first_search(g, vertices, distance);

    // Divide vertices into a,b
    for (int v : vertices)
    {
        if (distance[v] % 2)
        {
            b.push_back(v);
        }
        else
        {
            a.push_back(v);
        }
    }

    delete[] distance;
}

/*
Compute BFS on the vector of vertices given to us
Populate distance, parents
Assumes distance, parents are length vertices.size()
  n :                        Number of vertices in the adjacency matrix
  adjMatrix :             Graph's adjacency matrix
  vertices :               The set of vertices in the bipartite graph
  disance, parents :    The data we want from the BFS
*/
void breadth_first_search(Graph &g, std::vector<int> &vertices, int *distance)
{
    const int INFTY = std::numeric_limits<int>::max();

    for (int v : vertices)
    {
        distance[v] = INFTY;
    }

    /* Note: we'll keep the indices in distance/parent/q
       and not the vertices themselves */
    bool done = false;
    std::queue<int> q;
    int start = *vertices.begin();
    int u;

    while (!done)
    {
        distance[start] = 0;
        q.push(start);

        while (!q.empty())
        {
            u = q.front();
            q.pop();
            for (int v : vertices)
            {
                if (g.has_edge(u,v) && distance[v] == INFTY)
                {
                    distance[v] = distance[u]+1;
                    q.push(v);
                }
            }
        }

        done = true;
        /* Find the root of a new component if one exists */
        for (int v : vertices)
        {
            if (distance[v] == INFTY)
            {
                start = v;
                done = false;
                continue;
            }
        }
    }
}

/*
Compute the set of neighbors that the vertices in source has in target
  n :                        Number of vertices in the adjacency matrix
  adjMatrix :             Graph's adjacency matrix
  source :               The source vertices
  target :               The target vertices
  neighbors :           The set of neighbors
*/
void neighbors(Graph &g,
               std::vector<int> &source,
               std::vector<int> &target,
               std::vector<int> &neighbors)
{
    for (int u : source)
    {
        for (int v : target)
        {
            if (g.has_edge(u, v))
            {
                neighbors.push_back(v);
            }
        }
    }
}

 /* Check if the graph induced on vertices is bipartite */
bool verify_solution(Graph &g, std::vector<int> solution)
{
    /* Compute the vertices in the bipartite graph */
    bool *in_solution;
    in_solution = new bool[g.get_n()]();
    for (int v : solution)
    {
        in_solution[v] = true;
    }

    std::vector<int> vertices;
    for (int i = 0; i < g.get_n(); i++)
    {
        if (!in_solution[i])
        {
            vertices.push_back(i);
        }
    }
    delete[] in_solution;

    const int INF = std::numeric_limits<int>::max();
    int *distance;
    distance = new int[g.get_n()];

    for (int v : vertices)
    {
        distance[v] = INF;
    }

    bool done = false;
    std::queue<int> q;
    int start = *vertices.begin();
    int u;

    while (!done)
    {
        distance[start] = 0;
        q.push(start);

        while (!q.empty())
        {
            u = q.front();
            q.pop();
            for (int v : vertices)
            {
                if (g.has_edge(u,v))
                {
                    if (distance[v] == INF)
                    {
                        distance[v] = distance[u]+1;
                        q.push(v);
                    } else if ((distance[v] % 2) == (distance[u] % 2))
                    {
                        delete[] distance;
                        return false;
                    }
                }
            }
        }

        done = true;
        /* Find the root of a new component if one exists */
        for (int v : vertices)
        {
            if (distance[v] == INF)
            {
                start = v;
                done = false;
                continue;
            }
        }
    }

    delete[] distance;
    return true;
}

/* Embedding */
void compute_embedding(Chimera &hardware,
                       std::vector<int> left_partition,
                       std::vector<int> right_partition,
                       Embedding &phi)
{
    int c = hardware.get_c();
    int n = hardware.get_n();
    int n_row = ceil((float)right_partition.size()/c);
    int n_col = ceil((float)left_partition.size()/c);

    int position;
    int jump = 2*c*n;
    for (int i = 0; i < left_partition.size(); i++)
    {
        position = (i/4*2*c)+(i%4);
        for (int j = 0; j < n_row; j++)
        {
            phi.add_vertex(left_partition.at(i), position+(j*jump));
        }
    }
    jump = 2*c;
    for (int i = 0; i < right_partition.size(); i++)
    {
        position = (i/4*2*c*n)+(i%4+4);
        for (int j = 0; j < n_col; j++)
        {
            phi.add_vertex(right_partition.at(i), position+(j*jump));
        }
    }
}


/* Given a Chimera(c,m) graph and an OCT decomposition of the program,
compute the embedding in the top-left corner of the hardware. */
void compute_embedding(Graph &program,
                       Chimera &hardware,
                       std::vector<int> solution,
                       Embedding &phi)
{
    /* Compute the vertices in the bipartite graph */
    bool *in_solution;
    in_solution = new bool[program.get_n()]();
    for (int v : solution)
    {
        in_solution[v] = true;
    }
    std::vector<int> vertices;
    for (int i = 0; i < program.get_n(); i++) {
        if (!in_solution[i])
        {
            vertices.push_back(i);
        }
    }
    delete[] in_solution;

    std::vector<int> left_partition;
    std::vector<int> right_partition;
    compute_bipartitions(program, vertices, left_partition, right_partition);
    left_partition.insert(left_partition.begin(),
                          solution.begin(),
                          solution.end());
    right_partition.insert(right_partition.begin(),
                           solution.begin(),
                           solution.end());

    int c = hardware.get_c();
    int n = hardware.get_n();
    int n_row = ceil((float)right_partition.size()/c);
    int n_col = ceil((float)left_partition.size()/c);

    int position;
    int jump = 2*c*n;
    for (int i = 0; i < left_partition.size(); i++)
    {
        position = (i/4*2*c)+(i%4);
        for (int j = 0; j < n_row; j++)
        {
            phi.add_vertex(left_partition.at(i), position+(j*jump));
        }
    }
    jump = 2*c;
    for (int i = 0; i < right_partition.size(); i++)
    {
        position = (i/4*2*c*n)+(i%4+4);
        for (int j = 0; j < n_col; j++)
        {
            phi.add_vertex(right_partition.at(i), position+(j*jump));
        }
    }
}
