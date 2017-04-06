#include "../include/virtual_hardware.h"

VirtualHardware::VirtualHardware(int size, int c)
{
    partites = std::vector< std::vector<int> >(2);
    min_indices = \
        std::vector< std::vector<int> >(size, std::vector<int>(size, size+1));
    max_indices = \
        std::vector< std::vector<int> >(size, std::vector<int>(size, -1));
    scores = std::vector< std::vector<int> >(size, std::vector<int>(size, 0));
    neighbors = \
        std::vector< std::vector< std::vector<int> > >(2,
            std::vector< std::vector<int> >(size, std::vector<int>()));
    positions = std::vector< std::vector<int> >(2, std::vector<int>(size, -1));
    distinct_vertices = size;
    qubits_used = 0;
    chimera_c = c;
}

bool VirtualHardware::embed(Graph &program,
                            Chimera &chimera,
                            EmbeddingScheme embedding_scheme,
                            ReductionScheme reduction_scheme,
                            std::vector<int> &oct)
{
  /* Add vertices and edges as appropriate */
    if (embedding_scheme == NATIVE)
    {
        if (program.get_n() > std::min(chimera.get_m(),
                                       chimera.get_n()) * chimera.get_c())
        {
            return false;
        }

        /* Maps vertices using the native clique Embedding
           Vertices on both sides have only the lexicographical
           first edge enabled */
        for (int i = 0; i < program.get_n(); i++)
        {
            partites[0].push_back(i);
            partites[1].push_back(i);
            for (int j = 0; j < program.get_n(); j++)
            {
                if (i <= j && program.has_edge(i, j))
                {
                    neighbors[0][i].push_back(j);
                    neighbors[1][i].push_back(j);
                }
            }
            neighbors[0][i].push_back(i);
            neighbors[1][i].push_back(i);
        }
    } else if (embedding_scheme == OCT)
    {
        /* Add the vertices */
        std::vector<int> left_partition;
        std::vector<int> right_partition;
        std::vector<int> vertices;
        for (int i = 0; i < program.get_n(); i++)
        {
            if (std::find(oct.begin(), oct.end(), i) == oct.end())
            {
                vertices.push_back(i);
            }
        }
        compute_bipartitions(program,
                             vertices,
                             left_partition,
                             right_partition);

        /* if one side is too big for the virtual hardware, we cannot embed */
        int bigger_partite = std::max(left_partition.size(),
                                      right_partition.size()) + oct.size();
        int smaller_partite = std::min(left_partition.size(),
                                       right_partition.size()) + oct.size();
        int bigger_hardware = chimera.get_c() * std::max(chimera.get_m(),
          chimera.get_n());
        int smaller_hardware = chimera.get_c() * std::min(chimera.get_m(),
          chimera.get_n());

        if (bigger_partite > bigger_hardware ||
            smaller_partite > smaller_hardware)
        {
            return false;
        }

        for (int vertex : oct)
        {
            partites[0].push_back(vertex);
            partites[1].push_back(vertex);
        }

        for (int vertex : left_partition)
        {
            partites[0].push_back(vertex);
        }

        for (int vertex : right_partition)
        {
            partites[1].push_back(vertex);
        }


        /* Add the edges */
        for (int u : left_partition)
        {
            for (int v : oct)
            {
                if (program.has_edge(u ,v))
                {
                    neighbors[0][u].push_back(v);
                    neighbors[1][v].push_back(u);
                }
            }
            for (int v : right_partition)
            {
                if (program.has_edge(u, v))
                {
                    neighbors[0][u].push_back(v);
                    neighbors[1][v].push_back(u);
                }
            }
        }
        for (int u : right_partition)
        {
            for (int v : oct)
            {
                if (program.has_edge(u, v))
                {
                    neighbors[1][u].push_back(v);
                    neighbors[0][v].push_back(u);
                }
            }
        }

        /* Handle the OCT-closed edges using the scheme */
        if (reduction_scheme == FULL)
        {
            for (int u : oct)
            {
                for (int v : oct)
                {
                    /* Take lexicographical first and u == v */
                    if (u == v || (u < v && program.has_edge(u, v)))
                    {
                        neighbors[0][u].push_back(v);
                        neighbors[1][v].push_back(u);
                    }
                }
            }
        }
        else if (reduction_scheme == NONE)
        {
            for (int u : oct)
            {
                for (int v : oct)
                {
                    /* Take undirected edge if it exists and u == v */
                    if (u == v || program.has_edge(u, v))
                    {
                        neighbors[0][u].push_back(v);
                        neighbors[1][v].push_back(u);
                    }
                }
            }
        }
    }

    evaluate();
    return true;
}

void VirtualHardware::add_vertex(int partite_index, int position, int vertex)
{
    partites[partite_index][position] = vertex;
}

int VirtualHardware::size(int partite_index)
{
    return partites[partite_index].size();
}

