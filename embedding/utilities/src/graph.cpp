#include "../include/graph.h"

Graph::Graph(int input_n)
{
    n = input_n;
    adjacency_matrix = new bool[n*n]();
    ordering = std::vector<int>(n, -1);
}

Graph::~Graph() {
    delete[] adjacency_matrix;
}

bool Graph::has_edge(int u, int v)
{
    return adjacency_matrix[u * n + v];
}

void Graph::add_edge(int u, int v)
{
    adjacency_matrix[u * n + v] = true;
}

void Graph::remove_edge(int u, int v)
{
    adjacency_matrix[u * n + v] = false;
}

void Graph::set_edge(int u, int v, bool value)
{
    adjacency_matrix[u * n + v] = value;
}

int Graph::get_n()
{
    return n;
}

void Graph::set_ordering(int index, int vertex)
{
    ordering[index] = vertex;
}

int Graph::get_ordering(int index)
{
    return ordering[index];
}

void Graph::shuffle_ordering()
{
    std::random_shuffle(ordering.begin(), ordering.end());
}

void Graph::print() {
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            std::cout << has_edge(i,j) << " ";
        }
        std::cout << std::endl;
    }
}


Hardware::Hardware(int input_n, int input_diameter) : Graph(input_n)
{
    diameter = input_diameter;
}

Hardware::~Hardware() {}

int Hardware::get_diameter()
{
    return diameter;
}


Chimera::Chimera(int input_c, int input_m, int input_n)
{
    c = input_c;
    m = input_m;
    n = input_n;
    num_vertices = 2 * c * m * n;
}

int Chimera::get_c()
{
    return c;
}

int Chimera::get_m()
{
    return m;
}

int Chimera::get_n()
{
    return n;
}

int Chimera::get_num_vertices()
{
    return num_vertices;
}
