#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "visual.h"
#include "utils.h"

void draw_array(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, int highlight1, int highlight2) {
    int bar_width = window_width / size;

    for (int i = 0; i < size; i++) {
        int height = (int)array[i];
        SDL_Rect bar = {
            .x = i * bar_width,
            .y = window_height - height,
            .w = bar_width - 1,
            .h = height
        };

        if (i == highlight1 || i == highlight2)
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // rouge
        else
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // vert

        SDL_RenderFillRect(renderer, &bar);
    }
}

void draw_stats(SDL_Renderer* renderer, TTF_Font* font, Stats stats) {
    char buffer[128];
    SDL_Color white = {255, 255, 255, 255};

    snprintf(buffer, sizeof(buffer), "Comparaisons: %d | Acces memoire: %d | Temps: %d ms",
             stats.comparisons, stats.memory_accesses, stats.end_time - stats.start_time);

    SDL_Surface* surface = TTF_RenderText_Solid(font, buffer, white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dst = {10, 10, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void draw_instructions(SDL_Renderer* renderer, TTF_Font* font) {
    const char* lines[] = {
        "Veuillez choisir le type de tableau :",
        "1 - Aleatoire",
        "2 - Deja trie",
        "3 - Inverse",
        "4 - Presque trie",
        "5 - Doublons",
        "6 - Pyramide",
        "8 - GENERER  DES ETUDIANTS (age,nom,grade)",
    };

    SDL_Color white = {255, 255, 255, 255};
    int y = 20;

    for (int i = 0; i < 8; i++) {
        SDL_Surface* surface = TTF_RenderText_Solid(font, lines[i], white);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect dst = {20, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &dst);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        y += 30;
    }
}
void draw_students(SDL_Renderer* renderer, TTF_Font* font, Student* array, int size) {
    SDL_Color white = {255, 255, 255, 255};
    int y = 20;
    char buffer[128];

    for (int i = 0; i < size; i++) {
        snprintf(buffer, sizeof(buffer), "%s - Age: %d - Grade: %.1f",
                 array[i].name, array[i].age, array[i].grade);

        SDL_Surface* surface = TTF_RenderText_Solid(font, buffer, white);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect dst = {20, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &dst);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        y += 25;
    }
}