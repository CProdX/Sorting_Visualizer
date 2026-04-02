#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "utils.h"
#include "visual.h"
#include "sorting.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define ARRAY_SIZE 100

// Définition des variables globales
bool is_paused = false;
int animation_delay = 20;
bool running = true;

int main() {
    // Initialisation SDL
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    // Chargement de la police système
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);
    if (!font) {
        SDL_Log("Erreur chargement police: %s", TTF_GetError());
        return 1;
    }

    // Création de la fenêtre
    SDL_Window* window = SDL_CreateWindow("Sorting Visualizer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    // Création du renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Variables locales
    float array[ARRAY_SIZE];
    Student students[ARRAY_SIZE];
    ArrayMode current_mode = MODE_RANDOM;
    Stats stats = {0, 0, 0, 0};
    bool sorted = false;
    bool mode_selected = false;
    SDL_Event event;
    bool is_student_mode = false;

    // Boucle principale
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;

            if (event.type == SDL_KEYDOWN) {
                // ===================================================================
                // NOUVEAU BLOC : Gérer la réinitialisation après un tri
                // ===================================================================
                if (sorted && event.key.keysym.sym == SDLK_r) {
                    // Réinitialiser tous les états pour revenir au début
                    mode_selected = false;
                    sorted = false;
                    is_student_mode = false;
                    stats = (Stats){0, 0, 0, 0}; // Remettre les stats à zéro

                    // On passe à l'événement suivant, l'écran sera rafraîchi avec les instructions
                    continue;
                }

                // (Le code pour la pause et la vitesse a été déplacé dans sorting.c)

                // Choix du mode de génération
                if (!mode_selected) {
                    switch (event.key.keysym.sym) {
                        case SDLK_1: current_mode = MODE_RANDOM; mode_selected = true; break;
                        case SDLK_2: current_mode = MODE_SORTED; mode_selected = true; break;
                        case SDLK_3: current_mode = MODE_REVERSED; mode_selected = true; break;
                        case SDLK_4: current_mode = MODE_ALMOST_SORTED; mode_selected = true; break;
                        case SDLK_5: current_mode = MODE_DUPLICATES; mode_selected = true; break;
                        case SDLK_6: current_mode = MODE_PYRAMID; mode_selected = true; break;
                        case SDLK_8:
                            is_student_mode = true;
                            mode_selected = true;
                            break; // La génération se fait plus bas
                    }
                    if (mode_selected) {
                        if (is_student_mode) {
                            generate_students(students, ARRAY_SIZE);
                        } else {
                            generate_array(array, ARRAY_SIZE, WINDOW_HEIGHT, current_mode);
                        }
                    }
                }

                // Choix de l'algorithme pour tableaux
                if (mode_selected && !sorted && !is_student_mode) {
                    stats = (Stats){0, 0, 0, 0};
                    switch (event.key.keysym.sym) {
                        case SDLK_b:
                            bubble_sort_step_by_step(renderer, array, ARRAY_SIZE, WINDOW_WIDTH, WINDOW_HEIGHT, &stats);
                            sorted = true;
                            break;
                        case SDLK_s:
                            selection_sort_step_by_step(renderer, array, ARRAY_SIZE, WINDOW_WIDTH, WINDOW_HEIGHT, &stats);
                            sorted = true;
                            break;
                        case SDLK_i:
                            insertion_sort_step_by_step(renderer, array, ARRAY_SIZE, WINDOW_WIDTH, WINDOW_HEIGHT, &stats);
                            sorted = true;
                            break;
                        case SDLK_q:
                            stats.start_time = SDL_GetTicks();
                            quicksort_step_by_step(renderer, array, ARRAY_SIZE, WINDOW_WIDTH, WINDOW_HEIGHT, 0, ARRAY_SIZE - 1, &stats);
                            stats.end_time = SDL_GetTicks();
                            sorted = true;
                            break;
                        case SDLK_m:
                            stats.start_time = SDL_GetTicks();
                            mergesort_step_by_step(renderer, array, ARRAY_SIZE, WINDOW_WIDTH, WINDOW_HEIGHT, 0, ARRAY_SIZE - 1, &stats);
                            stats.end_time = SDL_GetTicks();
                            sorted = true;
                            break;
                    }
                }

                // Choix algorithme pour etudiant
                if (mode_selected && is_student_mode) {
                    switch (event.key.keysym.sym) {
                        case SDLK_a:
                            generic_sort(students, ARRAY_SIZE, compare_by_age);
                            break;
                        case SDLK_g:
                            generic_sort(students, ARRAY_SIZE, compare_by_grade);
                            break;
                        case SDLK_n:
                            generic_sort(students, ARRAY_SIZE, compare_by_name);
                            break;
                    }
                }
            }
        }

        // Affichage
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (!mode_selected) {
            draw_instructions(renderer, font);
        } else {
            if (is_student_mode) {
                draw_students(renderer, font, students, ARRAY_SIZE);
            } else {
                draw_array(renderer, array, ARRAY_SIZE, WINDOW_WIDTH, WINDOW_HEIGHT, -1, -1);
                draw_stats(renderer, font, stats);
            }
        }

        SDL_RenderPresent(renderer);
    } // FIN BOUCLE

    // Nettoyage
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}