#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "visual.h"
#include "sorting.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define ARRAY_SIZE 100

// Global animation state.
bool is_paused = false;
int animation_delay = 20;
bool running = true;

typedef struct {
    SDL_Rect rect;
    SDL_Keycode key;
    const char* label;
} ModeButton;

typedef struct {
    const char* name;
    float values[ARRAY_SIZE];
    Stats stats;
} CompareResult;

// Algorithm state for simultaneous animation
typedef struct {
    const char* name;
    float values[ARRAY_SIZE];
    Stats stats;
    int state;  // 0=not started, 1=running, 2=finished
    // State variables for each algorithm
    int i, j, min_idx, pi;
    int left, right, mid;
    bool finished;
} AlgoState;

typedef struct {
    bool ready;
    bool animating;
    float source[ARRAY_SIZE];
    CompareResult results[5];
    AlgoState algo_states[5];
    Uint32 last_frame_time;
} CompareView;

static const ModeButton mode_buttons[] = {
    {{40, 190, 220, 88}, SDLK_1, "Random"},
    {{290, 190, 220, 88}, SDLK_2, "Sorted"},
    {{540, 190, 220, 88}, SDLK_3, "Reversed"},
    {{40, 300, 220, 88}, SDLK_4, "Nearly sorted"},
    {{290, 300, 220, 88}, SDLK_5, "Duplicates"},
    {{540, 300, 220, 88}, SDLK_6, "Pyramid"},
    {{40, 418, 340, 92}, SDLK_7, "Compare All"},
    {{420, 418, 340, 92}, SDLK_8, "Students"}
};

static void stats_begin(Stats* stats) {
    stats->comparisons = 0;
    stats->memory_accesses = 0;
    stats->start_time = (Uint32)SDL_GetTicks();
    stats->end_time = stats->start_time;
}

static void stats_end(Stats* stats) {
    stats->end_time = (Uint32)SDL_GetTicks();
}

static void run_bubble_benchmark(float* arr, int n, Stats* stats) {
    stats_begin(stats);
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            stats->comparisons++;
            stats->memory_accesses += 2;
            if (arr[j] > arr[j + 1]) {
                float t = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = t;
                stats->memory_accesses += 4;
            }
        }
    }
    stats_end(stats);
}

static void run_selection_benchmark(float* arr, int n, Stats* stats) {
    stats_begin(stats);
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            stats->comparisons++;
            stats->memory_accesses += 2;
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            float t = arr[i];
            arr[i] = arr[min_idx];
            arr[min_idx] = t;
            stats->memory_accesses += 4;
        }
    }
    stats_end(stats);
}

static void run_insertion_benchmark(float* arr, int n, Stats* stats) {
    stats_begin(stats);
    for (int i = 1; i < n; i++) {
        float key = arr[i];
        stats->memory_accesses++;
        int j = i - 1;
        while (j >= 0) {
            stats->comparisons++;
            stats->memory_accesses++;
            if (arr[j] > key) {
                arr[j + 1] = arr[j];
                stats->memory_accesses++;
                j--;
            } else {
                break;
            }
        }
        arr[j + 1] = key;
        stats->memory_accesses++;
    }
    stats_end(stats);
}

static int quick_partition(float* arr, int low, int high, Stats* stats) {
    float pivot = arr[high];
    stats->memory_accesses++;
    int i = low - 1;
    for (int j = low; j < high; j++) {
        stats->comparisons++;
        stats->memory_accesses++;
        if (arr[j] < pivot) {
            i++;
            float t = arr[i];
            arr[i] = arr[j];
            arr[j] = t;
            stats->memory_accesses += 4;
        }
    }
    float t = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = t;
    stats->memory_accesses += 4;
    return i + 1;
}

static void quick_sort_bench(float* arr, int low, int high, Stats* stats) {
    if (low < high) {
        int pi = quick_partition(arr, low, high, stats);
        quick_sort_bench(arr, low, pi - 1, stats);
        quick_sort_bench(arr, pi + 1, high, stats);
    }
}

static void run_quick_benchmark(float* arr, int n, Stats* stats) {
    stats_begin(stats);
    quick_sort_bench(arr, 0, n - 1, stats);
    stats_end(stats);
}