/* Works with Chimera(L, M, N) */
void VirtualHardware::embed_in_chimera(Graph &program,
                                       Chimera &chimera,
                                       Embedding &phi)
{
    int current_qubit;
    const int CELL_JUMP = 2 * chimera.get_c();
    const int ROW_JUMP = CELL_JUMP * chimera.get_n();

    /* Add the left partite */
    for (int i = 0; i < partites[0].size(); i++)
    {
        /* Identify the first qubit added to the bag if the min_index is 0 */
        current_qubit = (i / chimera.get_c() * CELL_JUMP) + \
            (i % chimera.get_c());
        /* Trim the bag by starting where an edge is first needed */
        current_qubit += min_indices[0][i] * ROW_JUMP;
        phi.add_vertex(partites[0][i], current_qubit);
        for (int j = 0; j < max_indices[0][i] - min_indices[0][i]; j++)
        {
            current_qubit += ROW_JUMP;
            phi.add_vertex(partites[0][i], current_qubit);
        }
    }

    /* Add the right partite */
    for (int i = 0; i < partites[1].size(); i++)
    {
        current_qubit = (i / chimera.get_c() * ROW_JUMP) + \
            (i % chimera.get_c() + chimera.get_c());
        current_qubit += min_indices[1][i] * CELL_JUMP;
        phi.add_vertex(partites[1][i], current_qubit);
        for (int j = 0; j < max_indices[1][i] - min_indices[1][i]; j++)
        {
            current_qubit += CELL_JUMP;
            phi.add_vertex(partites[1][i], current_qubit);
        }
    }
}

/* Optimization: */
void VirtualHardware::evaluate()
{
    qubits_used = 0;

    /* Update/reset all the vectors */
    for (int partite_index = 0; partite_index < 2; partite_index++)
    {
        for (int i = 0; i < partites[partite_index].size(); i++)
        {
            positions[partite_index][partites[partite_index][i]] = i;
            min_indices[partite_index][i] = distinct_vertices+1;
            max_indices[partite_index][i] = -1;
        }
    }

    /* Evaluate each vertex and add its score to the total */
    for (int partite_index = 0; partite_index < 2; partite_index++)
    {
        for (int i = 0; i < partites[partite_index].size(); i++)
        {
            qubits_used += evaluate_vertex(partite_index, i);
        }
    }
}

int VirtualHardware::evaluate_vertex(int partite_index, int vertex_index)
{
    int vertex = partites[partite_index][vertex_index];
    for (int neighbor : neighbors[partite_index][vertex])
    {
        int neighbor_partite = (partite_index + 1) % 2;
        int neighbor_distance = \
            positions[neighbor_partite][neighbor] / chimera_c;
        if (neighbor_distance < min_indices[partite_index][vertex_index])
        {
            min_indices[partite_index][vertex_index] = neighbor_distance;
        }
        if (neighbor_distance > max_indices[partite_index][vertex_index])
        {
            max_indices[partite_index][vertex_index] = neighbor_distance;
        }
    }
    scores[partite_index][vertex_index] = \
        max_indices[partite_index][vertex_index] - \
        min_indices[partite_index][vertex_index] + 1;
    return scores[partite_index][vertex_index];
}

void VirtualHardware::reduce()
{
    /* For now, only offer the 2-exchange */
    int last_score = qubits_used;
    compute_best_two_exchange();
    int new_score = qubits_used;
    while(new_score < last_score)
    {
        last_score = new_score;
        compute_best_two_exchange();
        new_score = qubits_used;
    }
}


/* Run through all possible 2-exchanges for a vh and execute the best one */
void VirtualHardware::compute_best_two_exchange()
{
    /* 0: partite, 1: index1, 2: index2, 3: score */
    std::vector<int> best_exchange(4, -1);
    best_exchange[3] = qubits_used;
    std::vector<int> exchange(4, 0);

    /* Run through all 2-exchanges and track the best */
    for (exchange[0] = 0; exchange[0] < 2; exchange[0]++)
    {
        for (exchange[1] = 0;
             exchange[1] < partites[exchange[0]].size();
             exchange[1]++)
        {
            for (exchange[2] = exchange[1]+1;
                exchange[2] < partites[exchange[0]].size();
                exchange[2]++)
            {
                compute_two_exchange(exchange);

                if (exchange[3] < best_exchange[3])
                {
                    best_exchange = exchange;
                }
                compute_two_exchange(exchange);
            }
        }
    }
    if (best_exchange[0] != -1)
    {
        compute_two_exchange(best_exchange);
    }
}

/* Given a 2-exchange, execute and report back the score */
void VirtualHardware::compute_two_exchange(std::vector<int> &exchange)
{
    int partite_index = exchange[0];
    int index1 = exchange[1];
    int index2 = exchange[2];
    int temp = partites[partite_index][index1];
    partites[partite_index][index1] = partites[partite_index][index2];
    partites[partite_index][index2] = temp;
    evaluate();
    exchange[3] = qubits_used;
}


void VirtualHardware::write_to_file(std::string &filename)
{
    std::ofstream outfile(filename + ".vh");
    outfile << partites[0].size() << " " << partites[1].size() << std::endl;
    for (int i = 0; i < partites[0].size(); i++) {
        outfile << 0 << " " << i << " " << partites[0][i] << std::endl;
    }
    for (int i = 0; i < partites[1].size(); i++) {
        outfile << 1 << " " << i << " " << partites[1][i] << std::endl;
    }
    outfile.close();
}
