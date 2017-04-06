#ifndef ALGORITHMS_OCTCOMMON_H_
#define ALGORITHMS_OCTCOMMON_H_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>
#include <queue>
#include <string>
#include <utility>
#include <vector>
#include <math.h>
#include <time.h>

#include "../../utilities/include/graph.h"
#include "../../utilities/include/embedding.h"

/* Functions */
/* Computation Subroutines */
void compute_st_sep(Graph &g,
					int subgraph_n,
					std::vector<int> vertices,
					std::vector<int> &left_partition,
					std::vector<int> &right_partition,
					std::vector<int> &sep);
bool compute_st_path(Graph &g,
					 std::vector<int> &vertices,
					 int s,
					 int t,
					 std::vector<int> &path,
					 std::vector<int> &s_vertices);
void compute_partition(int x,
					   std::vector<int> &partitio_initial,
					   std::vector<int> &partition1,
					   std::vector<int> &partition2);
bool is_independent(Graph &g, std::vector<int> &vertices);
void compute_bipartitions(Graph &g,
						  std::vector<int> &vertices,
						  std::vector<int> &a,
						  std::vector<int> &b);
void breadth_first_search(Graph &g, std::vector<int> &vertices, int *distance);
void neighbors(Graph &g,
			   std::vector<int> &source,
			   std::vector<int> &target,
			   std::vector<int> &neighbors);

/* Verification */
bool verify_solution(Graph &g, std::vector<int> solution);

/* Embedding */
void compute_embedding(Chimera &hardware,
					   std::vector<int> left_partition,
					   std::vector<int> right_partition,
					   Embedding &phi);
void compute_embedding(Graph &program,
					   Chimera &hardware,
					   std::vector<int> solution,
					   Embedding &phi);
#endif