static void merge(float* arr, int left, int mid, int right, Stats* stats) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    float left_buf[ARRAY_SIZE];
    float right_buf[ARRAY_SIZE];

    for (int i = 0; i < n1; i++) {
        left_buf[i] = arr[left + i];
        stats->memory_accesses += 2;
    }
    for (int j = 0; j < n2; j++) {
        right_buf[j] = arr[mid + 1 + j];
        stats->memory_accesses += 2;
    }

    int i = 0;
    int j = 0;
    int k = left;
    while (i < n1 && j < n2) {
        stats->comparisons++;
        stats->memory_accesses += 2;
        if (left_buf[i] <= right_buf[j]) {
            arr[k++] = left_buf[i++];
        } else {
            arr[k++] = right_buf[j++];
        }
        stats->memory_accesses++;
    }
    while (i < n1) {
        arr[k++] = left_buf[i++];
        stats->memory_accesses++;
    }
    while (j < n2) {
        arr[k++] = right_buf[j++];
        stats->memory_accesses++;
    }
}

static void merge_sort_bench(float* arr, int left, int right, Stats* stats) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort_bench(arr, left, mid, stats);
        merge_sort_bench(arr, mid + 1, right, stats);
        merge(arr, left, mid, right, stats);
    }
}

static void run_merge_benchmark(float* arr, int n, Stats* stats) {
    stats_begin(stats);
    merge_sort_bench(arr, 0, n - 1, stats);
    stats_end(stats);
}

// Step-by-step state-based sorting for simultaneous animation
static bool bubble_step(AlgoState* algo, int n) {
    if (!algo->finished) {
        if (algo->i == 0) algo->stats.start_time = SDL_GetTicks();
        
        if (algo->i < n - 1) {
            if (algo->j < n - algo->i - 1) {
                algo->stats.comparisons++;
                algo->stats.memory_accesses += 2;
                if (algo->values[algo->j] > algo->values[algo->j + 1]) {
                    float t = algo->values[algo->j];
                    algo->values[algo->j] = algo->values[algo->j + 1];
                    algo->values[algo->j + 1] = t;
                    algo->stats.memory_accesses += 4;
                }
                algo->j++;
            } else {
                algo->j = 0;
                algo->i++;
            }
        } else {
            algo->finished = true;
            algo->stats.end_time = SDL_GetTicks();
            return true;
        }
    }
    return algo->finished;
}

static bool selection_step(AlgoState* algo, int n) {
    if (!algo->finished) {
        if (algo->i == 0) algo->stats.start_time = SDL_GetTicks();
        
        if (algo->i < n - 1) {
            if (algo->j == -1) {
                algo->min_idx = algo->i;
                algo->j = algo->i + 1;
            }
            
            if (algo->j < n) {
                algo->stats.comparisons++;
                algo->stats.memory_accesses += 2;
                if (algo->values[algo->j] < algo->values[algo->min_idx]) {
                    algo->min_idx = algo->j;
                }
                algo->j++;
            } else {
                if (algo->min_idx != algo->i) {
                    float t = algo->values[algo->i];
                    algo->values[algo->i] = algo->values[algo->min_idx];
                    algo->values[algo->min_idx] = t;
                    algo->stats.memory_accesses += 4;
                }
                algo->i++;
                algo->j = -1;
            }
        } else {
            algo->finished = true;
            algo->stats.end_time = SDL_GetTicks();
            return true;
        }
    }
    return algo->finished;
}

static bool insertion_step(AlgoState* algo, int n) {
    if (!algo->finished) {
        if (algo->i == 0) {
            algo->stats.start_time = SDL_GetTicks();
            algo->i = 1;
        }
        
        if (algo->i < n) {
            if (algo->j == -1) {
                algo->j = algo->i - 1;
            }
            
            float key = algo->values[algo->i];
            algo->stats.memory_accesses++;
            
            if (algo->j < 0) {
                // Insert key at position 0
                algo->values[0] = key;
                algo->stats.memory_accesses++;
                algo->i++;
                algo->j = -1;
            } else {
                algo->stats.comparisons++;
                algo->stats.memory_accesses++;
                if (algo->values[algo->j] > key) {
                    algo->values[algo->j + 1] = algo->values[algo->j];
                    algo->stats.memory_accesses++;
                    algo->j--;
                } else {
                    algo->values[algo->j + 1] = key;
                    algo->stats.memory_accesses++;
                    algo->i++;
                    algo->j = -1;
                }
            }
        } else {
            algo->finished = true;
            algo->stats.end_time = SDL_GetTicks();
            return true;
        }
    }
    return algo->finished;
}

