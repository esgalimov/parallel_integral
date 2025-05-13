#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <chrono>
#include <iostream>

constexpr int BUF_SIZE = 10000;
constexpr int N_ROUNDS = 1000;

char  buf[BUF_SIZE];
char  send_str[BUF_SIZE];
int   has_ping = 0;
int   has_pong = 0;
pthread_mutex_t mtx      = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_ping = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_pong = PTHREAD_COND_INITIALIZER;


void *thread_A(void *arg) {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N_ROUNDS; i++) {
        pthread_mutex_lock(&mtx);
        memcpy(buf, send_str, BUF_SIZE);
        has_ping = 1;
        pthread_cond_signal(&cond_ping);

        while (!has_pong)
            pthread_cond_wait(&cond_pong, &mtx);

        //printf("[A] received: %s\n", buf);
        has_pong = 0;
        pthread_mutex_unlock(&mtx);
    }

    double tm = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - start).count();
    
    std::cout << tm / N_ROUNDS << " us" << std::endl;

    return NULL;
}

void *thread_B(void *arg) {
    for (int i = 0; i < N_ROUNDS; i++) {
        pthread_mutex_lock(&mtx);
        while (!has_ping)
            pthread_cond_wait(&cond_ping, &mtx);

        //printf("[B] received: %s\n", buf);
        has_ping = 0;

        memcpy(buf, send_str, BUF_SIZE);
        has_pong = 1;
        pthread_cond_signal(&cond_pong);
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

int main(void) {
    for (int i = 0; i < BUF_SIZE - 1; ++i) {
        send_str[i] = 's';
    }
    send_str[BUF_SIZE - 1] = '\0';
    
    pthread_t A, B;
    
    pthread_create(&A, NULL, thread_A, NULL);
    pthread_create(&B, NULL, thread_B, NULL);
    

    pthread_join(A, NULL);
    pthread_join(B, NULL);
    return 0;
}