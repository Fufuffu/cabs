#define TABS_IMPLEMENTATION
#include "tabs.h"
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 4
#define INCREMENTS_PER_THREAD 1000000

int shared_counter = 0;
tabs_mutex_t counter_mutex;

typedef struct {
    int thread_id;
} thread_data_t;

TABS_THREAD_RETURN_TYPE counter_worker(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;

    printf("Thread %d starting...\n", data->thread_id);

    for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
        tabs_mutex_lock(&counter_mutex);
        shared_counter++;
        tabs_mutex_unlock(&counter_mutex);
    }

    printf("Thread %d finished\n", data->thread_id);
    TABS_THREAD_RETURN;
}

int main() {
    tabs_thread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];

    if (tabs_mutex_init(&counter_mutex) != TABS_SUCCESS) {
        printf("Failed to initialize mutex\n");
        return 1;
    }

    printf("Threads: %d\n", NUM_THREADS);
    printf("Increments per thread: %d\n", INCREMENTS_PER_THREAD);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;

        if (tabs_create_thread(&threads[i], counter_worker, &thread_data[i]) != TABS_SUCCESS) {
            printf("Failed to create thread %d\n", i);
            tabs_mutex_destroy(&counter_mutex);
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        tabs_join_thread(threads[i], NULL);
    }

    int expected = NUM_THREADS * INCREMENTS_PER_THREAD;
    printf("\n----------------------------------------\n");
    printf("Final count: %d\n", shared_counter);
    printf("Expected:    %d\n", expected);
    printf("Difference:  %d\n", expected - shared_counter);
    printf("Result:      %s\n", (shared_counter == expected) ? "CORRECT" : "INCORRECT");
    printf("----------------------------------------\n");

    tabs_mutex_destroy(&counter_mutex);

    return 0;
}
