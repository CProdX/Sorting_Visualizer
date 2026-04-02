cmake_minimum_required(VERSION 3.10)
project(sorting_visualizer C)

set(CMAKE_C_STANDARD 11)

# Inclure les headers SDL2
include_directories(/usr/include/SDL2)

# Ajouter les fichiers source
add_executable(sorting_visualizer
        src/main.c
        src/utils.c
        src/visual.c
        src/sorting.c
)

# Lier les bibliothèques SDL2 et SDL2_ttf
target_link_libraries(sorting_visualizer
        SDL2
        SDL2_ttf
)
