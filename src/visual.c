#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "visual.h"
#include "utils.h"

static void draw_vertical_gradient(SDL_Renderer* renderer, int width, int height, SDL_Color top, SDL_Color bottom) {
    for (int y = 0; y < height; y++) {
        float t = (height > 1) ? (float)y / (float)(height - 1) : 0.0f;
        Uint8 r = (Uint8)((1.0f - t) * top.r + t * bottom.r);
        Uint8 g = (Uint8)((1.0f - t) * top.g + t * bottom.g);
        Uint8 b = (Uint8)((1.0f - t) * top.b + t * bottom.b);
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, 0, y, width, y);
    }
}

void draw_compare_background(SDL_Renderer* renderer) {
    SDL_Color bg_top = {9, 20, 39, 255};
    SDL_Color bg_bottom = {16, 42, 74, 255};
    draw_vertical_gradient(renderer, 800, 600, bg_top, bg_bottom);
}

static void draw_label(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int x, int y) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void draw_button(SDL_Renderer* renderer, TTF_Font* font, const char* label, SDL_Rect rect, bool highlighted) {
    SDL_Color fill_top = highlighted ? (SDL_Color){33, 166, 255, 255} : (SDL_Color){54, 70, 102, 255};
    SDL_Color fill_bottom = highlighted ? (SDL_Color){20, 118, 207, 255} : (SDL_Color){35, 47, 72, 255};
    SDL_Color border = (SDL_Color){198, 220, 255, 255};
    SDL_Color text_color = (SDL_Color){255, 255, 255, 255};
    SDL_Rect shadow = {rect.x + 2, rect.y + 3, rect.w, rect.h};

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 90);
    SDL_RenderFillRect(renderer, &shadow);

    for (int y = 0; y < rect.h; y++) {
        float t = (rect.h > 1) ? (float)y / (float)(rect.h - 1) : 0.0f;
        Uint8 r = (Uint8)((1.0f - t) * fill_top.r + t * fill_bottom.r);
        Uint8 g = (Uint8)((1.0f - t) * fill_top.g + t * fill_bottom.g);
        Uint8 b = (Uint8)((1.0f - t) * fill_top.b + t * fill_bottom.b);
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, rect.x, rect.y + y, rect.x + rect.w, rect.y + y);
    }

    SDL_SetRenderDrawColor(renderer, border.r, border.g, border.b, border.a);
    SDL_RenderDrawRect(renderer, &rect);

    SDL_Surface* surface = TTF_RenderText_Blended(font, label, text_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect text_rect = {
        .x = rect.x + (rect.w - surface->w) / 2,
        .y = rect.y + (rect.h - surface->h) / 2,
        .w = surface->w,
        .h = surface->h
    };

    SDL_RenderCopy(renderer, texture, NULL, &text_rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void draw_array(SDL_Renderer* renderer, float* array, int size, int window_width, int window_height, int highlight1, int highlight2) {
    SDL_Color bg_top = {9, 20, 39, 255};
    SDL_Color bg_bottom = {16, 42, 74, 255};
    int margin_x = 10;
    int chart_top = 70;
    int chart_bottom = window_height - 45;
    int chart_height = chart_bottom - chart_top;
    int inner_width = window_width - (2 * margin_x);
    int bar_width = (size > 0) ? inner_width / size : inner_width;

    if (bar_width < 1) {
        bar_width = 1;
    }

    draw_vertical_gradient(renderer, window_width, window_height, bg_top, bg_bottom);

    SDL_SetRenderDrawColor(renderer, 130, 180, 245, 70);
    SDL_RenderDrawLine(renderer, margin_x, chart_bottom, window_width - margin_x, chart_bottom);

    for (int i = 0; i < size; i++) {
        float normalized = array[i] / (float)window_height;
        if (normalized < 0.0f) {
            normalized = 0.0f;
        }
        if (normalized > 1.0f) {
            normalized = 1.0f;
        }

        int height = (int)(normalized * chart_height);
        if (height < 2) {
            height = 2;
        }

        SDL_Rect bar = {
            .x = margin_x + i * bar_width,
            .y = chart_bottom - height,
            .w = bar_width - 1,
            .h = height
        };

        if (i == highlight1 || i == highlight2)
            SDL_SetRenderDrawColor(renderer, 255, 129, 67, 255);
        else
            SDL_SetRenderDrawColor(renderer, 85, 214, 156, 255);

        SDL_RenderFillRect(renderer, &bar);
    }
}

void draw_array_in_rect(SDL_Renderer* renderer, float* array, int size, SDL_Rect rect) {
    int chart_margin_x = 8;
    int chart_margin_y = 8;
    int chart_w = rect.w - (2 * chart_margin_x);
    int chart_h = rect.h - (2 * chart_margin_y);
    int bar_w = (size > 0) ? chart_w / size : chart_w;

    if (bar_w < 1) {
        bar_w = 1;
    }

    SDL_SetRenderDrawColor(renderer, 34, 54, 89, 255);
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 94, 132, 189, 180);
    SDL_RenderDrawRect(renderer, &rect);

    for (int i = 0; i < size; i++) {
        float normalized = array[i] / 600.0f;
        if (normalized < 0.0f) {
            normalized = 0.0f;
        }
        if (normalized > 1.0f) {
            normalized = 1.0f;
        }

        int h = (int)(normalized * chart_h);
        if (h < 1) {
            h = 1;
        }

        SDL_Rect bar = {
            .x = rect.x + chart_margin_x + i * bar_w,
            .y = rect.y + rect.h - chart_margin_y - h,
            .w = bar_w,
            .h = h
        };

        SDL_SetRenderDrawColor(renderer, 99, 220, 173, 255);
        SDL_RenderFillRect(renderer, &bar);
    }
}

void draw_compare_panel(SDL_Renderer* renderer, TTF_Font* font, const char* title, float* array, int size, Stats stats, SDL_Rect rect) {
    char info_line[160];
    SDL_Color title_color = {235, 245, 255, 255};
    SDL_Color info_color = {189, 219, 255, 255};
    SDL_Rect panel = rect;
    SDL_Rect chart_rect = {rect.x + 10, rect.y + 58, rect.w - 20, rect.h - 68};

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 7, 14, 24, 190);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 127, 186, 255, 200);
    SDL_RenderDrawRect(renderer, &panel);

    draw_label(renderer, font, title, title_color, rect.x + 12, rect.y + 10);

    snprintf(info_line, sizeof(info_line), "cmp:%d mem:%d t:%dms",
             stats.comparisons, stats.memory_accesses, stats.end_time - stats.start_time);
    draw_label(renderer, font, info_line, info_color, rect.x + 12, rect.y + 32);

    draw_array_in_rect(renderer, array, size, chart_rect);
}

