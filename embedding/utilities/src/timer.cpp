#include "../include/timer.h"

void Timer::start_timer()
{
    gettimeofday(&start, NULL);
}

void Timer::stop_timer()
{
    gettimeofday(&stop, NULL);
}

double Timer::get_time_passed()
{
    return (stop.tv_sec - start.tv_sec) + \
        (stop.tv_usec - start.tv_usec) / 1000000.0f;
}

void Timer::write_to_file(std::string &filename)
{
    std::ofstream outfile(filename + ".timing");
    outfile << get_time_passed() << std::endl;
    outfile.close();
}
