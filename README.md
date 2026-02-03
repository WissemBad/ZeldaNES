# NUPRC - Jeu Zelda-like en C

## Description

Jeu d'action/aventure en 2D inspiré de Zelda, développé en C avec SDL2.
Le joueur contrôle Link et doit éliminer les ennemis dans chaque salle.

## Contrôles

| Touche | Action |
|--------|--------|
| ↑ ↓ ← → | Déplacement |
| F | Attaque |
| P / Échap | Pause |
| Entrée | Valider (menus) |

## Compilation

```bash
mkdir build && cd build
cmake ..
make
./NUPRC
```

### Dépendances

- SDL2
- SDL2_ttf
- SDL2_image
- SDL2_mixer

Sur macOS avec Homebrew :
```bash
brew install sdl2 sdl2_ttf sdl2_image sdl2_mixer
```

## Structure du projet

```
NUPRC/
├── include/          # Headers
│   ├── core.h        # Constantes et types
│   ├── game.h        # Boucle principale
│   ├── map.h         # Gestion de la carte
│   ├── link.h        # Personnage jouable
│   ├── enemy.h       # Ennemis et IA
│   ├── animation.h   # Système d'animation
│   ├── menu.h        # Système de menus
│   ├── hud.h         # Interface utilisateur
│   ├── render.h      # Fonctions SDL
│   ├── iomanager.h   # Gestion des entrées
│   └── utils.h       # Utilitaires
├── src/              # Sources
├── assets/           # Ressources
│   ├── fonts/
│   ├── textures/
│   └── meta/
└── CMakeLists.txt
```

## Fonctionnalités

- Système de menus (principal, pause, game over)
- 3 types d'ennemis (basique, rapide, tank)
- 2 modes d'IA (aléatoire, poursuite)
- Animations de sprites (marche, attaque)
- Collision entre personnages
- HUD avec statistiques
