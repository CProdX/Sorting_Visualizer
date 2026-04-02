#ifndef SORTING_H
#define SORTING_H

#include <SDL2/SDL.h>
#include "utils.h"  // Pour Stats

// Tri à bulles
void bubble_sort_step_by_step(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, Stats* stats);

// Tri par sélection
void selection_sort_step_by_step(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, Stats* stats);

// Tri par insertion
void insertion_sort_step_by_step(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, Stats* stats);

// Tri rapide
void quicksort_step_by_step(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, int low, int high, Stats* stats);

// Tri fusion
void mergesort_step_by_step(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, int left, int right, Stats* stats);


#endif
