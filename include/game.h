#ifndef NUPRC_GAME_H
#define NUPRC_GAME_H

#include "core.h"
#include "map.h"
#include "link.h"
#include "enemy.h"
#include "menu.h"

typedef struct {
    RenderState render;
    GameState   state;
    GameState   previousState;
    PlayerStats stats;
    Map         map;
    Link        player;
    Enemy       enemies[GAME_MAX_ENEMIES];
    int         enemyCount;
    bool        running;
    Menu        menu;
} Game;

void Game_init(Game* game);
void Game_run(Game* game);
void Game_destroy(Game* game);

void Game_setState(Game* game, GameState newState);
void Game_startNewGame(Game* game);
void Game_pause(Game* game);
void Game_resume(Game* game);

void Game_handleInput(Game* game);
void Game_update(Game* game);
void Game_render(Game* game);

#endif
