# ZeldaNES

Petit jeu 2D en C et SDL2, inspiré des Zelda sur NES. Projet personnel sans lien avec Nintendo ; l'exécutable et le projet CMake s'appellent `NUPRC`.

## Contrôles

- `↑ ↓ ← →` ou `Z Q S D` : déplacement
- `F` : attaque
- `P` ou `Échap` : pause
- `Entrée` ou `Espace` : valider dans les menus

## Build

```bash
cmake -S . -B build
cmake --build build
./build/NUPRC
```

## Dépendances

- `SDL2`
- `SDL2_ttf`
- `SDL2_image`
- `SDL2_mixer`

Exemple sous macOS avec Homebrew :

```bash
brew install sdl2 sdl2_ttf sdl2_image sdl2_mixer
```

## Audio

La configuration audio se trouve dans `assets/meta/audio/audio.cfg`. Elle règle :
- les OST en boucle (`MUSIC_MENU`, `MUSIC_GAMEPLAY`, `MUSIC_GAMEOVER`)
- les SFX par événement (`SFX_ENEMY_KILLED`, `SFX_WALK`, `SFX_MENU_CLICK`, etc.)
- les volumes (`MASTER_VOLUME`, `MUSIC_VOLUME`, `SFX_VOLUME`)
- la fréquence du son de pas (`WALK_INTERVAL_FRAMES`)

Les fichiers audio se placent dans :
- `assets/sounds/music/`
- `assets/sounds/sfx/`

## Crédits Assets

- Tiles map + sprites Link: <https://github.com/asweigart/nes_zelda_map_data>
- Sprites attaque Link: <https://www.spriters-resource.com/nes/legendofzelda/asset/8366/>
- Sprites ennemis: <https://www.spriters-resource.com/nes/legendofzelda/asset/31805/>
