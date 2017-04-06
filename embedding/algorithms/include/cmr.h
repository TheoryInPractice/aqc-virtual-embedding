#ifndef ALGORITHMS_CMR_H_
#define ALGORITHMS_CMR_H_

#include <iostream>
#include <limits>
#include <vector>
#include <math.h>
#include <algorithm>

#include "../../utilities/include/graph.h"
#include "../../utilities/include/embedding.h"

void compute_cmr_dijkstra(Hardware &g,
                          Graph &h,
                          CMR_Embedding &phi,
                          int seed,
                          int tries,
                          bool verbose);
bool find_minor_embedding(Hardware &g,
                          Graph &h,
                          CMR_Embedding &phi,
                          bool verbose);
void find_minimal_vertex_model(Hardware &g,
                               Graph &h,
                               CMR_Embedding &phi,
                               int *weight,
                               int current_bag,
                               bool verbose);
int shortest_path_distance(Hardware &g,
                           Graph &h,
                           CMR_Embedding &phi,
                           int *weight,
                           std::vector<std::vector<int>> &path,
                           int source_vertex,
                           int bag);
void dijkstra(Hardware &g,
              CMR_Embedding &phi,
              int *weight,
              int *dist,
              int *prev,
              int source);
int lowest_cost_root(Hardware &g, Graph &h, int *cost);
int minimum_distance(int *visited, int *distance, int length);
int weight_computation(Hardware &g, CMR_Embedding &phi, int bag, int vertex);

#endif
