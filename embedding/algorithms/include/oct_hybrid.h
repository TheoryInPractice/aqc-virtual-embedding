#ifndef ALGORITHMS_OCTHYBRID_H_
#define ALGORITHMS_OCTHYBRID_H_

#include "oct_common.h"
#include "oct_fast.h"
#include "oct_exact.h"

void compute_hybrid_oct_embed(Graph &program,
                              std::vector<int> &solution,
                              int seed,
                              int repeats,
                              bool verbose);
#endif
