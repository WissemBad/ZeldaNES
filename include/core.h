#ifndef NUPRC_CORE_H
#define NUPRC_CORE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define GRID_CELL_SIZE      50
#define GRID_WORLD_WIDTH    256
#define GRID_WORLD_HEIGHT   88
#define GRID_ROOM_WIDTH     16
#define GRID_ROOM_HEIGHT    11

#define MAP_TILE_SIZE       16
#define MAP_TILES_WIDTH     20
#define MAP_TILES_HEIGHT    8
#define MAP_TILES_COUNT     (MAP_TILES_WIDTH * MAP_TILES_HEIGHT)

#define WINDOW_TITLE            "NUPRC - Zelda-like"
#define WINDOW_FONT_PATH        "fonts/Inter-Bold.ttf"
#define WINDOW_FONT_SIZE        16
#define WINDOW_TEXTAREA_HEIGHT  100
#define WINDOW_WIDTH            (GRID_ROOM_WIDTH * GRID_CELL_SIZE)
#define WINDOW_HEIGHT           (GRID_ROOM_HEIGHT * GRID_CELL_SIZE + WINDOW_TEXTAREA_HEIGHT)

#define GAME_MAX_ENEMIES    10
#define GAME_INITIAL_LIVES  5
#define GAME_INITIAL_SCORE  0
#define GAME_INITIAL_ROOM   {7, 7}
#define GAME_WIN_KILLS      25

#define MOVEMENT_SPEED      0.15f

#define ASSET_TEXTURE_LINK      "textures/characters/link2.bmp"
#define ASSET_TEXTURE_ENEMY     "textures/characters/enemy7.bmp"
#define ASSET_MAP_TILES         "textures/map/overworldtiles.bmp"
#define ASSET_MAP_WORLD         "meta/map/overworld_tile_map.txt"
#define ASSET_MAP_BLOCKING      "meta/map/overworld_blocking_map.txt"
#define ASSET_AUDIO_CONFIG      "meta/audio/audio.cfg"

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER,
    STATE_WIN
} GameState;

typedef struct {
    SDL_Window*   window;
    SDL_Renderer* renderer;
    TTF_Font*     font;
} RenderState;

typedef struct {
    int score;
    int kills;
    int playtime;
    int moves;
} PlayerStats;

typedef struct {
    float x;
    float y;
    float targetX;
    float targetY;
} Camera;

#endif
