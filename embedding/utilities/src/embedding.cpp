#include "../include/embedding.h"

Embedding::Embedding(int input_g_n, int input_h_n)
{
    phi = new bool[input_g_n * input_h_n]();
    g_n = input_g_n;
    h_n = input_h_n;
}

Embedding::~Embedding()
{
    delete[] phi;
}

void Embedding::add_vertex(int bag, int vertex)
{
    phi[bag * g_n + vertex] = true;
}

void Embedding::remove_vertex(int bag, int vertex)
{
    phi[bag * g_n + vertex] = false;
}

bool Embedding::contains(int bag, int vertex)
{
    return phi[bag * g_n + vertex];
}

bool Embedding::is_bag_empty(int bag)
{
    for (int i = 0; i < g_n; i++)
    {
        if (contains(bag, i))
        {
            return false;
        }
    }
    return true;
}

bool Embedding::is_empty()
{
    for (int i = 0; i < h_n; i++)
    {
        if (!is_bag_empty(i))
        {
            return false;
        }
    }
    return true;
}

void Embedding::clear_bag(int bag)
{
    for (int i = 0; i < g_n; i++)
    {
        remove_vertex(bag, i);
    }
}

void Embedding::clear()
{
    for (int i = 0; i < h_n; i++)
    {
        clear_bag(i);
    }
}

int Embedding::compute_qubits_used()
{
    int counter = 0;
    for (int i = 0; i < g_n * h_n; i++)
    {
        counter += phi[i];
    }
    return counter;
}

