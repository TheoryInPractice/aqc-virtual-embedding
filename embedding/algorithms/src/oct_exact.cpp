#include "../include/oct_exact.h"

void compute_oct_embed(Graph &program, std::vector<int> &solution, bool verbose)
{
    bool result = oct_embed_minimize(program, solution, 0, verbose);
    std::cout << "Run correctly: " << result << std::endl;
    std::cout << "Solution size: " << solution.size() << std::endl;
    std::cout << "Solution verified "
              << verify_solution(program, solution)
              << std::endl;
}

void compute_oct_embed(Graph &program,
                       int k,
                       std::vector<int> &solution,
                       bool verbose)
{
    bool result = oct_embed_parameterized(program, k, solution, verbose);
    std::cout << "Run correctly: " << result << std::endl;
    std::cout << "Solution size: " << solution.size() << std::endl;
    std::cout << "Solution verified "
              << verify_solution(program, solution)
              << std::endl;
}

/*
Input:
  n :          Graph's order
  adjMatrix :  Graph's adjacency matrix
  ordering :   An ordering on the graph's vertices
  k :          The size of the solution we're looking for
  solution :   An empty array of size k, to put solution in if it exists
Output:
  Return true/false if a solution of size k exists/doesn't exist
*/
bool oct_embed_parameterized(Graph &g,
                             int k,
                             std::vector<int> &solution,
                             bool verbose)
{
    /* The initial solution of size k */
    std::vector<int> current_solution;
    for (int i = 0; i < k; i++)
    {
        current_solution.push_back(g.get_ordering(i));
    }

    bool result = false;
    std::vector<int> next_solution;

    for (int i = k+2; i < g.get_n(); i++)
    {
        current_solution.push_back(g.get_ordering(i));
        result = iterative_compression_parameterized(g, i+1, k,
            current_solution, next_solution, verbose);

        if (result)
        {
            current_solution = next_solution;
            next_solution.clear();
        }
        else
        {
            return false;
        }
    }

    solution.insert(solution.end(),
                    current_solution.begin(),
                    current_solution.end());
    return true;
}


/*
  n :            Number of vertices in the adjacency matrix
  subgraph_n :  Number of vertices in this induced subgraph
  adjMatrix :  Graph's adjacency matrix
  ordering :   An ordering on the graph's vertices
  k :          The size of the solution we're looking for
  current_solution : Our current solution of size k+1
  solution :   An empty array of size k, to put solution in if it exists
*/
bool iterative_compression_parameterized(Graph &g,
                                         int subgraph_n,
                                         int k,
                                         std::vector<int> &current_solution,
                                         std::vector<int> &solution,
                                         bool verbose)
{
    std::vector<int> vertices_ab;
    std::vector<int> vertices_a;
    std::vector<int> vertices_b;

    /* Populate vertices_ab with everything but the current solution */
    for (int i = 0; i < subgraph_n; i++)
    {
        /* If a vertex isn't in the solution,
           it must be in the bipartite graph */
        if (find(current_solution.begin(),
                 current_solution.end(),
                 g.get_ordering(i)) == current_solution.end())
        {
            vertices_ab.push_back(g.get_ordering(i));
        }
    }

    /* Compute A,B */
    compute_bipartitions(g, vertices_ab, vertices_a, vertices_b);

    /* Compute all ("3^k") partitions of
       current_solution = partition_t union partition_l union partition_r */
    const int outer_upper = pow(2,current_solution.size());
    std::vector<int> partition_t;
    std::vector<int> partition_lr;
    std::vector<int> partition_l;
    std::vector<int> partition_r;

    for (int i = 0; i < outer_upper; i++)
    {
        partition_t.clear();
        partition_lr.clear();
        compute_partition(i, current_solution, partition_t, partition_lr);
        const int inner_upper = pow(2,partition_lr.size());

        if (verbose) {std::cout << "Outer iteration " << i << ":" << std::endl;}

        for (int j = 0; j < inner_upper; j++)
        {
            partition_l.clear();
            partition_r.clear();
            compute_partition(j, partition_lr, partition_l, partition_r);

            /* Check that L and R are independent sets */
            if (!is_independent(g, partition_l) ||
                !is_independent(g, partition_r))
            {
                continue;
            }

            std::vector<int> left_partition;
            std::vector<int> right_partition;
            neighbors(g, partition_l, vertices_a, left_partition);  /* AL */
            neighbors(g, partition_r, vertices_b, left_partition);  /* BR */
            neighbors(g, partition_l, vertices_b, right_partition); /* BL */
            neighbors(g, partition_r, vertices_a, right_partition); /* AR */

            /* Construct G_tilde */
            /* Compute seperating set on G_tilde */
            std::vector<int> sep;
            compute_st_sep(g, subgraph_n, vertices_ab, left_partition,
                right_partition, sep);

            if (sep.size() + partition_t.size() <= k)
            {
                solution.clear();
                solution.insert(solution.end(),
                                partition_t.begin(),
                                partition_t.end());
                solution.insert(solution.end(), sep.begin(), sep.end());
                return true;
            }
        }
    }
    return false;
}

