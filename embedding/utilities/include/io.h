#ifndef UTILITIES_IO_H_
#define UTILITIES_IO_H_

#include <fstream>
#include "graph.h"

char* get_cmd_option(char ** begin, char ** end, const std::string & option);
bool cmd_option_exists(char** begin, char** end, const std::string& option);
void print_error(int e, std::string flag);
bool help_flag(int argc, char *argv[]);
bool read_int(int argc, char *argv[], std::string flag, int &n);
bool read_string(int argc,
                 char *argv[],
                 std::string flag,
                 std::string &filename);
void read_program(int argc, char *argv[], int &n);
Graph *read_program(int argc, char *argv[]);
Hardware *read_hardware(int argc, char *argv[]);
Chimera *read_chimera(int argc, char *argv[]);
void read_cmr_inputs(int argc,
                     char *argv[],
                     Hardware **hardware,
                     Chimera **chimera,
                     Graph **program,
                     int &seed,
                     std::string &outfile);
void read_complete_inputs(int argc,
                          char *argv[],
                          int &n,
                          Chimera **chimera,
                          std::string &outfile);
void read_oct_inputs(int argc,
                     char *argv[],
                     Graph **program,
                     Chimera **chimera,
                     std::string &outfile);
void read_oct_inputs(int argc,
                     char *argv[],
                     Graph **program,
                     Chimera **chimera,
                     int &k,
                     std::string &outfile);
void read_oct_inputs(int argc,
                     char *argv[],
                     Graph **program,
                     Chimera **chimera,
                     int &seed,
                     int &repeats,
                     std::string &outfile);
#endif
