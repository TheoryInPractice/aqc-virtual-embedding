#include "../include/cmr.h"

void compute_cmr_dijkstra(Hardware &g,
                          Graph &h,
                          CMR_Embedding &phi,
                          int seed,
                          int tries,
                          bool verbose)
{
    srand(seed);
    h.shuffle_ordering();
    bool done = find_minor_embedding(g, h, phi, verbose);
    tries--;
    while (!done && tries >= 0)
    {
        h.shuffle_ordering();
        phi.clear();
        done = find_minor_embedding(g, h, phi, verbose);
        tries--;
    }
    if (!done)
    {
        phi.clear();
    }
}

bool find_minor_embedding(Hardware &g,
                          Graph &h,
                          CMR_Embedding &phi,
                          bool verbose)
{
    int old_condition_1 = std::numeric_limits<int>::max();
    int old_condition_2 = std::numeric_limits<int>::max();
    int new_condition_1 = std::numeric_limits<int>::max();
    int new_condition_2 = std::numeric_limits<int>::max();

    /* set stage := 1 */
    int *weight = new int[g.get_n()];
    int stage = 1;

    /* while condition1 or condition2 is improving, or stage <= 2 */
    while (stage <= 2 ||
           old_condition_1 > new_condition_1 ||
           old_condition_2 > new_condition_2)
    {
        old_condition_1 = new_condition_1;
        old_condition_2 = new_condition_2;

        /* for i in {1,...,n} do */
        for (int bag_index = 0; bag_index < h.get_n(); bag_index++)
        {
            int bag = h.get_ordering(bag_index);

            if (verbose)
            {
                std::cout << "Embedding bag " << bag << std::endl;
                std::cout << "   Non-1 weights:" << std::endl;
            }

            phi.clear_bag(bag);

            /* for g in V(G) do */
            for (int vertex = 0; vertex < g.get_n(); vertex++)
            {
                /* set w(g) := diam(G)^{number of vertex models != phi(xi)
                that g is in} */
                weight[vertex] = weight_computation(g, phi, bag, vertex);
                if (verbose)
                {
                    if (weight[vertex] != 1)
                    {
                        std::cout << "   weight of "
                                  << vertex
                                  << ": "
                                  << weight[vertex]
                                  << std::endl;
                    }
                }
            }

            find_minimal_vertex_model(g, h, phi, weight, bag, verbose);
        }

        new_condition_1 = phi.get_max_number_of_bags();
        new_condition_2 = phi.get_qubits_used();
        stage++;
    }

    delete[] weight;
    return phi.is_valid();
}

void find_minimal_vertex_model(Hardware &g,
                               Graph &h,
                               CMR_Embedding &phi,
                               int *weight,
                               int current_bag,
                               bool verbose)
{
    const int NIL = -1;
    /* if all phi(xj) are empty */
    /*   return random {g*} */

    bool no_neighbors_embedded = true;
    for (int neighbor = 0; neighbor < h.get_n(); neighbor++)
    {
        if (h.has_edge(current_bag, neighbor) && !phi.is_bag_empty(neighbor))
        {
            no_neighbors_embedded = false;
            break;
        }
    }

    if (no_neighbors_embedded) {
        int temp = rand();
        if (verbose)
        {
            std::cout << "   No neighbors, generated random: "
                      << temp
                      << " "
                      << temp % g.get_n()
                      << std::endl;
        }
        int root_vertex = (int) (temp % g.get_n());
        phi.add_vertex(current_bag, root_vertex);
        if (verbose) {
            std::cout << "   Bag: " << root_vertex << std::endl;
        }
        return;
    }

    int *cost = new int[g.get_n() * h.get_n()]();
    std::vector<std::vector<int>> path(g.get_n()*h.get_n(), std::vector<int>());

    /* for all g in V(G) and all j do */
    for (int vertex = 0; vertex < g.get_n(); vertex++)
    {
        for (int bag = 0; bag < h.get_n(); bag++)
        {
            if (h.has_edge(current_bag, bag))
            {
                if (phi.is_bag_empty(bag))
                {
                    /* set cost[vertex, bag]=0, which is already done */
                }
                else if (phi.contains(bag, vertex))
                {
                    cost[bag*g.get_n()+vertex] = weight[vertex];
                }
                else
                {
                    cost[bag*g.get_n()+vertex] = shortest_path_distance(g, h,
                        phi, weight, path, vertex, bag);
                }
            }
        }
    }

    /* set g* := argmin_g sum_j c(g,j) */
    int root_vertex = lowest_cost_root(g, h, cost);
    int index;

    /* phi(xi) = {g*} union {paths from g* to each phi(xj)} */
    phi.add_vertex(current_bag, root_vertex);
    for (int bag = 0; bag < h.get_n(); bag++)
    {
        for (int vertex : path[bag*g.get_n()+root_vertex])
        {
            phi.add_vertex(current_bag, vertex);
        }
    }

    if (verbose) {
        std::cout << "   Bag: ";
        for (int i = 0; i < g.get_n(); i++)
        {
            if (phi.contains(current_bag, i))
            {
                std::cout << i << " ";
            }
        }
        std::cout << std::endl;
    }

    delete[] cost;
}


 /* Utility functions: */
