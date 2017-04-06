#include "../include/oct_hybrid.h"

void compute_hybrid_oct_embed(Graph &program,
                              std::vector<int> &solution,
                              int seed,
                              int repeats,
                              bool verbose)
{
    /* Call fast_oct_embed to get approximation of the oct set */
    compute_fast_oct_embed(program, solution, seed, repeats, verbose);

    /* If the OCT set is empty then we need to stop (or start with a BFS) */
    if (solution.size() == 0)
    {
        return void();
    }

    /* Adjust the ordering so that these oct vertices come last */
    int counter = 0;
    for (int i = 0; i < program.get_n(); i++)
    {
        int vertex = program.get_ordering(i);
        /* Move the bipartite graph to the front of the ordering */
        if (std::find(solution.begin(), solution.end(), vertex) ==
            solution.end())
        {
            program.set_ordering(counter++, vertex);
        }
    }

    if (verbose) {
        std::cout << "OCT vertices: ";
        for (int vertex : solution)
        {
            std::cout << vertex << " ";
            program.set_ordering(counter++, vertex);
        }
        std::cout << std::endl;

        std::cout << "Program ordering: ";
        for (int i = 0; i < program.get_n(); i++)
        {
            std::cout << program.get_ordering(i) << " ";
        }
        std::cout << std::endl;
    }

    /* Call the exact algorithm with the appropriate starting index */
    int starting_index = program.get_n() - solution.size();
    if (verbose)
    {
        std::cout << "Starting index " << starting_index << std::endl;
    }
    solution.clear();
    oct_embed_minimize(program, solution, starting_index, verbose);

    if (verbose)
    {
        std::cout << "Solution size: " << solution.size() << std::endl;
        std::cout << "Solution verified: "
                  << verify_solution(program, solution)
                  << std::endl;
    }
}
