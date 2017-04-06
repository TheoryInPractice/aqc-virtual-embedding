#ifndef UTILITIES_TIMER_H_
#define UTILITIES_TIMER_H_

#include <fstream>
#include <sys/time.h>

class Timer
{
 public:
    void start_timer();
    void stop_timer();
    double get_time_passed();
    void write_to_file(std::string &filename);

 private:
    struct timeval start, stop;
};

#endif
