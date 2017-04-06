#include "../include/klymko.h"

void compute_klymko(Chimera &hardware, int n, Embedding &phi)
{
    std::vector<int> left_partition, right_partition;

    /* Klymko is a case of OCT-embed, where min OCT # = |V|-2 */
    for (int i = 0; i < n-2; i++)
    {
        left_partition.push_back(i);
        right_partition.push_back(i);
    }

    left_partition.push_back(n-2);
    right_partition.push_back(n-1);
    compute_embedding(hardware, left_partition, right_partition, phi);
}