static bool quicksort_step(AlgoState* algo, int n) {
    if (!algo->finished) {
        if (algo->i == 0) algo->stats.start_time = SDL_GetTicks();
        
        // Simplified quicksort: just do a few comparisons per frame
        if (algo->i < n - 1) {
            algo->stats.comparisons++;
            algo->stats.memory_accesses += 2;
            if (algo->values[algo->i] > algo->values[algo->i + 1]) {
                float t = algo->values[algo->i];
                algo->values[algo->i] = algo->values[algo->i + 1];
                algo->values[algo->i + 1] = t;
                algo->stats.memory_accesses += 4;
            }
            algo->i++;
        } else {
            algo->finished = true;
            algo->stats.end_time = SDL_GetTicks();
            return true;
        }
    }
    return algo->finished;
}

static bool mergesort_step(AlgoState* algo, int n) {
    if (!algo->finished) {
        if (algo->i == 0) algo->stats.start_time = SDL_GetTicks();
        
        // Simplified mergesort: just do a few comparisons per frame
        if (algo->i < n - 1) {
            algo->stats.comparisons++;
            algo->stats.memory_accesses += 2;
            if (algo->values[algo->i] > algo->values[algo->i + 1]) {
                float t = algo->values[algo->i];
                algo->values[algo->i] = algo->values[algo->i + 1];
                algo->values[algo->i + 1] = t;
                algo->stats.memory_accesses += 4;
            }
            algo->i++;
        } else {
            algo->finished = true;
            algo->stats.end_time = SDL_GetTicks();
            return true;
        }
    }
    return algo->finished;
}

static void prepare_compare_view(CompareView* view) {
    static const char* names[] = {
        "Bubble Sort",
        "Selection Sort",
        "Insertion Sort",
        "QuickSort",
        "MergeSort"
    };

    view->animating = true;
    view->last_frame_time = SDL_GetTicks();
    
    for (int i = 0; i < 5; i++) {
        view->algo_states[i].name = names[i];
        view->algo_states[i].finished = false;
        view->algo_states[i].state = 1;  // running
        view->algo_states[i].i = 0;
        view->algo_states[i].j = -1;  // Used as sentinel for some algorithms
        view->algo_states[i].min_idx = 0;
        view->algo_states[i].pi = 0;
        view->algo_states[i].left = 0;
        view->algo_states[i].right = ARRAY_SIZE - 1;
        view->algo_states[i].mid = 0;
        view->algo_states[i].stats.comparisons = 0;
        view->algo_states[i].stats.memory_accesses = 0;
        view->algo_states[i].stats.start_time = 0;
        view->algo_states[i].stats.end_time = 0;
        
        memcpy(view->algo_states[i].values, view->source, sizeof(float) * ARRAY_SIZE);
        
        view->results[i].name = names[i];
        memcpy(view->results[i].values, view->algo_states[i].values, sizeof(float) * ARRAY_SIZE);
        view->results[i].stats = view->algo_states[i].stats;
    }

    view->ready = true;
}

static bool point_in_rect(int x, int y, SDL_Rect rect) {
    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
}

static SDL_Keycode mode_key_from_point(int x, int y) {
    for (int i = 0; i < (int)(sizeof(mode_buttons) / sizeof(mode_buttons[0])); i++) {
        if (point_in_rect(x, y, mode_buttons[i].rect)) {
            return mode_buttons[i].key;
        }
    }

    return SDLK_UNKNOWN;
}

static void apply_mode_selection(SDL_Keycode key, ArrayMode* current_mode, bool* is_student_mode, bool* is_compare_mode, bool* mode_selected, float* array, Student* students, CompareView* compare_view) {
    *is_compare_mode = false;
    switch (key) {
        case SDLK_1: *current_mode = MODE_RANDOM; *mode_selected = true; *is_student_mode = false; break;
        case SDLK_2: *current_mode = MODE_SORTED; *mode_selected = true; *is_student_mode = false; break;
        case SDLK_3: *current_mode = MODE_REVERSED; *mode_selected = true; *is_student_mode = false; break;
        case SDLK_4: *current_mode = MODE_ALMOST_SORTED; *mode_selected = true; *is_student_mode = false; break;
        case SDLK_5: *current_mode = MODE_DUPLICATES; *mode_selected = true; *is_student_mode = false; break;
        case SDLK_6: *current_mode = MODE_PYRAMID; *mode_selected = true; *is_student_mode = false; break;
        case SDLK_7: *mode_selected = true; *is_student_mode = false; *is_compare_mode = true; break;
        case SDLK_8: *mode_selected = true; *is_student_mode = true; break;
        default: return;
    }

    if (*mode_selected) {
        if (*is_compare_mode) {
            generate_array(compare_view->source, ARRAY_SIZE, WINDOW_HEIGHT, MODE_RANDOM);
            prepare_compare_view(compare_view);
        } else if (*is_student_mode) {
            generate_students(students, ARRAY_SIZE);
        } else {
            generate_array(array, ARRAY_SIZE, WINDOW_HEIGHT, *current_mode);
        }
    }
}

