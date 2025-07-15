/*
 * numSort.c - A mostly empty file. Write a program that sorts an array of numbers (the type doesn't really matter) using the library function qsort(). 
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Comparison function for qsort
 * Returns negative if a < b, zero if a == b, positive if a > b
 */
int compare(const void *a, const void *b) {
    int32_t int_a = *(const int32_t *)a;
    int32_t int_b = *(const int32_t *)b;

    if (int_a < int_b) return -1;
    if (int_a > int_b) return 1;
    return 0;
}

/**
 * Sorts array of int32s using qsort
 */
void sort(int32_t *arr, size_t n) {
    qsort(arr, n, sizeof(int32_t), compare);
}

/**
 * Prints an array of int32s
 */
void printArray(int32_t *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main(void) {
    int32_t numbers[] = {5, 3, 8, 6, 2, 7, 4, 1};
    size_t n = sizeof(numbers) / sizeof(numbers[0]);
    sort(numbers, n);
    printArray(numbers, n);
    return 0;
}
