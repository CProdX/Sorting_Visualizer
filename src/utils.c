#include <stdlib.h>
#include <time.h>
#include "utils.h"

const char* names[] = {
    "Alice", "Bob", "Charlie", "Diana", "Eve",
    "Frank", "Grace", "Hugo", "Ivy", "Jack"
};

void generate_students(Student* array, int size) {
    for (int i = 0; i < size; i++) {
        strcpy(array[i].name, names[rand() % 10]);
        array[i].age = 18 + rand() % 13;           // 18–30
        array[i].grade = ((float)rand() / RAND_MAX) * 20.0f; // 0–20
    }
}

void generate_array(float* array, int size, int max_value, ArrayMode mode) {
    switch (mode) {
        case MODE_RANDOM:
            for (int i = 0; i < size; i++)
                array[i] = ((float)rand() / RAND_MAX) * max_value;
            break;

        case MODE_SORTED:
            for (int i = 0; i < size; i++)
                array[i] = ((float)i / size) * max_value;
            break;

        case MODE_REVERSED:
            for (int i = 0; i < size; i++)
                array[i] = ((float)(size - i - 1) / size) * max_value;
            break;

        case MODE_ALMOST_SORTED:
            for (int i = 0; i < size; i++)
                array[i] = ((float)i / size) * max_value;
            for (int i = 0; i < size / 10; i++) {
                int a = rand() % size;
                int b = rand() % size;
                float temp = array[a];
                array[a] = array[b];
                array[b] = temp;
            }
            break;

        case MODE_DUPLICATES:
            for (int i = 0; i < size; i++) {
                int bucket = rand() % 5;
                array[i] = ((float)bucket / 5) * max_value;
            }
            break;

        case MODE_PYRAMID:
            for (int i = 0; i < size / 2; i++)
                array[i] = ((float)i / (size / 2)) * max_value;
            for (int i = size / 2; i < size; i++)
                array[i] = ((float)(size - i) / (size / 2)) * max_value;
            break;
    }
}

void sort_students_by_age(Student* array, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (array[j].age > array[j + 1].age) {
                Student temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

void sort_students_by_grade(Student* array, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (array[j].grade > array[j + 1].grade) {
                Student temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

void sort_students_by_name(Student* array, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (strcmp(array[j].name, array[j + 1].name) > 0) {
                Student temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

void generic_sort(Student* array, int size, int (*compare)(Student a, Student b)) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (compare(array[j], array[j + 1]) > 0) {
                Student temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}


int compare_by_age(Student a, Student b) {
    return a.age - b.age;
}

int compare_by_grade(Student a, Student b) {
    return (a.grade > b.grade) - (a.grade < b.grade); // renvoie -1, 0 ou 1
}

int compare_by_name(Student a, Student b) {
    return strcmp(a.name, b.name);
}


