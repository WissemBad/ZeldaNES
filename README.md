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

## Audio

Toute la config audio est dans:

`assets/meta/audio/audio.cfg`

Tu peux y régler:
- les OST en boucle (`MUSIC_MENU`, `MUSIC_GAMEPLAY`, `MUSIC_GAMEOVER`)
- les SFX par événement (`SFX_ENEMY_KILLED`, `SFX_WALK`, `SFX_MENU_CLICK`, etc.)
- les volumes (`MASTER_VOLUME`, `MUSIC_VOLUME`, `SFX_VOLUME`)
- la fréquence du son de pas (`WALK_INTERVAL_FRAMES`)

Dépose tes fichiers audio dans:
- `assets/sounds/music/`
- `assets/sounds/sfx/`

## Crédits Assets

- Tiles map + sprites Link: <https://github.com/asweigart/nes_zelda_map_data>
- Sprites attaque Link: <https://www.spriters-resource.com/nes/legendofzelda/asset/8366/>
- Sprites ennemis: <https://www.spriters-resource.com/nes/legendofzelda/asset/31805/>
