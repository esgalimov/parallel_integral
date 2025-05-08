#ifndef MY_STUPID_INTEGRAL
#define MY_STUPID_INTEGRAL


#include <iostream>
#include <cmath>
#include <vector>
#include <pthread.h>

namespace stupid_integral {

struct integral_thread_t {
    double start;
    double finish;

    int n_steps;
    int tid;

    std::vector<double>* res_vec;
};

double func(double x) { 
    return std::exp(-x * x);
    //return std::sin(1 / x); 
}

void* thread_integral(void* arg) {
    integral_thread_t* data = static_cast<integral_thread_t*>(arg);

    double result = 0.0;
    double pos = data->start;
    double step = (data->finish - data->start) / data->n_steps;

    for (int i = 0; i < data->n_steps; ++i) {
        result += func(pos) * step;
        pos += step;
    }
    (*data->res_vec)[data->tid] = result;

    pthread_exit(NULL);
}

double get_bounder(double start, double finish, int tid, int thr_num) {
    double step = (finish - start) / thr_num;
    return start + step * (tid + 1);
}


} // <---- namespace integral 

#endif