void Embedding::print() {
    std::cout << "Embedding:" << std::endl;
    for (int i = 0; i < h_n; i++)
    {
        std::cout << "phi(" << i << ")" << std::endl;
        for (int j = 0; j < g_n; j++)
        {
            if (contains(i, j))
            std::cout << j << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "Requires "
              << compute_qubits_used()
              << " qubits"
              << std::endl;
}

void Embedding::write_to_file(std::string &filename)
{
    std::ofstream outfile(filename + ".embedding");
    bool first;
    for (int i = 0; i < h_n; i++)
    {
        outfile << i << ":";
        first = true;
        for (int j = 0; j < g_n; j++)
        {
            if (contains(i, j))
            {
                if (first) {
                    outfile << " " << j;
                    first = false;
                }
                else
                {
                    outfile << ", " << j;
                }
            }
        }
        outfile << std::endl;
    }
}

void Embedding::write_to_latex(int n,
                               Chimera &hardware,
                               std::string &filename) {
    std::ofstream outfile(filename + ".tex");

    /* Write header */
    outfile << "\\documentclass[12pt]{standalone}" << std::endl;
    outfile << "\\usepackage{tikz, amsmath, amssymb, graphics, setspace,"
            << "xcolor}"
            << std::endl;
    outfile << "\\usetikzlibrary{arrows,automata,positioning,shapes,fit}"
            << std::endl;
    outfile << "\\begin{document}" << std::endl;
    outfile << "\\begin{tikzpicture}" << std::endl;
    outfile << "\\tikzstyle{every state}=[fill=white,draw=black,text=black]"
            << std::endl;

    /* Write vertices */
    for (int row = 0; row < hardware.get_m(); row++)
    {
        for (int col = 0; col < hardware.get_n(); col++)
        {
            for (int vertex = 0; vertex < 2*hardware.get_c(); vertex++)
            {
                int vertexLabel = row*(hardware.get_n()*2*hardware.get_c())\
                    + col*(2*hardware.get_c()) + vertex;
                int x = col*5 + 2*(vertex/hardware.get_c());
                int y = row*9 + 2*(vertex%hardware.get_c());
                bool in_a_bag = false;
                for (int bag = 0; bag < n; bag++)
                {
                    if (contains(bag, vertexLabel))
                    {
                        in_a_bag = true;
                        outfile << "\\node [state] ("
                                << vertexLabel
                                << ") at ("
                                << x
                                << ", -"
                                << y
                                << ") {"
                                << bag
                                << "};"
                                << std::endl;
                        break;
                    }
                }
                if (!in_a_bag)
                {
                    outfile << "\\node [state] ("
                            << vertexLabel
                            << ") at ("
                            << x
                            << ", -"
                            << y
                            << ") {};"
                            << std::endl;
                }
            }
        }
    }

    /* Write inner cell edges */
    for (int cell = 0;
         cell < 2*hardware.get_c()*hardware.get_m()*hardware.get_n();
         cell += 2*hardware.get_c())
    {
        for (int left_vertex = 0; left_vertex < hardware.get_c(); left_vertex++)
        {
            for (int right_vertex = hardware.get_c();
                 right_vertex < 2*hardware.get_c();
                 right_vertex++)
            {
                outfile << "\\path ("
                        << cell + left_vertex
                        << ") edge ("
                        << cell + right_vertex
                        << ");"
                        << std::endl;
            }
        }
    }

    /* Write vertical edges */
    for (int row = 0; row < hardware.get_m()-1; row++)
    {
        for (int col = 0; col < hardware.get_n(); col++)
        {
            for (int vertex = 0; vertex < hardware.get_c(); vertex++)
            {
                int u = col*2*hardware.get_c() +\
                    row*2*hardware.get_c()*hardware.get_n() + vertex;
                int v = col*2*hardware.get_c() +\
                    (row+1)*2*hardware.get_c()*hardware.get_n() + vertex;
                outfile << "\\path [bend right] ("
                        << u
                        << ") edge ("
                        <<  v
                        << ");"
                        << std::endl;
            }
        }
    }

    /* Write horizontal edges */
    for (int row = 0; row < hardware.get_m(); row++)
    {
        for (int col = 0; col < hardware.get_n()-1; col++)
        {
            for (int vertex = hardware.get_c();
                 vertex < 2*hardware.get_c();
                 vertex++)
            {
                int u = col*2*hardware.get_c() +\
                    row*2*hardware.get_c()*hardware.get_n() + vertex;
                int v = (col+1)*2*hardware.get_c() +\
                    row*2*hardware.get_c()*hardware.get_n() + vertex;
                outfile << "\\path [bend left] ("
                        << u
                        << ") edge ("
                        << v
                        << ");"
                        << std::endl;
            }
        }
    }

    /* Write footer */
    outfile << "\\end{tikzpicture}" << std::endl;
    outfile << "\\end{document}" << std::endl;

    outfile.close();
}


CMR_Embedding::CMR_Embedding(int input_g_n,
                             int input_h_n): Embedding(input_g_n, input_h_n)
{
    qubits_used = 0;
    number_of_bags = new int[input_g_n]();
}

CMR_Embedding::~CMR_Embedding()
{
    delete[] number_of_bags;
}

void CMR_Embedding::add_vertex(int bag, int vertex)
{
    if (!phi[bag * g_n + vertex])
    {
        phi[bag * g_n + vertex] = true;
        number_of_bags[vertex]++;
        qubits_used++;
    }
}

void CMR_Embedding::remove_vertex(int bag, int vertex)
{
    if (phi[bag * g_n + vertex])
    {
        phi[bag * g_n + vertex] = false;
        number_of_bags[vertex]--;
        qubits_used--;
    }
}

bool CMR_Embedding::contains(int bag, int vertex)
{
  return phi[bag * g_n + vertex];
}

void CMR_Embedding::clear_bag(int bag)
{
    for (int i = 0; i < g_n; i++) {
        remove_vertex(bag, i);
    }
}

void CMR_Embedding::clear()
{
    for (int i = 0; i < h_n; i++)
    {
        clear_bag(i);
    }
}

bool CMR_Embedding::is_valid()
{
    for (int i = 0; i < g_n; i++)
    {
        if (number_of_bags[i] > 1)
        {
            return false;
        }
    }
    return true;
}

int CMR_Embedding::get_qubits_used()
{
    return qubits_used;
}

int CMR_Embedding::get_number_of_bags(int vertex)
{
    return number_of_bags[vertex];
}

int CMR_Embedding::get_max_number_of_bags()
{
    int result = 0;
    for (int i = 0; i < g_n; i++)
    {
        if (number_of_bags[i] > result)
        {
            result = number_of_bags[i];
        }
    }
    return result;
}
