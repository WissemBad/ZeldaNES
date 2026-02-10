# NUPRC

Petit jeu 2D inspiré des Zelda NES, codé en C avec SDL2.

## Contrôles

- `↑ ↓ ← →` ou `Z Q S D`: déplacement
- `F`: attaque
- `P` ou `Échap`: pause
- `Entrée` / `Espace`: valider dans les menus

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

Exemple (macOS/Homebrew):

```bash
brew install sdl2 sdl2_ttf sdl2_image sdl2_mixer
```

## Crédits Assets

- Tiles map + sprites Link: <https://github.com/asweigart/nes_zelda_map_data>
- Sprites attaque Link: <https://www.spriters-resource.com/nes/legendofzelda/asset/8366/>
- Sprites ennemis: <https://www.spriters-resource.com/nes/legendofzelda/asset/31805/>
