# Sorting Visualizer

Petit projet C avec SDL2 pour visualiser des algorithmes de tri.

## Structure

```text
.
├── src/
│   ├── main.c
│   ├── sorting.c
│   ├── sorting.h
│   ├── utils.c
│   ├── utils.h
│   ├── visual.c
│   └── visual.h
├── CMakeLists.txt
└── README.md
```

## Fonctionnalites

- Visualisation de tris sur un tableau anime
- Tri a bulles, selection, insertion, rapide et fusion
- Generation de plusieurs profils de depart
- Mode etudiant avec tri par age, note ou nom

## Installation

Dependances : SDL2, SDL2_ttf, CMake et un compilateur C.

Sous Ubuntu, Debian ou WSL :

```bash
sudo apt update
sudo apt install libsdl2-dev libsdl2-ttf-dev cmake make gcc
```

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Lancement

```bash
./build/sorting_visualizer
```

## Commandes

- `1` a `6` : choisir le mode de generation
- `8` : mode etudiant
- `b`, `s`, `i`, `q`, `m` : lancer un tri sur tableau
- `a`, `g`, `n` : trier les etudiants
- `r` : recommencer apres un tri termine
