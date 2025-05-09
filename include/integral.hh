#ifndef MY_INTEGRAL
#define MY_INTEGRAL

#include <cmath>
#include <iostream>
#include <stack>
#include <vector>
#include <stdexcept>

#include <pthread.h>
#include <semaphore.h>

namespace integral {

struct task_t {
    double left;
    double right;
    double f_left;
    double f_right;
    double s_left_right;
};

constexpr int MAX_ELEMS_LOCAL_STK = 16;

struct sdat_t
{
    std::stack<integral::task_t> global_stk;
    sem_t sem_stk;
    sem_t sem_ans;
    sem_t sem_task_present;
    int nactive = 0;
    double eps = 0.000'000'1;
    double ans = 0.0;
} sdat;


double func(double x) {
    //return std::exp(-x * x);
    return std::sin(1 / x);
    //return 1;
}

void* thread_integral(void* arg) {
    int* tid = static_cast<int*>(arg);
    std::stack<task_t> stk;

    double sum = 0.0;

    sem_wait(&sdat.sem_task_present);
    sem_wait(&sdat.sem_stk);

    task_t tsk = sdat.global_stk.top();
    sdat.global_stk.pop();

    if (!sdat.global_stk.empty())
        sem_post(&sdat.sem_task_present);

    sem_post(&sdat.sem_stk);

    double left = tsk.left;
    double right = tsk.right;
    double f_left = tsk.f_left;
    double f_right = tsk.f_right;
    double s_left_right = tsk.s_left_right;

    while (1) {
        double middle = (left + right) / 2;
        double f_middle = func(middle);

        double s_left_middle = (f_left + f_middle) * (middle - left) / 2;
        double s_middle_right = (f_middle + f_right) * (right - middle) / 2;

        double s_all = s_left_middle + s_middle_right;

        if (std::abs(s_all - s_left_right) >= sdat.eps) {
            stk.emplace(left, middle, f_left, f_middle, s_left_middle);

            left = middle;
            f_left = f_middle;
            s_left_right = s_middle_right;
        }
        else {
            sum += s_all;

            if (stk.empty()) {
                //std::cout << "cock1" << std::endl;
                sem_wait(&sdat.sem_task_present);
                sem_wait(&sdat.sem_stk);

                int get_global_cnt = sdat.global_stk.size() <= 4 ? 1 : sdat.global_stk.size() / 4;
                bool is_terminal = false;

                for (int i = 0; i < get_global_cnt; ++i) {
                    task_t tsk = sdat.global_stk.top();
                    sdat.global_stk.pop();
                    stk.push(tsk);
                    if (tsk.left > tsk.right) is_terminal = true;
                }

                if (!sdat.global_stk.empty())
                    sem_post(&sdat.sem_task_present);

                if (!is_terminal) sdat.nactive++;

                sem_post(&sdat.sem_stk);

                if (is_terminal) { std::cout << "break " << sum << std::endl; break; }
                
                sem_wait(&sdat.sem_stk);
                sdat.nactive--;

                if (!sdat.nactive && sdat.global_stk.empty()) {
                    for (int i = 0; i < 4; ++i)
                        sdat.global_stk.emplace(2, 1, 0, 0, 0);

                    sem_post(&sdat.sem_task_present);
                }
                sem_post(&sdat.sem_stk);
            }
            task_t tsk = stk.top();
            stk.pop();

            left = tsk.left;
            right = tsk.right;
            f_left = tsk.f_left;
            f_right = tsk.f_right;
            s_left_right = tsk.s_left_right;
        }

        int stk_sz = stk.size();

        sem_wait(&sdat.sem_stk);
        if (stk_sz > MAX_ELEMS_LOCAL_STK && sdat.global_stk.empty()) {
            while (stk.size() > stk_sz / 4) {
                task_t tsk = stk.top();
                stk.pop();
                sdat.global_stk.push(tsk);
            }
            sem_post(&sdat.sem_task_present);
        }
        sem_post(&sdat.sem_stk);

    }

    sem_wait(&sdat.sem_ans);
    sdat.ans += sum;
    sem_post(&sdat.sem_ans);
    return NULL;
}

double serial_integral_with_local_stack(double left, double right, double eps) {
    std::stack<task_t> stk;

    double sum = 0.0;

    double f_left = func(left);
    double f_right = func(right);

    double s_left_right = (f_left + f_right) * (right - left) / 2;

    while (1) {
        double middle = (left + right) / 2;
        double f_middle = func(middle);

        double s_left_middle = (f_left + f_middle) * (middle - left) / 2;
        double s_middle_right = (f_middle + f_right) * (right - middle) / 2;

        double s_all = s_left_middle + s_middle_right;

        if (std::abs(s_all - s_left_right) >= eps) {
            stk.emplace(left, middle, f_left, f_middle, s_left_middle);

            left = middle;
            f_left = f_middle;
            s_left_right = s_middle_right;
        }
        else {
            sum += s_all;

            if (stk.empty()) break;

            task_t tsk = stk.top();
            stk.pop();

            left = tsk.left;
            right = tsk.right;
            f_left = tsk.f_left;
            f_right = tsk.f_right;
            s_left_right = tsk.s_left_right;
        }
    }

    return sum;
}

} // <---- namespace integral 

#endif