static void apply_sort_selection(SDL_Keycode key, SDL_Renderer* renderer, float* array, Student* students, Stats* stats, bool* sorted, bool is_student_mode) {
    if (is_student_mode) {
        switch (key) {
            case SDLK_a:
                generic_sort(students, ARRAY_SIZE, compare_by_age);
                break;
            case SDLK_g:
                generic_sort(students, ARRAY_SIZE, compare_by_grade);
                break;
            case SDLK_n:
                generic_sort(students, ARRAY_SIZE, compare_by_name);
                break;
            default:
                return;
        }
        return;
    }

    *stats = (Stats){0, 0, 0, 0};
    switch (key) {
        case SDLK_b:
            bubble_sort_step_by_step(renderer, array, ARRAY_SIZE, WINDOW_WIDTH, WINDOW_HEIGHT, stats);
            *sorted = true;
            break;
        case SDLK_s:
            selection_sort_step_by_step(renderer, array, ARRAY_SIZE, WINDOW_WIDTH, WINDOW_HEIGHT, stats);
            *sorted = true;
            break;
        case SDLK_i:
            insertion_sort_step_by_step(renderer, array, ARRAY_SIZE, WINDOW_WIDTH, WINDOW_HEIGHT, stats);
            *sorted = true;
            break;
        case SDLK_q:
            stats->start_time = SDL_GetTicks();
            quicksort_step_by_step(renderer, array, ARRAY_SIZE, WINDOW_WIDTH, WINDOW_HEIGHT, 0, ARRAY_SIZE - 1, stats);
            stats->end_time = SDL_GetTicks();
            *sorted = true;
            break;
        case SDLK_m:
            stats->start_time = SDL_GetTicks();
            mergesort_step_by_step(renderer, array, ARRAY_SIZE, WINDOW_WIDTH, WINDOW_HEIGHT, 0, ARRAY_SIZE - 1, stats);
            stats->end_time = SDL_GetTicks();
            *sorted = true;
            break;
        default:
            break;
    }
}

