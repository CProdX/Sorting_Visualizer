# Sorting Algorithms Visualization

SDL2 project for visualising sorting algorithms and comparing their behavior on different data sets.

## Project Organization

The project is organised around a simple SDL application and a small set of source files.

```text
.
├── src/
│   ├── main.c                 # SDL loop and program entry point
│   ├── sorting.c              # Sorting algorithms and step-by-step animation
│   ├── sorting.h              # Sorting function declarations
│   ├── utils.c                # Data generation and helper functions
│   ├── utils.h                # Shared types, stats, and declarations
│   ├── visual.c               # SDL rendering functions
│   └── visual.h               # Rendering declarations
├── CMakeLists.txt             # Build configuration
└── README.md                  # Project documentation
```

## Features

- SDL2 window-based visualisation.
- Animated bar rendering for arrays.
- Bubble Sort, Selection Sort, Insertion Sort, QuickSort, and MergeSort.
- Multiple input distributions: random, sorted, reversed, nearly sorted, duplicates, and pyramid.
- Student mode with sorting by age, grade, or name.
- Large on-screen buttons for mouse and touchscreen selection.
- Real-time statistics display in the window.

## Data Types

The project uses arrays of `float` for the main visualisation and a `Student` structure for the alternative data mode:

```c
typedef struct {
	char name[20];
	int age;
	float grade;
} Student;
```

Sorting is driven by comparison functions so the same approach can be reused for different data types.

## Keyboard Controls

### Choose the input mode

- `1` random array
- `2` sorted array
- `3` reversed array
- `4` nearly sorted array
- `5` duplicate-heavy array
- `6` pyramid array
- `8` student mode

### Sort arrays

- `b` Bubble Sort
- `s` Selection Sort
- `i` Insertion Sort
- `q` QuickSort
- `m` MergeSort

### Sort students

- `a` sort by age
- `g` sort by grade
- `n` sort by name

### Reset

- `r` restart after a completed sort

## Build and Run

### Dependencies

- SDL2
- SDL2_ttf
- CMake
- GCC

On Ubuntu, Debian, or WSL:

```bash
sudo apt update
sudo apt install libsdl2-dev libsdl2-ttf-dev cmake make gcc
```

### Build

```bash
cmake -S . -B build
cmake --build build
```

### Run

```bash
./build/sorting_visualizer
```

## Notes

- The project targets GCC on Ubuntu or WSL.
- Windows users should run the project in WSL for the closest environment match.