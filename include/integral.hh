#ifndef MY_INTEGRAL
#define MY_INTEGRAL

#include <cmath>
#include <iostream>
#include <stack>

namespace integral {

struct task_t {
    double left;
    double right;
    double f_left;
    double f_right;
    double s_left_right;
};

double func(double x) {
    return std::exp(-x * x);
}


double integral_with_local_stack(double left, double right, double eps) {
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

        if (std::abs(s_all - s_left_right) >= eps * s_all) {
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