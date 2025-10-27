#define TABS_IMPLEMENTATION
#include "tabs.h"
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 1000000000
#define NUM_THREADS 4

typedef struct {
    int* array;
    int start_index;
    int end_index;
    long long partial_sum;
} thread_data_t;

TABS_THREAD_RETURN_TYPE sum_worker(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    long long sum = 0;

    printf("Thread processing elements %d to %d\n", data->start_index, data->end_index - 1);

    for (int i = data->start_index; i < data->end_index; i++) {
        sum += data->array[i];
    }

    data->partial_sum = sum;
    printf("Thread finished with partial sum: %lld\n", sum);

    TABS_THREAD_RETURN;
}

int main() {
    int* array = (int*)malloc(ARRAY_SIZE * sizeof(int));
    if (!array) {
        printf("Failed to allocate array\n");
        return 1;
    }

    printf("Initializing array with %d elements...\n", ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = i + 1;
    }

    tabs_thread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    int chunk_size = ARRAY_SIZE / NUM_THREADS;

    printf("\nStarting %d threads to sum the array...\n\n", NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].array = array;
        thread_data[i].start_index = i * chunk_size;
        thread_data[i].end_index = (i == NUM_THREADS - 1) ? ARRAY_SIZE : (i + 1) * chunk_size;
        thread_data[i].partial_sum = 0;

        if (tabs_create_thread(&threads[i], sum_worker, &thread_data[i]) != TABS_SUCCESS) {
            printf("Failed to create thread %d\n", i);
            free(array);
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        tabs_join_thread(threads[i], NULL);
    }

    long long total_sum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        total_sum += thread_data[i].partial_sum;
    }


    long long expected = ((long long)ARRAY_SIZE * (ARRAY_SIZE + 1)) / 2;
    printf("\n----------------------------------------\n");
    printf("Total sum: %lld\n", total_sum);
    printf("Expected:  %lld\n", expected);
    printf("Result:    %s\n", (total_sum == expected) ? "CORRECT" : "INCORRECT");
    printf("----------------------------------------\n");

    free(array);
    return 0;
}