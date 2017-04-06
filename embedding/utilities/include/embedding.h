#ifndef UTILITIES_EMBEDDING_H_
#define UTILITIES_EMBEDDING_H_

#include <fstream>
#include "graph.h"

class Embedding
{
  public:
    Embedding(int input_g_n, int input_h_n);
    ~Embedding();
    void add_vertex(int bag, int vertex);
    void remove_vertex(int bag, int vertex);
    bool contains(int bag, int vertex);
    bool is_bag_empty(int bag);
    bool is_empty();
    void clear_bag(int bag);
    void clear();
    int compute_qubits_used();
    void print();
    void write_to_file(std::string &filename);
    void write_to_latex(int n, Chimera &hardware, std::string &filename);

 protected:
  bool *phi;
  int g_n;
  int h_n;
};

class CMR_Embedding : public Embedding
{
  public:
    CMR_Embedding(int input_g_n, int input_h_n);
    ~CMR_Embedding();
    void add_vertex(int bag, int vertex);
    void remove_vertex(int bag, int vertex);
    bool contains(int bag, int vertex);
    void clear_bag(int bag);
    void clear();
    bool is_valid();
    int get_qubits_used();
    int get_number_of_bags(int vertex);
    int get_max_number_of_bags();

 protected:
    /* counts the sum of bag sizes */
    int qubits_used;
    /* counts how many bags v is in */
    int *number_of_bags;
};

#endif
