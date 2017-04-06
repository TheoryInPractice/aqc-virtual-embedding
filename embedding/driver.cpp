#include "driver.h"

/*
 Main driver, takes in CLI arguments with flags, and uses these to fire off the
 appropriate embedding algorithm.
*/

int main(int argc, char *argv[])
{
    if (help_flag(argc, argv))
    {
        return 0;
    }

    std::string algorithm("");
    try
    {
        read_string(argc, argv, "-a", algorithm);
        if (algorithm == "triad")
        {
            embed_complete(argc, argv, &compute_triad);
        }
        else if (algorithm == "triad-reduce" || algorithm == "triad-native")
        {
            embed_triad_reduce(argc, argv);
        }
        else if (algorithm == "klymko")
        {
            embed_complete(argc, argv, &compute_klymko);
        }
        else if (algorithm == "cmr")
        {
            embed_cmr(argc, argv);
        }
        else if (algorithm == "oct")
        {
            embed_oct(argc, argv, &compute_oct_embed, VirtualHardware::NONE);
        }
        else if (algorithm == "oct-reduce")
        {
            embed_oct(argc, argv, &compute_oct_embed, VirtualHardware::FULL);
        }
        else if (algorithm == "fast-oct" || algorithm == "oct-fast")
        {
            embed_oct_seed_repeats(argc, argv, &compute_fast_oct_embed,
                VirtualHardware::NONE);
        }
        else if (algorithm == "fast-oct-reduce" ||
                 algorithm == "oct-fast-native")
        {
            embed_oct_seed_repeats(argc, argv, &compute_fast_oct_embed,
                VirtualHardware::FULL);
        }
        else if (algorithm == "hybrid-oct")
        {
            std::cout << "Got here!" << std::endl;
            embed_oct_seed_repeats(argc, argv, &compute_hybrid_oct_embed,
                VirtualHardware::NONE);
        }
        else if (algorithm == "hybrid-oct-reduce")
        {
            embed_oct_seed_repeats(argc, argv, &compute_hybrid_oct_embed,
                VirtualHardware::FULL);
        }
     }
     catch (int e) {
        return 1;
    }
        return 0;
}


/*******************************************************************************
* Controller for embedding TRIAD and Klymko
*******************************************************************************/
void embed_complete(int argc, char *argv[], void (*embedding_algorithm)(
    Chimera &chimera, int n, Embedding &phi))
{

    // Read in what we need
    int n;
    Chimera *chimera;
    std::string outfile("");
    read_complete_inputs(argc, argv, n, &chimera, outfile);

    // Prepare for running the embedding algorithm
    Timer timer;
    Embedding phi(chimera->get_num_vertices(), n);

    // Execute
    timer.start_timer();
    (*embedding_algorithm)(*chimera, n, phi);
    timer.stop_timer();

    // Write what we need
    timer.write_to_file(outfile);
    phi.write_to_file(outfile);
    phi.write_to_latex(n, *chimera, outfile);

    // Clean up
    delete chimera;
}


/*******************************************************************************
* Controller for embedding CMR
*******************************************************************************/
void embed_cmr(int argc, char *argv[])
{
    // Read in what we need
    Hardware *hardware;
    Chimera *chimera;
    Graph *program;
    int seed;
    std::string outfile("");
    read_cmr_inputs(argc, argv, &hardware, &chimera, &program, seed, outfile);

    // Prepare for running the embedding algorithm
    CMR_Embedding phi(hardware->get_n(), program->get_n());
    Timer timer;

    // Execute
    timer.start_timer();
    compute_cmr_dijkstra(*hardware, *program, phi, seed, 20, false);
    timer.stop_timer();

    // Write what we need
    timer.write_to_file(outfile);
    phi.write_to_file(outfile);
    phi.write_to_latex(program->get_n(), *chimera, outfile);

    // Clean up
    delete hardware;
    delete chimera;
    delete program;
}

