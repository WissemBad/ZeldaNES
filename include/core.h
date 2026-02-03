/**
 * @file core.h
 * @brief Fichier central contenant les constantes, types et structures de base du jeu
 */

#ifndef NUPRC_CORE_H
#define NUPRC_CORE_H

//==============================================================================
// INCLUDES SYSTÈME ET BIBLIOTHÈQUES
//==============================================================================

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//==============================================================================
// CONSTANTES - GRILLE ET MONDE
//==============================================================================

#define GRID_CELL_SIZE      50
#define GRID_WORLD_WIDTH    256
#define GRID_WORLD_HEIGHT   88
#define GRID_ROOM_WIDTH     16
#define GRID_ROOM_HEIGHT    11

#define MAP_TILE_SIZE       16
#define MAP_TILES_WIDTH     20
#define MAP_TILES_HEIGHT    8
#define MAP_TILES_COUNT     (MAP_TILES_WIDTH * MAP_TILES_HEIGHT)

//==============================================================================
// CONSTANTES - FENÊTRE
//==============================================================================

#define WINDOW_TITLE            "NuPRC Game"
#define WINDOW_FONT_PATH        "assets/fonts/DejaVuSans-Bold.ttf"
#define WINDOW_FONT_SIZE        16
#define WINDOW_TEXTAREA_HEIGHT  100
#define WINDOW_WIDTH            (GRID_ROOM_WIDTH * GRID_CELL_SIZE)
#define WINDOW_HEIGHT           (GRID_ROOM_HEIGHT * GRID_CELL_SIZE + WINDOW_TEXTAREA_HEIGHT)

//==============================================================================
// CONSTANTES - GAMEPLAY
//==============================================================================

#define GAME_MAX_ENEMIES    10
#define GAME_INITIAL_LIVES  3
#define GAME_INITIAL_SCORE  0
#define GAME_INITIAL_ROOM   {7, 7}

//==============================================================================
// CONSTANTES - CHEMINS DES ASSETS
//==============================================================================

#define ASSET_TEXTURE_LINK      "assets/textures/characters/link2.bmp"
#define ASSET_TEXTURE_ENEMY     "assets/textures/characters/enemy7.bmp"
#define ASSET_MAP_TILES         "assets/textures/map/overworldtiles.bmp"
#define ASSET_MAP_WORLD         "assets/meta/map/overworld_tile_map.txt"
#define ASSET_MAP_BLOCKING      "assets/meta/map/overworld_blocking_map.txt"

//==============================================================================
// ÉNUMÉRATIONS
//==============================================================================

/** États possibles du jeu */
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER
} GameState;

//==============================================================================
// STRUCTURES COMMUNES
//==============================================================================

/** État du système de rendu SDL */
typedef struct {
    SDL_Window*   window;
    SDL_Renderer* renderer;
    TTF_Font*     font;
} RenderState;

/** Statistiques du joueur affichées dans le HUD */
typedef struct {
    int score;
    int kills;
    int playtime;
    int moves;          /**< Nombre de déplacements effectués */
} PlayerStats;

#endif // NUPRC_CORE_H
