#include <stdbool.h>
#include <SDL2/SDL.h>
#include "visual.h"
#include "utils.h"
#include "sorting.h"

// NOUVELLE FONCTION QUI GÈRE TOUS LES ÉVÉNEMENTS PENDANT UNE ANIMATION
void handle_animation_events() {
    SDL_Event event;

    // 1. On traite tous les événements en attente (pause, vitesse, quitter)
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                    is_paused = !is_paused;
                    break;
                case SDLK_PLUS:
                case SDLK_KP_PLUS:
                    if (animation_delay > 5) animation_delay -= 5;
                    else animation_delay = 1;
                    break;
                case SDLK_MINUS:
                case SDLK_KP_MINUS:
                    animation_delay += 5;
                    break;
            }
        }
    }

    // 2. Si la pause est activée, on entre dans une boucle d'attente
    while (is_paused && running) {
        // Cette boucle attend qu'on appuie à nouveau sur ESPACE ou qu'on quitte
        while (SDL_PollEvent(&event)) {
             if (event.type == SDL_QUIT) {
                 running = false;
                 is_paused = false; // Pour sortir de la boucle d'attente
             }
             if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                 is_paused = false; // Pour sortir de la boucle d'attente
             }
        }
        SDL_Delay(100); // Repos pour le CPU pendant la pause
    }
}


//
// 🔵 Tri à bulles (Bubble Sort)
//
void bubble_sort_step_by_step(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, Stats* stats) {
    stats->start_time = SDL_GetTicks();
    if (!running) return;

    for (int i = 0; i < size - 1 && running; i++) {
        for (int j = 0; j < size - i - 1 && running; j++) {
            stats->comparisons++;
            stats->memory_accesses += 2;

            if (array[j] > array[j + 1]) {
                float temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
                stats->memory_accesses += 4;
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            draw_array(renderer, array, size, window_width, window_height, j, j + 1);
            SDL_RenderPresent(renderer);

            handle_animation_events(); // <-- APPEL À LA NOUVELLE FONCTION
            SDL_Delay(animation_delay);
        }
    }

    stats->end_time = SDL_GetTicks();
}

//
// 🟢 Tri par sélection (Selection Sort)
//
void selection_sort_step_by_step(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, Stats* stats) {
    stats->start_time = SDL_GetTicks();
    if (!running) return;

    for (int i = 0; i < size - 1 && running; i++) {
        int min_index = i;

        for (int j = i + 1; j < size && running; j++) {
            stats->comparisons++;
            stats->memory_accesses += 2;

            if (array[j] < array[min_index]) {
                min_index = j;
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            draw_array(renderer, array, size, window_width, window_height, i, j);
            SDL_RenderPresent(renderer);
            handle_animation_events(); // On peut aussi écouter les événements ici
            SDL_Delay(animation_delay/4); // Délai plus court pour les comparaisons
        }

        if (min_index != i) {
            float temp = array[i];
            array[i] = array[min_index];
            array[min_index] = temp;
            stats->memory_accesses += 4;

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            draw_array(renderer, array, size, window_width, window_height, i, min_index);
            SDL_RenderPresent(renderer);

            handle_animation_events(); // <-- APPEL À LA NOUVELLE FONCTION
            SDL_Delay(animation_delay);
        }
    }

    stats->end_time = SDL_GetTicks();
}

//
// 🟡 Tri par insertion (Insertion Sort)
//
void insertion_sort_step_by_step(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, Stats* stats) {
    stats->start_time = SDL_GetTicks();
    if (!running) return;

    for (int i = 1; i < size && running; i++) {
        float key = array[i];
        stats->memory_accesses++;
        int j = i - 1;

        while (j >= 0 && array[j] > key && running) {
            stats->comparisons++;
            stats->memory_accesses += 2;
            array[j + 1] = array[j];
            stats->memory_accesses++;
            j--;

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            draw_array(renderer, array, size, window_width, window_height, j + 1, i);
            SDL_RenderPresent(renderer);
            handle_animation_events(); // <-- APPEL À LA NOUVELLE FONCTION
            SDL_Delay(animation_delay);
        }
        array[j + 1] = key;
        stats->memory_accesses++;
    }

    stats->end_time = SDL_GetTicks();
}


//
// 🔴 Tri rapide (QuickSort) - La fonction visuelle est celle qui doit être modifiée
//
void quicksort_visual(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, int a, int b) {
    if (!running) return;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    draw_array(renderer, array, size, window_width, window_height, a, b);
    SDL_RenderPresent(renderer);

    handle_animation_events(); // <-- APPEL À LA NOUVELLE FONCTION
    SDL_Delay(animation_delay);
}

void quicksort_step_by_step(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, int low, int high, Stats* stats) {
    if (low < high && running) {
        float pivot = array[high];
        stats->memory_accesses++;
        int i = low - 1;

        for (int j = low; j < high && running; j++) {
            stats->comparisons++;
            stats->memory_accesses++;
            if (array[j] < pivot) {
                i++;
                float temp = array[i];
                array[i] = array[j];
                array[j] = temp;
                stats->memory_accesses += 4;
            }
            quicksort_visual(renderer, array, size, window_width, window_height, j, i);
        }

        float temp = array[i + 1];
        array[i + 1] = array[high];
        array[high] = temp;
        stats->memory_accesses += 4;
        int pi = i + 1;

        quicksort_visual(renderer, array, size, window_width, window_height, pi, high);
        quicksort_step_by_step(renderer, array, size, window_width, window_height, low, pi - 1, stats);
        quicksort_step_by_step(renderer, array, size, window_width, window_height, pi + 1, high, stats);
    }
}


//
// 🟣 Tri fusion (MergeSort) - La fonction visuelle est celle qui doit être modifiée
//
void mergesort_visual(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, int left, int right) {
    if (!running) return;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    draw_array(renderer, array, size, window_width, window_height, left, right);
    SDL_RenderPresent(renderer);

    handle_animation_events(); // <-- APPEL À LA NOUVELLE FONCTION
    SDL_Delay(animation_delay);
}

void mergesort_step_by_step(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, int left, int right, Stats* stats) {
    if (left < right && running) {
        int mid = left + (right - left) / 2;
        mergesort_step_by_step(renderer, array, size, window_width, window_height, left, mid, stats);
        mergesort_step_by_step(renderer, array, size, window_width, window_height, mid + 1, right, stats);

        int n1 = mid - left + 1;
        int n2 = right - mid;
        float L[n1], R[n2];

        for (int i = 0; i < n1; i++) L[i] = array[left + i];
        for (int j = 0; j < n2; j++) R[j] = array[mid + 1 + j];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2 && running) {

            if (L[i] <= R[j]) {
                array[k] = L[i++];

            } else {
                array[k] = R[j++];
            }
            mergesort_visual(renderer, array, size, window_width, window_height, k, k);
            k++;
        }
        while (i < n1 && running) {
            array[k] = L[i++];
            mergesort_visual(renderer, array, size, window_width, window_height, k, k);
            k++;
        }
        while (j < n2 && running) {
            array[k] = R[j++];
            mergesort_visual(renderer, array, size, window_width, window_height, k, k);
            k++;
        }
    }
}