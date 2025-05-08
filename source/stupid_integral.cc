#include "stupid_integral.hh"



int main(int argc, char* argv[]) {

    int thr_num = 4;

    double start = -10;
    double finish = 10;
    int n_steps = 1'000'000;

    std::vector<double> res_vec(thr_num);

    std::vector<pthread_t> thr(thr_num);
    std::vector<integral::integral_thread_t> it_data(thr_num);
    
    for (int i = 0; i < thr_num; ++i) {
        it_data[i].tid = i;
        it_data[i].res_vec = &res_vec;
        it_data[i].start  = integral::get_bounder(start, finish, i - 1, thr_num);;
        it_data[i].finish = integral::get_bounder(start, finish, i, thr_num);
        it_data[i].n_steps = n_steps / thr_num;
    }

    for (int i = 0; i < thr_num; ++i) {
        int ret_code = pthread_create(&thr[i], NULL, integral::thread_integral, &it_data[i]);

        if (ret_code) {
            std::cerr << "error: pthread_create, ret_code: " << ret_code << std::endl;
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < thr_num; ++i) {
        pthread_join(thr[i], NULL);
    }

    double sum = 0.0;
    for (auto& elem : res_vec) { 
        std::cout << elem << " ";
        sum += elem; 
    }
    std::cout << std::endl;

    std::cout << "SUM = " << sum * sum << std::endl;

    return EXIT_SUCCESS;
}