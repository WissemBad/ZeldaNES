# ME_NUPRC - Jeu de Déplacement SDL2

## 📋 Description

Jeu simple développé en C avec SDL2 où le joueur doit atteindre une case d'arrivée en un nombre limité de déplacements.

## 🎮 Gameplay

- **Objectif** : Atteindre la case d'arrivée (marquée par une texture spéciale)
- **Contrôles** : Touches fléchées (↑ ↓ ← →)
- **Limite** : Maximum 20 déplacements
- **Grille** : 10x10 cases de 50x50 pixels

## 🏗️ Architecture

### Structure du Projet

```
ME_NUPRC/
├── include/              # Fichiers headers (.h)
│   ├── character.h       # Gestion du personnage
│   ├── game.h           # Logique principale du jeu
│   ├── grid.h           # Gestion de la grille
│   ├── IOManager.h      # Entrées/Sorties SDL
│   └── settings.h       # Configuration et constantes
├── src/                 # Fichiers source (.c)
│   ├── character.c
│   ├── game.c
│   ├── grid.c
│   ├── IOManager.c
│   └── main.c
├── assets/              # Ressources graphiques
│   ├── fonts/
│   │   └── DejaVuSans-Bold.ttf
│   └── images/
│       ├── perso.bmp
│       └── arrivee.bmp
└── cmake-build-debug/   # Fichiers de build
```

### Modules

#### Character (character.c/h)
Gestion du personnage joueur :
- `Character_init()` : Création et initialisation
- `Character_move()` : Déplacement selon une direction
- `Character_draw()` : Affichage graphique
- `Character_destroy()` : Libération des ressources

#### Game (game.c/h)
Logique principale du jeu :
- `Game_init()` : Initialisation de la partie
- `Game_update()` : Mise à jour de l'état du jeu
- `Game_draw()` : Rendu graphique
- `Game_end()` : Écran de fin
- `Game_destroy()` : Nettoyage des ressources

#### Grid (grid.c/h)
Gestion de la grille de jeu :
- `Grid_update()` : Mise à jour après déplacement
- `Grid_draw()` : Affichage de la grille et de l'arrivée
- `Grid_destroy()` : Libération des textures

#### IOManager (IOManager.c/h)
Gestion des entrées/sorties SDL :
- Initialisation SDL et TTF
- Création fenêtre/renderer
- Chargement de textures
- Gestion du clavier
- Affichage de texte
- Nettoyage SDL

## 🛠️ Compilation

### Prérequis
- **C Compiler** (gcc/clang)
- **CMake** (≥ 3.10)
- **SDL2** (bibliothèque graphique)
- **SDL2_ttf** (gestion des polices)

### Installation des dépendances (macOS)
```bash
brew install sdl2 sdl2_ttf cmake
```

### Compilation
```bash
cd cmake-build-debug
cmake ..
cmake --build . --target ME_NUPRC
```

### Exécution
```bash
./cmake-build-debug/ME_NUPRC
```

## 📝 Conventions de Code

### Nommage des Fonctions
Format : `Entity_method()`
- `Character_init()`, `Character_move()`
- `Game_update()`, `Game_draw()`
- `Grid_update()`, `Grid_draw()`
- `IO_createWindow()`, `IO_quit()`

### Nommage des Variables
Format : `camelCase`
- `endPosition`, `isRunning`, `hasWon`
- `renderer`, `window`, `character`

### Documentation
- **Headers (.h)** : Docstrings complètes (`/** @brief ... */`)
- **Sources (.c)** : Pas de commentaires `//` (code auto-documenté)

## 🎯 Règles du Jeu

1. Le joueur démarre au centre de la grille (position 4,4)
2. L'arrivée est placée aléatoirement sur la grille
3. Le joueur dispose de **20 déplacements maximum**
4. Les déplacements hors limites sont interdits

### Conditions de Victoire
- ✅ Atteindre la case d'arrivée

### Conditions de Défaite
- ❌ Dépasser 20 déplacements

## 🔧 Gestion des Ressources

Le jeu gère proprement toutes les ressources :
1. **Textures** : Libérées via `Character_destroy()` et `Grid_destroy()`
2. **Polices** : Fermées avec `TTF_CloseFont()`
3. **Bibliothèque TTF** : Fermée avec `TTF_Quit()`
4. **Renderer/Window** : Détruits via `IO_quit()`
5. **SDL** : Fermée avec `SDL_Quit()`

## 📊 Statistiques

- **Lignes de code** : ~418 lignes (sans compter les headers)
- **Fichiers source** : 5 fichiers .c
- **Fichiers header** : 5 fichiers .h
- **Warnings** : 0 erreurs critiques

## 👨‍💻 Auteur

Développé par Wissem

## 📄 Licence

Projet éducatif - ME_NUPRC

---

**Status** : ✅ Production Ready