/* Starting index is either 0 (no preprocessing) or |V(G)-S-1|
   where S is our OCT approximation */
bool oct_embed_minimize(Graph &g,
                        std::vector<int> &solution,
                        int starting_index,
                        bool verbose)
{
    std::vector<int> current_solution;
    for (int i = starting_index; i < starting_index + 1; i++)
    {
        current_solution.push_back(g.get_ordering(i));
    }

    bool result = false;
    std::vector<int> next_solution;

    for (int i = starting_index + 1; i < g.get_n(); i++)
    {
        current_solution.push_back(g.get_ordering(i));
        result = iterative_compression_minimize(g, i+1, current_solution,
            next_solution, verbose);

        if (result)
        {
            current_solution = next_solution;
            next_solution.clear();
        }
        else
        {
            return false;
        }
    }

    solution.insert(solution.end(),
                    current_solution.begin(),
                    current_solution.end());
    return true;
}

bool iterative_compression_minimize(Graph &g,
                                    int subgraph_n,
                                    std::vector<int> &current_solution,
                                    std::vector<int> &solution,
                                    bool verbose)
{
    std::vector<int> vertices_ab;
    std::vector<int> vertices_a;
    std::vector<int> vertices_b;

    /* Populate vertices_ab with everything but the current solution */
    for (int i = 0; i < subgraph_n; i++)
    {
        /* If a vertex isn't in the solution,
           it must be in the bipartite graph */
        if (find(current_solution.begin(),
                 current_solution.end(),
                 g.get_ordering(i)) == current_solution.end())
        {
            vertices_ab.push_back(g.get_ordering(i));
        }
    }

    /* Compute A,B */
    compute_bipartitions(g, vertices_ab, vertices_a, vertices_b);

    /* Compute all ("3^k") partitions of
       current_solution = partition_t union partition_l union partition_r */
    const int outer_upper = pow(2,current_solution.size());
    std::vector<int> partition_t;
    std::vector<int> partition_lr;
    std::vector<int> partition_l;
    std::vector<int> partition_r;

    for (int i = 0; i < outer_upper; i++)
    {
        partition_t.clear();
        partition_lr.clear();
        compute_partition(i, current_solution, partition_t, partition_lr);
        const int inner_upper = pow(2,partition_lr.size());

        if (verbose) {
            std::cout << "Outer iteration " << i << ":" << std::endl;
        }

        for (int j = 0; j < inner_upper; j++)
        {
            partition_l.clear();
            partition_r.clear();
            compute_partition(j, partition_lr, partition_l, partition_r);

            /* Check that L and R are independent sets */
            if (!is_independent(g, partition_l) ||
                !is_independent(g, partition_r))
            {
                continue;
            }

            std::vector<int> left_partition;
            std::vector<int> right_partition;
            neighbors(g, partition_l, vertices_a, left_partition);  /* AL */
            neighbors(g, partition_r, vertices_b, left_partition);  /* BR */
            neighbors(g, partition_l, vertices_b, right_partition); /* BL */
            neighbors(g, partition_r, vertices_a, right_partition); /* AR */

            /* Construct G_tilde */
            /* Compute seperating set on G_tilde */
            std::vector<int> sep;
            compute_st_sep(g, subgraph_n, vertices_ab, left_partition,
                right_partition, sep);
            int new_solution_size = sep.size() + partition_t.size();

            /* If the graph is bipartite, return with empty solution now */
            if (new_solution_size == 0)
            {
                return true;
            }
            else if (solution.size() == 0 ||
                     new_solution_size < solution.size())
            {
                solution.clear();
                solution.insert(solution.end(),
                                partition_t.begin(),
                                partition_t.end());
                solution.insert(solution.end(), sep.begin(), sep.end());
            }
        }
    }
    return solution.size() != 0;
}
