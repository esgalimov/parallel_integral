#include "integral.hh"
#include <chrono>

int main(int argc, char* argv[]) try {
    if (argc != 4) {
        std::cout << "Bad args" << std::endl;
        return 1;
    }
    double left = std::stod(argv[1]);
    double right = std::stod(argv[2]);
    integral::sdat.eps = std::stod(argv[3]);

    sem_init(&integral::sdat.sem_stk, 1, 1);
    sem_init(&integral::sdat.sem_ans, 1, 1);
    sem_init(&integral::sdat.sem_task_present, 1, 1);

    double step = (right - left) / 16;

    for (int i = 0; i < 16; ++i) {
        double curr_left = left + step * i;
        double curr_right = left + step * (i + 1);

        double f_left = integral::func(left);
        double f_right = integral::func(curr_right);

        integral::sdat.global_stk.emplace(curr_left, curr_right, f_left, f_right,
                     (f_left + f_right) * (curr_right - curr_left) / 2);
    }

    std::vector<pthread_t> thr(4);
    std::vector<int> tids(4);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 4; ++i) {
        tids[i] = i;
        int ret_code = pthread_create(&thr[i], NULL, integral::thread_integral, &tids[i]);

        if (ret_code) {
            std::cerr << "error: pthread_create, ret_code: " << ret_code << std::endl;
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < 4; ++i) {
        pthread_join(thr[i], NULL);
    }

    double tm = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - start).count();

    sem_destroy(&integral::sdat.sem_stk);
    sem_destroy(&integral::sdat.sem_ans);
    sem_destroy(&integral::sdat.sem_task_present);

    std::cout.precision(20);

    std::cout << "ANSWER = " << integral::sdat.ans << " TIME = " << tm << " us" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    double res = integral::serial_integral_with_local_stack(left, right, integral::sdat.eps);
    tm = std::chrono::duration_cast<std::chrono::microseconds>(
         std::chrono::high_resolution_clock::now() - start).count();

    std::cout << "ANSWER = " << res << " TIME = " << tm << " us" << std::endl;

    return 0;
} 
catch (const std::exception& exc) {
    std::cerr << "Something wrong: " << exc.what() << std::endl;
}