/*******************************************************************************
*
*******************************************************************************/
void embed_oct(int argc, char *argv[], void (*embedding_algorithm)(Graph &program, std::vector<int> &solution, bool verbose), VirtualHardware::ReductionScheme reduction_scheme)
{

    Timer timer;
    // Prepare general oct inputs
    Graph *program;
    Chimera *chimera;
    std::string outfile("");

    // Prepare for running the embedding algorithm
    std::vector<int> solution;

    // Read in specifics and execute
    read_oct_inputs(argc, argv, &program, &chimera, outfile);
    timer.start_timer();
    (*embedding_algorithm)(*program, solution, false);


    // Map solution to Virtual Hardware
    // If it does not fit then reject
    VirtualHardware vh(program->get_n(), chimera->get_c());
    if (!vh.embed(*program, *chimera, VirtualHardware::OCT, reduction_scheme, solution))
    {
        timer.stop_timer();
        // Write what we need
        timer.write_to_file(outfile);
        delete program;
        delete chimera;
        return void();
    }

    // Optimize the embedding
    if (reduction_scheme != VirtualHardware::NONE)
    {
        vh.reduce();
    }

    // Embed into the hardware
    Embedding phi(chimera->get_num_vertices(), program->get_n());
    vh.embed_in_chimera(*program, *chimera, phi);

    timer.stop_timer();

    // Write what we need
    timer.write_to_file(outfile);
    vh.write_to_file(outfile);
    phi.write_to_file(outfile);
    phi.write_to_latex(program->get_n(), *chimera, outfile);

    // Clean up
    delete program;
    delete chimera;
}


/*******************************************************************************
* Controller for running an oct algorithm that requires a seed and number of
* repeats specified.
*******************************************************************************/
void embed_oct_seed_repeats(int argc, char *argv[], void (*embedding_algorithm)(
    Graph &program, std::vector<int> &solution, int seed, int repeats,
    bool verbose), VirtualHardware::ReductionScheme reduction_scheme)
    {

    Timer timer;
    // Prepare general oct inputs
    Graph *program;
    Chimera *chimera;
    std::string outfile("");

    // Prepare for running the embedding algorithm
    std::vector<int> solution;

    // Read in specifics and execute
    int seed, repeats;
    read_oct_inputs(argc, argv, &program, &chimera, seed, repeats, outfile);
    timer.start_timer();
    (*embedding_algorithm)(*program, solution, seed, repeats, false);

    // Map solution to Virtual Hardware
    // If it does not fit then reject
    VirtualHardware vh(program->get_n(), chimera->get_c());
    if (!vh.embed(*program, *chimera, VirtualHardware::OCT, reduction_scheme, solution))
    {
        timer.stop_timer();
        // Write what we need
        timer.write_to_file(outfile);
        delete program;
        delete chimera;
        return void();
    }

    // Optimize the embedding
    if (reduction_scheme != VirtualHardware::NONE)
    {
        vh.reduce();
    }

    // Embed into the hardware
    Embedding phi(chimera->get_num_vertices(), program->get_n());
    vh.embed_in_chimera(*program, *chimera, phi);

    timer.stop_timer();

    // Write what we need
    timer.write_to_file(outfile);
    vh.write_to_file(outfile);
    phi.write_to_file(outfile);
    phi.write_to_latex(program->get_n(), *chimera, outfile);

    // Clean up
    delete program;
    delete chimera;
}


/*******************************************************************************
* Controller for the TRIAD embedding algorithm with the reduction stack
*******************************************************************************/
void embed_triad_reduce(int argc, char *argv[])
{

    Timer timer;
    timer.start_timer();
    // Read in what we need
    Graph *program;
    Chimera *chimera;
    std::string outfile("");
    read_oct_inputs(argc, argv, &program, &chimera, outfile);

    // Embed in virtual hardware
    // DOES NOTHING, we just want to use the embed method
    std::vector<int> solution;
    VirtualHardware vh(program->get_n(), chimera->get_c());
    if (!vh.embed(*program, *chimera, VirtualHardware::NATIVE, VirtualHardware::FULL, solution))
    {
        timer.stop_timer();
        // Write what we need
        timer.write_to_file(outfile);
        delete program;
        delete chimera;
        return void();
    }

    vh.reduce();
    Embedding phi(chimera->get_num_vertices(), program->get_n());
    vh.embed_in_chimera(*program, *chimera, phi);

    timer.stop_timer();

    // Write what we need
    timer.write_to_file(outfile);
    vh.write_to_file(outfile);
    phi.write_to_file(outfile);
    phi.write_to_latex(program->get_n(), *chimera, outfile);

    // Clean up
    delete program;
    delete chimera;
}