int shortest_path_distance(Hardware &g,
                           Graph &h,
                           CMR_Embedding &phi,
                           int *weight,
                           std::vector<std::vector<int>> &path,
                           int source_vertex,
                           int bag)
{
    const int NIL = -1;

    int closest_vertex = NIL;
    int closest_vertex_distance = std::numeric_limits<int>::max();

    int *distance = new int[g.get_n()]();
    int *parent = new int[g.get_n()]();

    /* Compute the distances using Dijkstra (don't maintain the paths) */
    dijkstra(g, phi, weight, distance, parent, source_vertex);

    /* Search for smallest distance from all vertices in phi(xj) */
    for (int i = 0; i < g.get_n(); i++)
    {
        /* if i in phi(xj) */
        if (phi.contains(bag, i))
        {
            if (distance[i] < closest_vertex_distance)
            {
                closest_vertex = i;
                closest_vertex_distance = distance[i];
            }
        }
    }

    /* We don't include the closest_vertex because we don't
       want bags to intersect */
    int path_index = bag*g.get_n()+source_vertex;
    int current_vertex = parent[closest_vertex];
    while (current_vertex != source_vertex)
    {
        path[path_index].push_back(current_vertex);
        current_vertex = parent[current_vertex];
    }

    delete[] distance;
    delete[] parent;
    return closest_vertex_distance;
}

/* dijkstra with path list */
void dijkstra(Hardware &g,
              CMR_Embedding &phi,
              int *weight,
              int *dist,
              int *prev,
              int source)
{
    int vertices_examined = 0;
    int *visited = new int[g.get_n()]();

    /* Initialize distances */
    for (int i = 0; i < g.get_n(); i++)
    {
        dist[i] = std::numeric_limits<int>::max();
    }
    dist[source] = 0;

    /* While we haven't compute all distances */
    while (vertices_examined < g.get_n())
    {
        int u = minimum_distance(visited, dist, g.get_n());
        visited[u] = 1;
        vertices_examined++;

        /* for each unvisited neighbor i of u: */
        for (int i = 0; i < g.get_n(); i++)
        {
            if (visited[i] == 0 && g.has_edge(u, i))
            {
                /* The shortest xj -> g path using head weights
                is equivalent to the shortest g -> xj path using tail weights.
                This also excludes w(phi(xj)) as required. */
                int alt = dist[u] + weight[u];
                if (alt < dist[i])
                {
                    dist[i] = alt;
                    prev[i] = u;
                }
            }
        }
    }
    delete[] visited;
}

int lowest_cost_root(Hardware &g, Graph &h, int *cost)
{
    int minimum_sum = std::numeric_limits<int>::max();
    int minimum_vertex = -1;

    for (int vertex = 0; vertex < g.get_n(); vertex++)
    {
        int sum = 0;
        for (int bag = 0; bag < h.get_n(); bag++)
        {
            sum += cost[bag*g.get_n()+vertex];
        }
        if (sum < minimum_sum)
        {
            minimum_sum = sum;
            minimum_vertex = vertex;
        }
    }
    return minimum_vertex;
}

/* Return the index of the smallest element in the array */
int minimum_distance(int *visited, int *distance, int length)
{
    int index = -1;
    int value = std::numeric_limits<int>::max();

    for (int i = 0; i < length; i++)
    {
        if (visited[i] == 0 && distance[i] < value)
        {
            index = i;
            value = distance[i];
        }
    }
    return index;
}

int weight_computation(Hardware &g, CMR_Embedding &phi, int bag, int vertex)
{
    int exponent = phi.get_number_of_bags(vertex);
    if (phi.contains(bag, vertex))
    {
        exponent--;
    }

    return pow(g.get_n(), exponent);
}
