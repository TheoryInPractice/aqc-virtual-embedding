#ifndef ALGORITHMS_OCTFAST_H_
#define ALGORITHMS_OCTFAST_H_

#include <stdlib.h>
#include "oct_common.h"

void compute_fast_oct_embed(Graph &program,
                            std::vector<int> &minimum_solution,
                            int seed,
                            int repeats,
                            bool verbose);
void fast_oct_embed(Graph &g, std::vector<int> &solution, bool verbose);
void greedy_independent_set(std::vector<int> vertices,
                            Graph &g,
                            std::vector<int> &independent_set,
                            bool verbose);
#endif
