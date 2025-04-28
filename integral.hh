#ifndef MY_INTEGRAL
#define MY_INTEGRAL


#include <iostream>
#include <cmath>
#include <vector>
#include <pthread.h>

namespace integral {

struct integral_thread_t {
    double start;
    double finish;
    double n_steps;

    int tid;

    std::vector<double>* res_vec;
};

double func(double x) { return std::sin(1 / x); }

void* thread_sort(void* arg) {
    integral_thread_t* data = static_cast<integral_thread_t*>(arg);

    double result = 0.0;
    double pos = data->start;
    double step = (data->finish - data->start) / data->n_steps;

    for (int i = 0; i < data->n_steps; ++i) {
        result += pos * step;
        pos += step;
    }
    *data->res_vec[tid] = result;

    pthread_exit(NULL);
}


} // <---- namespace integral 

#endif