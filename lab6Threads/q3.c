#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_THREADS 5

bool hasRun[NUM_THREADS] = {false};

void runMe(int32_t *arg) {
    int32_t value = (*arg);
    assert(value >= 0 && value < NUM_THREADS && "Bad argument passed to 'runMe()!'");
    hasRun[value] = 1;
    int32_t *ret = (int32_t*)malloc(sizeof(int32_t));
    *ret = value * value;
    pthread_exit((void*)ret);
}

int32_t runThreads(size_t n) {
    // You need to write this.
    pthread_t threads[NUM_THREADS];
    int32_t args[NUM_THREADS];
    int32_t sum = 0;
    for (size_t i = 0; i < n; i++) {
        args[i] = i;
        if (pthread_create(&threads[i], NULL, (void*(*)(void*))runMe, (void*)&args[i]) != 0) {
            perror("create");
            exit(1);
        }
    }
    for (size_t i = 0; i < n; i++) {
        void *retval;
        if (pthread_join(threads[i], &retval) != 0) {
            perror("join");
            exit(1);
        }
        int32_t value = *((int32_t*)retval);
        free(retval);
        sum += value;
    }
    return sum;
}

int main (void) {
    int32_t sum = runThreads(NUM_THREADS);
    size_t numCorrect = 0;
    for (size_t i = 0; i < NUM_THREADS; i++) {
        if (hasRun[i]) {
            numCorrect++;
        }
    }
    printf("%zu %d\n", numCorrect, sum);
    return 0;
}