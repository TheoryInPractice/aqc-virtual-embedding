#include "../include/oct_fast.h"

void compute_fast_oct_embed(Graph &program,
                            std::vector<int> &minimum_solution,
                            int seed,
                            int repeats,
                            bool verbose)
{
    srand(seed);
    std::vector<int> solution;
    std::vector<int> counter(program.get_n(), 0);

    for (int i = 0; i < repeats; i++)
    {
        fast_oct_embed(program, solution, verbose);
        if (solution.size() < minimum_solution.size() || minimum_solution.size() == 0)
        {
            minimum_solution = solution;
        }
        solution.clear();
    }

    if (verbose)
    {
        std::cout << "Solution size: " << minimum_solution.size() << std::endl;
        std::cout << "Solution verified: "
                  << verify_solution(program, minimum_solution)
                  << std::endl;
    }
}

/* Internal methods */
void fast_oct_embed(Graph &g, std::vector<int> &solution, bool verbose)
{
    /* Initialize the vertex list */
    for (int u = 0; u < g.get_n(); u++)
    {
        solution.push_back(u);
    }

    std::vector<int> partite;
    greedy_independent_set(solution, g, partite, verbose);
    for (int u : partite)
    {
        solution.erase(std::remove(solution.begin(), solution.end(), u),
                       solution.end());
    }

    partite.clear();
    greedy_independent_set(solution, g, partite, verbose);
    for (int u : partite)
    {
        solution.erase(std::remove(solution.begin(), solution.end(), u),
                       solution.end());
    }
}

void greedy_independent_set(std::vector<int> vertices,
                            Graph &g,
                            std::vector<int> &independent_set,
                            bool verbose)
{
    /* populate initial degrees vector */
    std::vector<int> degrees(g.get_n(), 0);
    for (int i = 0; i < g.get_n(); i++)
    {
        int u = g.get_ordering(i);
        for (int j = 0; j < g.get_n(); j++)
        {
            int v = g.get_ordering(j);
            degrees[u] += g.has_edge(u, v);
        }
    }

    /* populate vertex_removed vector */
    std::vector<int> vertex_removed(g.get_n(), 0);

    /* While vertices left in the graph, keep going */
    while (!vertices.empty())
    {
        /* find lowest degree vertex */
        int minimum_degree = std::numeric_limits<int>::max();
        std::vector<int> minimum_degree_vertices(g.get_n(), 0);

        for (int u : vertices)
        {
            if (degrees[u] < minimum_degree && degrees[u] > -1)
            {
                minimum_degree = degrees[u];
                minimum_degree_vertices.clear();
                minimum_degree_vertices.push_back(u);
            }
            else if (degrees[u] == minimum_degree)
            {
                minimum_degree_vertices.push_back(u);
            }
        }
        /* Pick a random min degree vertex */
        int minimum_degree_vertex = \
            minimum_degree_vertices[rand() % minimum_degree_vertices.size()];

        /* remove this vertex and its neighbors from the graph */
        vertex_removed[minimum_degree_vertex] = 1;
        vertices.erase(std::remove(vertices.begin(),
                                   vertices.end(),
                                   minimum_degree_vertex), vertices.end());
        degrees[minimum_degree_vertex] = -1;
        independent_set.push_back(minimum_degree_vertex);
        for (int v = 0; v < g.get_n(); v++)
        {
            if (g.has_edge(minimum_degree_vertex, v) && !vertex_removed[v])
            {
                vertex_removed[v] = 1;
                vertices.erase(std::remove(vertices.begin(),
                                           vertices.end(), v),
                               vertices.end());
                degrees[v] = -1;
                for (int nbr = 0; nbr < g.get_n(); nbr++)
                {
                    if (g.has_edge(v, nbr) && !vertex_removed[nbr])
                    {
                        degrees[nbr]--;
                    }
                }
            }
        }
    }
}
