#ifndef SORTING_H
#define SORTING_H

#include <SDL2/SDL.h>
#include "utils.h"  // For Stats

// Bubble Sort
void bubble_sort_step_by_step(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, Stats* stats);

// Selection Sort
void selection_sort_step_by_step(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, Stats* stats);

// Insertion Sort
void insertion_sort_step_by_step(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, Stats* stats);

// QuickSort
void quicksort_step_by_step(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, int low, int high, Stats* stats);

// MergeSort
void mergesort_step_by_step(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, int left, int right, Stats* stats);


#endif
