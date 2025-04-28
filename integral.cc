#include "integral.hh"



int main(int argc, char* argv[]) {

    int thr_num = 4;

    double start = 1;
    double finish = 2;
    double n_steps = 100'000;

    std::vector<double>* res_vec(thr_num);

    std::vector<pthread_t> thr(thr_num);
    std::vector<integral_thread_t> it_data(thr_num);
    
    for (int i = 0; i < thr_num; ++i) {
        it_data[i].tid = i;
        it_data[i].res_vec = &res_vec;

    }

    pthread_barrier_init(&parallel_sorter::barrier, NULL, thr_num);

    for (int i = 0; i < thr_num; ++i) {
        int ret_code = pthread_create(&thr[i], NULL, parallel_sorter::thread_sort, &sp[i]);

        if (ret_code) {
            std::cerr << "error: pthread_create, ret_code: " << ret_code << std::endl;
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < thr_num; ++i) {
        pthread_join(thr[i], NULL);
    }


    return EXIT_SUCCESS;
}