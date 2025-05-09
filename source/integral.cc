#include "integral.hh"
#include <vector>

int main(int argc, char* argv[]) try {
    if (argc != 4) {
        std::cout << "Bad args" << std::endl;
        return 1;
    }
    double left = std::stod(argv[1]);
    double right = std::stod(argv[2]);
    integral::sdat.eps = std::stod(argv[3]);

    sem_init(&integral::sdat.sem_stk, 1, 1);
    sem_init(&integral::sdat.sem_task_present, 1, 1);

    double f_left = integral::func(left);
    double f_right = integral::func(right);

    integral::sdat.global_stk.emplace(left, right, f_left, f_right,
                     (f_left + f_right) * (right - left) / 2);
    
    // double res = integral::serial_integral_with_local_stack(-10, 10, 0.00001);

    // std::cout <<  res * res << std::endl;

    std::vector<pthread_t> thr(4);

    for (int i = 0; i < 4; ++i) {
        int ret_code = pthread_create(&thr[i], NULL, integral::thread_integral, NULL);

        if (ret_code) {
            std::cerr << "error: pthread_create, ret_code: " << ret_code << std::endl;
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < 4; ++i) {
        pthread_join(thr[i], NULL);
    }

    return 0;
} 
catch (const std::exception& exc) {
    std::cerr << "Something wrong: " << exc.what() << std::endl;
}