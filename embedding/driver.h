#ifndef DRIVER_H_
#define DRIVER_H_

#include <algorithm>

#include "utilities/include/embedding.h"
#include "utilities/include/graph.h"
#include "utilities/include/io.h"
#include "utilities/include/timer.h"
#include "utilities/include/virtual_hardware.h"
#include "algorithms/include/oct_common.h"
#include "algorithms/include/oct_exact.h"
#include "algorithms/include/oct_fast.h"
#include "algorithms/include/oct_hybrid.h"
#include "algorithms/include/klymko.h"
#include "algorithms/include/cmr.h"
#include "algorithms/include/triad.h"

int main(int argc, char *argv[]);
void embed_cmr(int argc, char *argv[]);
void embed_complete(int argc,
                    char *argv[],
                    void (*embedding_algorithm)(
                      Chimera &chimera,
                      int n,
                      Embedding &phi));
void embed_oct(int argc,
               char *argv[],
               void (*embedding_algorithm)(
                 Graph &program,
                 std::vector<int> &solution,
                 bool verbose),
               VirtualHardware::ReductionScheme reduction_scheme);
void embed_oct_k(int argc, char *argv[]);
void embed_oct_seed_repeats(int argc,
                            char *argv[],
                            void (*embedding_algorithm)(
                              Graph &program,
                              std::vector<int> &solution,
                              int seed,
                              int repeats,
                              bool verbose),
                           VirtualHardware::ReductionScheme reduction_scheme);
void embed_triad_reduce(int argc, char *argv[]);
#endif
