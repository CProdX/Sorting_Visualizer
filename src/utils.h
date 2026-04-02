#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>
#include <stdbool.h>
// Déclarations des variables globales pour le contrôle de l'animation
    extern bool is_paused;
extern int animation_delay;
extern bool running;
typedef struct {
    int comparisons;
    int memory_accesses;
    Uint32 start_time;
    Uint32 end_time;
} Stats;
void generate_random_array(int* array, int size, int max_value);
typedef enum {
    MODE_RANDOM,
    MODE_SORTED,
    MODE_REVERSED,
    MODE_ALMOST_SORTED,
    MODE_DUPLICATES,
    MODE_PYRAMID
} ArrayMode;

typedef struct {
    char name[20];
    int age;
    float grade;
} Student;
void generate_students(Student* array, int size);
void generate_array(float* array, int size, int max_value, ArrayMode mode);
void sort_students_by_age(Student* array, int size);
void sort_students_by_grade(Student* array, int size);
void sort_students_by_name(Student* array, int size);
void generic_sort(Student* array, int size, int (*compare)(Student a, Student b));

int compare_by_age(Student a, Student b);
int compare_by_grade(Student a, Student b);
int compare_by_name(Student a, Student b);


#endif
