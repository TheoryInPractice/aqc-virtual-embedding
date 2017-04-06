#ifndef ALGORITHMS_OCTEXACT_H_
#define ALGORITHMS_OCTEXACT_H_

#include "oct_common.h"
#include "oct_fast.h"

void compute_oct_embed(Graph &program,
                       std::vector<int> &solution,
                       bool verbose);
void compute_oct_embed(Graph &program,
                       int k,
                       std::vector<int> &solution,
                       bool verbose);
void compute_oct_embed_hybrid(Graph &program,
                              std::vector<int> &solution,
                              int seed,
                              int repeats,
                              bool verbose);
bool oct_embed_parameterized(Graph &g,
                             int k,
                             std::vector<int> &solution,
                             bool verbose);
bool oct_embed_minimize(Graph &g,
                        std::vector<int> &solution,
                        int starting_index,
                        bool verbose);
bool iterative_compression_parameterized(Graph &g,
                                         int subgraph_n,
                                         int k,
                                         std::vector<int> &current_solution,
                                         std::vector<int> &solution,
                                         bool verbose);
bool iterative_compression_minimize(Graph &g,
                                    int subgraph_n,
                                    std::vector<int> &current_solution,
                                    std::vector<int> &solution,
                                    bool verbose);
#endif
