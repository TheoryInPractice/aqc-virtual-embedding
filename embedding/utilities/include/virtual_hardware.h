#ifndef UTILITIES_VIRTUALHARDWARE_H_
#define UTILITIES_VIRTUALHARDWARE_H_

#include <fstream>
#include <algorithm>
#include <vector>
#include "../../algorithms/include/oct_common.h"

/* Currently hardcoded for complete bipartite */
class VirtualHardware
{
  public:
    enum EmbeddingScheme
    {
        NATIVE,
        OCT
    };
    enum ReductionScheme
    {
        NONE,
        FULL
    };
    VirtualHardware(int size, int c);
    bool embed(Graph &program,
               Chimera &chimera,
               EmbeddingScheme embedding_scheme,
               ReductionScheme reduction_scheme,
               std::vector<int> &oct);
    void add_vertex(int partite_index, int position, int vertex);
    void switch_vertices(int partite_index, int position1, int position2);
    int size(int partite_index);
    void write_to_file(std::string &filename);

    /* The trivial mapping, no order optimization
      Returns whether the OCT-decomposed graph can fit the virtual hardware
    */

    bool generate_from_oct(Graph &program,
                           Chimera &hardware,
                           std::vector<int> &oct);
    /* Works with Chimera(L, M, N) */
    void embed_in_chimera(Graph &program, Chimera &chimera, Embedding &phi);
    void evaluate();
    int evaluate_vertex(int partite_index, int vertex_index);

    void reduce();
    void compute_best_two_exchange();
    void compute_two_exchange(std::vector<int> &exchange);

  protected:
    /* indexed by [partite #][vertex index] */
    std::vector< std::vector<int> > partites;
    std::vector< std::vector<int> > min_indices;
    std::vector< std::vector<int> > max_indices;
    std::vector< std::vector<int> > scores;

    /* indexed by [partite #][vertex value][neighbors] */
    std::vector< std::vector< std::vector<int> > > neighbors;

    /* indexed by [partite #][vertex value] */
    std::vector< std::vector<int> > positions;

    int distinct_vertices;
    int qubits_used;
    int chimera_c;
};

#endif
