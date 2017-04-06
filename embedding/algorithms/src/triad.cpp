#include "../include/triad.h"

void compute_triad(Chimera &chimera, int n, Embedding &phi)
{
    /* Identify the minimum chimera this graph can be embedded in */
    const int LIMITING_DIM = std::min(chimera.get_n(), chimera.get_m());
    const int CELLS_NEEDED = ceil(float(n) / chimera.get_c());
    if (CELLS_NEEDED > LIMITING_DIM)
    {
        return void();
    }

    /* chimera specific details */
    const int CELL_SIZE = 2 * chimera.get_c();
    const int ROW_SIZE = chimera.get_n() * CELL_SIZE;

    /* Construct each bag */
    for (int bag = 0; bag < n; bag++)
    {
        const int BAG_Q = (bag / chimera.get_c());
        const int BAG_R = (bag % chimera.get_c());

        /* Every bag is composed of a line of horizontal edges,
           an inner-cell edge, then a line of vertical edges.
           We only need to keep track of the vertices here.
        */
        const int CENTER_VERTICAL = BAG_Q * (CELL_SIZE+ROW_SIZE) + BAG_R;
        const int CENTER_HORIZONTAL = CENTER_VERTICAL + chimera.get_c();

        /* Vertical Vertices */
        int vertex = CENTER_VERTICAL;
        phi.add_vertex(bag, vertex);
        for (int row = 1; row < (CELLS_NEEDED - BAG_Q); row++)
        {
            vertex += ROW_SIZE;
            phi.add_vertex(bag, vertex);
        }

        /* Horizontal vertices */
        vertex = CENTER_HORIZONTAL;
        phi.add_vertex(bag, vertex);
        for (int col = 1; col <= BAG_Q; col++)
        {
            vertex -= CELL_SIZE;
            phi.add_vertex(bag, vertex);
        }
    }
}
