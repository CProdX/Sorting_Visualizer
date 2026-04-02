#ifndef VISUAL_H
#define VISUAL_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "utils.h"

void draw_button(SDL_Renderer* renderer, TTF_Font* font, const char* label, SDL_Rect rect, bool highlighted);
void draw_instructions(SDL_Renderer* renderer, TTF_Font* font);
void draw_array_in_rect(SDL_Renderer* renderer, float* array, int size, SDL_Rect rect);
void draw_compare_panel(SDL_Renderer* renderer, TTF_Font* font, const char* title, float* array, int size, Stats stats, SDL_Rect rect);
void draw_compare_background(SDL_Renderer* renderer);

void draw_array(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, int highlight1, int highlight2);
void draw_stats(SDL_Renderer* renderer, TTF_Font* font, Stats stats);
void draw_students(SDL_Renderer* renderer, TTF_Font* font, Student* array, int size);

#endif