void draw_stats(SDL_Renderer* renderer, TTF_Font* font, Stats stats) {
    char buffer[128];
    SDL_Color white = {232, 245, 255, 255};
    SDL_Rect panel = {10, 10, 780, 40};

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 3, 10, 22, 165);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 120, 178, 255, 150);
    SDL_RenderDrawRect(renderer, &panel);

    snprintf(buffer, sizeof(buffer), "Comparisons: %d | Memory accesses: %d | Time: %d ms",
             stats.comparisons, stats.memory_accesses, stats.end_time - stats.start_time);

    SDL_Surface* surface = TTF_RenderText_Blended(font, buffer, white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dst = {20, 20, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void draw_instructions(SDL_Renderer* renderer, TTF_Font* font) {
    const char* lines[] = {
        "Pick a mode with mouse, touch, or keyboard.",
        "Array modes: 1 2 3 4 5 6",
        "Student mode: 8",
        "Sort keys: B S I Q M | Student keys: A G N",
        "Playback: SPACE pause/resume, +/- speed, R reset",
    };

    SDL_Color bg_top = {8, 16, 31, 255};
    SDL_Color bg_bottom = {17, 39, 66, 255};
    SDL_Color title_color = {229, 245, 255, 255};
    SDL_Color line_color = {188, 219, 255, 255};
    SDL_Color sub_color = {138, 184, 240, 255};
    SDL_Rect panel = {20, 18, 760, 150};
    int y = 72;

    draw_vertical_gradient(renderer, 800, 600, bg_top, bg_bottom);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 6, 14, 26, 180);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 126, 188, 255, 190);
    SDL_RenderDrawRect(renderer, &panel);

    draw_label(renderer, font, "Sorting Visualizer", title_color, 34, 24);
    draw_label(renderer, font, "Interactive mode selection", sub_color, 36, 46);

    for (int i = 0; i < 5; i++) {
        draw_label(renderer, font, lines[i], line_color, 38, y);
        y += 18;
    }

    draw_label(renderer, font, "Choose one large card below to start", sub_color, 250, 545);
}
void draw_students(SDL_Renderer* renderer, TTF_Font* font, Student* array, int size) {
    SDL_Color bg_top = {14, 23, 42, 255};
    SDL_Color bg_bottom = {24, 51, 82, 255};
    SDL_Color white = {235, 246, 255, 255};
    SDL_Color accent = {166, 212, 255, 255};
    SDL_Rect panel = {18, 16, 764, 564};
    int y = 52;
    int visible_count = size;
    char buffer[128];

    if (visible_count > 20) {
        visible_count = 20;
    }

    draw_vertical_gradient(renderer, 800, 600, bg_top, bg_bottom);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 6, 12, 20, 165);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 126, 188, 255, 170);
    SDL_RenderDrawRect(renderer, &panel);

    draw_label(renderer, font, "Students (first rows)", white, 32, 24);
    draw_label(renderer, font, "Name | Age | Grade", accent, 32, 78);
    y = 108;

    for (int i = 0; i < visible_count; i++) {
        snprintf(buffer, sizeof(buffer), "%2d. %-10s  age: %2d  grade: %4.1f",
                 i + 1,
                 array[i].name, array[i].age, array[i].grade);
        draw_label(renderer, font, buffer, white, 32, y);
        y += 22;
    }

    if (size > visible_count) {
        snprintf(buffer, sizeof(buffer), "... %d more students hidden", size - visible_count);
        draw_label(renderer, font, buffer, accent, 32, y + 8);
    }
}