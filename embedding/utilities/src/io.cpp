#include "../include/io.h"

char* get_cmd_option(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmd_option_exists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}


void print_error(int e, std::string flag)
{
    switch(e)
    {
        case 1:
            std::cout << "Error: Flag " << flag << " not provided" << std::endl;
            break;
        case 2:
            std::cout << "Error: Argument for flag "
                      << flag
                      << " is invalid"
                      << std::endl;
            break;
    }
}

bool help_flag(int argc, char *argv[])
{
    if (cmd_option_exists(argv, argv+argc, "-h")) {
        std::cout << "Usage: generate_embedding -a [algorithm]"
                  <<  " -pfile [program] -h [hardware] -o [output file]"
                  << std::endl;
        std::cout << "-h       : Help" << std::endl;
        std::cout << "-a       : Select the algorithm to run" << std::endl;
        std::cout << "-pfile   : The input file for the program" << std::endl;
        std::cout << "-hfile   : The input file for the hardware " << std::endl;
        std::cout << "-o       : The output file for the embedding"
                  << std::endl;
        std::cout << "-c/-m/-n : Chimera parameters" << std::endl;
        std::cout << "-k       : Parameter for parameterized algorithms"
                  << std::endl;
        std::cout << "-s       : Seed for the pseudorandom number generator"
                  << std::endl;
        std::cout << "-r       : Number of runs before giving up" << std::endl;
        std::cout << "See documentation for complete usage notes" << std::endl;
        return true;
    }
    return false;
}

bool read_int(int argc, char *argv[], std::string flag, int &n)
{
    try
    {
        if (!cmd_option_exists(argv, argv+argc, flag))
        {
            throw 1;
        }
        else
        {
            char *input = get_cmd_option(argv, argv+argc, flag);
            if (input)
            {
                n = atoi(input);
            }
            else
            {
                throw 2;
            }
        }
    }
    catch (int e)
    {
        print_error(e, flag);
        return false;
    }
    return true;
}

bool read_string(int argc,
                 char *argv[],
                 std::string flag,
                 std::string &filename)
{
    try
    {
        if (!cmd_option_exists(argv, argv+argc, flag))
        {
            throw 1;
        }
        else
        {
            char *input = get_cmd_option(argv, argv+argc, flag);
            if (input)
            {
                filename.append(input);
            }
            else
            {
                throw 2;
            }
        }
    }
    catch(int e)
    {
        print_error(e, flag);
        return false;
    }
    return true;
}

/* Reads in the program's # of vertices */
void read_program_order(int argc, char *argv[], int &n)
{
    std::string filename("");
    read_string(argc, argv, "-pfile", filename);
    std::ifstream infile(filename);
    infile >> n;
    infile.close();
}

/* Reads in a program to a vertex count (i.e. assumes complete graph) */
void read_program(int argc, char *argv[], int &n)
{
  std::string filename("");
  read_string(argc, argv, "-pfile", filename);
  std::ifstream infile(filename);
  infile >> n;
  infile.close();
}

/* Reads in a program into a Graph data structure */
Graph *read_program(int argc, char *argv[])
{
    std::string filename("");
    int n;
    read_string(argc, argv, "-pfile", filename);
    std::ifstream infile(filename);
    infile >> n;
    Graph *g = new Graph(n);
    int temp1, temp2;
    for (int i = 0; i < n; i++)
    {
        infile >> temp1;
        g->set_ordering(i, temp1);
    }
    while (infile >> temp1 >> temp2)
    {
        g->add_edge(temp1, temp2);
        g->add_edge(temp2, temp1);
    }
    infile.close();
    return g;
}

/* Reads a hardware graph into a Hardware data structure */
Hardware *read_hardware(int argc, char *argv[])
{
    std::string filename("");
    int n, diameter;
    read_string(argc, argv, "-hfile", filename);
    std::ifstream infile(filename);
    infile >> n >> diameter;
    Hardware *g = new Hardware(n, diameter);
    int temp1, temp2;
    for (int i = 0; i < n; i++)
    {
        infile >> temp1;
        g->set_ordering(i, temp1);
    }
    while (infile >> temp1 >> temp2)
    {
        g->add_edge(temp1, temp2);
        g->add_edge(temp2, temp1);
    }
    infile.close();
    return g;
}

/* Reads in the three Chimera dimensions into a Chimera data structure */
Chimera *read_chimera(int argc, char *argv[])
{
    int c,m,n;
    if (read_int(argc, argv, "-c", c) &&
        read_int(argc, argv, "-m", m) &&
        read_int(argc, argv, "-n", n))
    {
        return new Chimera(c, m, n);
    }
    return NULL;
}

void read_cmr_inputs(int argc,
                     char *argv[],
                     Hardware **hardware,
                     Chimera **chimera,
                     Graph **program,
                     int &seed,
                     std::string &outfile)
{
    *hardware = read_hardware(argc, argv);
    *chimera = read_chimera(argc, argv);
    *program = read_program(argc, argv);
    read_int(argc, argv, "-s", seed);
    read_string(argc, argv, "-o", outfile);
}

void read_complete_inputs(int argc,
                          char *argv[],
                          int &n,
                          Chimera **chimera,
                          std::string &outfile)
{
    *chimera = read_chimera(argc, argv);
    read_program(argc, argv, n);
    read_string(argc, argv, "-o", outfile);
}

void read_oct_inputs(int argc,
                     char *argv[],
                     Graph **program,
                     Chimera **chimera,
                     std::string &outfile)
{
    *chimera = read_chimera(argc, argv);
    *program = read_program(argc, argv);
    read_string(argc, argv, "-o", outfile);
}

void read_oct_inputs(int argc,
                     char *argv[],
                     Graph **program,
                     Chimera **chimera,
                     int &k,
                     std::string &outfile)
{
    *chimera = read_chimera(argc, argv);
    *program = read_program(argc, argv);
    read_int(argc, argv, "-k", k);
  read_string(argc, argv, "-o", outfile);
}

void read_oct_inputs(int argc,
                     char *argv[],
                     Graph **program,
                     Chimera **chimera,
                     int &seed,
                     int &repeats,
                     std::string &outfile)
{
  *chimera = read_chimera(argc, argv);
  *program = read_program(argc, argv);
  read_int(argc, argv, "-s", seed);
  read_int(argc, argv, "-r", repeats);
  read_string(argc, argv, "-o", outfile);
}
