# Sorting Visualizer

Un visualiseur de tris en C avec SDL2. Le projet affiche les etapes des principaux algorithmes de tri sous forme de barres animees, avec un mode dedie aux etudiants pour comparer des donnees selon plusieurs criteres.

## Apercu

- Visualisation en temps reel des tris classiques.
- Generateur de tableaux avec plusieurs profils de depart.
- Mode etudiant pour trier une liste de profils par age, note ou nom.
- Statistiques affichees a l ecran pendant l execution.

## Fonctionnalites

### Tri des tableaux

- Tri a bulles
- Tri par selection
- Tri par insertion
- Tri rapide
- Tri fusion

### Generation des donnees

- Aleatoire
- Triee
- Inversee
- Presque triee
- Avec doublons
- En pyramide

### Mode etudiant

Le mode etudiant genere une liste de profils fictifs et permet de la trier selon :

- l age
- la note
- le nom

## Commandes clavier

### Choix du mode de generation

- `1` : tableau aleatoire
- `2` : tableau trie
- `3` : tableau inverse
- `4` : tableau presque trie
- `5` : tableau avec doublons
- `6` : tableau en pyramide
- `8` : mode etudiant

### Tri des tableaux

- `b` : tri a bulles
- `s` : tri par selection
- `i` : tri par insertion
- `q` : tri rapide
- `m` : tri fusion

### Tri du mode etudiant

- `a` : tri par age
- `g` : tri par note
- `n` : tri par nom

### Remise a zero

- `r` : recommencer apres un tri termine

## Installation

### Dependances

Le projet utilise :

- SDL2
- SDL2_ttf

Sous Ubuntu / Debian / WSL :

```bash
sudo apt update
sudo apt install libsdl2-dev libsdl2-ttf-dev cmake make gcc
```

## Compilation

```bash
cmake -S . -B build
cmake --build build
```

## Execution

```bash
./build/sorting_visualizer
```

## Organisation du projet

- `src/main.c` : boucle principale, evenements et affichage
- `src/sorting.c` : algorithmes de tri et animations
- `src/visual.c` : rendu des barres, statistiques et instructions
- `src/utils.c` : generation des donnees et fonctions utilitaires

## Details techniques

- La fenetre est ouverte en 800x600.
- Le tableau contient 100 elements.
- La police est chargee depuis un chemin DejaVu Sans Linux / WSL en dur dans le code.

Si tu veux, je peux aussi ajouter une version plus moderne du README avec une section captures d ecran, un badge de build et un schema du flux de l application.