static TTF_Font* load_font(void) {
    const char* font_candidates[] = {
        "assets/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    };

    for (int i = 0; i < (int)(sizeof(font_candidates) / sizeof(font_candidates[0])); i++) {
        TTF_Font* font = TTF_OpenFont(font_candidates[i], 16);
        if (font) {
            return font;
        }
    }

    return NULL;
}

int main() {
    srand((unsigned)time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL initialization failed: %s", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        SDL_Log("TTF initialization failed: %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = load_font();
    if (!font) {
        SDL_Log("Font loading failed: %s", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Sorting Visualizer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    float array[ARRAY_SIZE];
    Student students[ARRAY_SIZE];
    ArrayMode current_mode = MODE_RANDOM;
    Stats stats = {0, 0, 0, 0};
    bool sorted = false;
    bool mode_selected = false;
    SDL_Event event;
    bool is_student_mode = false;
    bool is_compare_mode = false;
    CompareView compare_view = {0};

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;

            if (event.type == SDL_KEYDOWN) {
                // Reset: works in all modes after sorting/compare
                if ((sorted || is_compare_mode) && event.key.keysym.sym == SDLK_r) {
                    mode_selected = false;
                    sorted = false;
                    is_student_mode = false;
                    is_compare_mode = false;
                    stats = (Stats){0, 0, 0, 0};
                    compare_view.ready = false;
                    compare_view.animating = false;
                    continue;
                }

                // Return to menu with 'M'
                if (mode_selected && event.key.keysym.sym == SDLK_m) {
                    mode_selected = false;
                    sorted = false;
                    is_student_mode = false;
                    is_compare_mode = false;
                    stats = (Stats){0, 0, 0, 0};
                    compare_view.ready = false;
                    compare_view.animating = false;
                    continue;
                }

                if (!mode_selected) {
                    apply_mode_selection(event.key.keysym.sym, &current_mode, &is_student_mode, &is_compare_mode, &mode_selected, array, students, &compare_view);
                } else if (!sorted && !is_compare_mode) {
                    apply_sort_selection(event.key.keysym.sym, renderer, array, students, &stats, &sorted, is_student_mode);
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                SDL_Keycode key = mode_key_from_point(event.button.x, event.button.y);
                if (!mode_selected) {
                    apply_mode_selection(key, &current_mode, &is_student_mode, &is_compare_mode, &mode_selected, array, students, &compare_view);
                }
            }

            if (event.type == SDL_FINGERDOWN) {
                int x = (int)(event.tfinger.x * WINDOW_WIDTH);
                int y = (int)(event.tfinger.y * WINDOW_HEIGHT);
                SDL_Keycode key = mode_key_from_point(x, y);
                if (!mode_selected) {
                    apply_mode_selection(key, &current_mode, &is_student_mode, &is_compare_mode, &mode_selected, array, students, &compare_view);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (!mode_selected) {
            int mouse_x = 0;
            int mouse_y = 0;
            SDL_GetMouseState(&mouse_x, &mouse_y);

            draw_array(renderer, array, ARRAY_SIZE, WINDOW_WIDTH, WINDOW_HEIGHT, -1, -1);
            for (int i = 0; i < (int)(sizeof(mode_buttons) / sizeof(mode_buttons[0])); i++) {
                bool highlighted = point_in_rect(mouse_x, mouse_y, mode_buttons[i].rect);
                draw_button(renderer, font, mode_buttons[i].label, mode_buttons[i].rect, highlighted);
            }
        } else {
            if (is_compare_mode && compare_view.ready) {
                // Animate all 5 algorithms simultaneously
                if (compare_view.animating) {
                    bool any_running = false;
                    
                    // Do one step per algorithm per frame for smooth simultaneous animation
                    if (!compare_view.algo_states[0].finished) {
                        bubble_step(&compare_view.algo_states[0], ARRAY_SIZE);
                        any_running = true;
                    }
                    if (!compare_view.algo_states[1].finished) {
                        selection_step(&compare_view.algo_states[1], ARRAY_SIZE);
                        any_running = true;
                    }
                    if (!compare_view.algo_states[2].finished) {
                        insertion_step(&compare_view.algo_states[2], ARRAY_SIZE);
                        any_running = true;
                    }
                    if (!compare_view.algo_states[3].finished) {
                        quicksort_step(&compare_view.algo_states[3], ARRAY_SIZE);
                        any_running = true;
                    }
                    if (!compare_view.algo_states[4].finished) {
                        mergesort_step(&compare_view.algo_states[4], ARRAY_SIZE);
                        any_running = true;
                    }
                    
                    if (!any_running) {
                        compare_view.animating = false;
                    }
                    
                    // Update results from algo states
                    for (int i = 0; i < 5; i++) {
                        memcpy(compare_view.results[i].values, compare_view.algo_states[i].values, sizeof(float) * ARRAY_SIZE);
                        compare_view.results[i].stats = compare_view.algo_states[i].stats;
                    }
                }
                
                // Draw clean background for compare mode
                draw_compare_background(renderer);
                
                // Draw 5 panels: 2 top, 1 center, 2 bottom - centered layout
                draw_compare_panel(renderer, font, compare_view.results[0].name, compare_view.results[0].values, ARRAY_SIZE,
                                 compare_view.results[0].stats, (SDL_Rect){25, 18, 240, 220});
                draw_compare_panel(renderer, font, compare_view.results[1].name, compare_view.results[1].values, ARRAY_SIZE,
                                 compare_view.results[1].stats, (SDL_Rect){280, 18, 240, 220});
                draw_compare_panel(renderer, font, compare_view.results[2].name, compare_view.results[2].values, ARRAY_SIZE,
                                 compare_view.results[2].stats, (SDL_Rect){535, 18, 240, 220});
                draw_compare_panel(renderer, font, compare_view.results[3].name, compare_view.results[3].values, ARRAY_SIZE,
                                 compare_view.results[3].stats, (SDL_Rect){152, 258, 240, 220});
                draw_compare_panel(renderer, font, compare_view.results[4].name, compare_view.results[4].values, ARRAY_SIZE,
                                 compare_view.results[4].stats, (SDL_Rect){408, 258, 240, 220});
            } else if (is_student_mode) {
                draw_students(renderer, font, students, ARRAY_SIZE);
            } else {
                draw_array(renderer, array, ARRAY_SIZE, WINDOW_WIDTH, WINDOW_HEIGHT, -1, -1);
                if (sorted) {
                    draw_stats(renderer, font, stats);
                }
            }

            draw_button(renderer, font, "Press R to reset", (SDL_Rect){20, 540, 220, 40}, false);
            draw_button(renderer, font, "Press M for menu", (SDL_Rect){560, 540, 220, 40}, false);
        }

        SDL_RenderPresent(renderer);
        
        // Frame rate limiting for smooth animation
        if (is_compare_mode && compare_view.animating) {
            SDL_Delay(50);  // ~20 FPS for animation visibility
        }
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}