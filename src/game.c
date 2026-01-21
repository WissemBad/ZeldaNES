#include "game.h"
#include "grid.h"
#include "character.h"
#include "IOManager.h"
#include <stdlib.h>
#include <time.h>

void Game_initGrid(char grid[GRID_COLS][GRID_ROWS], Character player, int endPosition[2]) {
    for (int i = 0; i < GRID_COLS; i++) {
        for (int j = 0; j < GRID_ROWS; j++) {
            grid[i][j] = CELL_CHAR_EMPTY;
        }
    }

    grid[player.position[0]][player.position[1]] = CELL_CHAR_PLAYER;
    grid[endPosition[0]][endPosition[1]] = CELL_CHAR_END;
}

void Game_init(Game *game, SDL_Renderer *renderer, char* playerName) {
    srand(time(NULL));

    game->player = Character_init(playerName, NULL, renderer);
    game->endPosition[0] = rand() % GRID_COLS;
    game->endPosition[1] = rand() % GRID_ROWS;

    Game_initGrid(game->grid, game->player, game->endPosition);

    game->isRunning = true;
    game->hasWon = false;
}

bool Game_isEnd(Character character, int endPosition[2], bool *hasWon) {
    if (character.moveCount >= GAME_MAX_MOVES) {
        *hasWon = false;
        return true;
    }

    if (character.position[0] == endPosition[0] && character.position[1] == endPosition[1]) {
        *hasWon = true;
        return true;
    }

    return false;
}

bool Game_isExceed(Character character, Direction direction) {
    if (direction == DIR_UP && character.position[1] == 0) return true;
    if (direction == DIR_DOWN && character.position[1] == GRID_ROWS - 1) return true;
    if (direction == DIR_LEFT && character.position[0] == 0) return true;
    if (direction == DIR_RIGHT && character.position[0] == GRID_COLS - 1) return true;
    return false;
}

void Game_update(Game *game, Direction direction) {
    if (direction == -1 || Game_isExceed(game->player, direction)) {
        return;
    }

    game->player = Character_move(game->player, direction);
    Grid_update(game->grid, game->player, direction);

    Game_isEnd(game->player, game->endPosition, &game->hasWon);
    if (game->hasWon || game->player.moveCount >= GAME_MAX_MOVES) {
        game->isRunning = false;
    }
}

void Game_draw(Game *game, SDL_Renderer *renderer) {
    Grid_draw(renderer, game->endPosition);
    Character_draw(game->player, renderer);
}

void Game_end(Game *game, SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Color white = {255, 255, 255, 255};

    if (game->hasWon) {
        IO_printText(50, WINDOW_HEIGHT/2 - 90, "VICTOIRE", 400, 50, font, white, renderer);
        IO_printText(80, WINDOW_HEIGHT/2, "Vous avez atteint l'arrivee.", 340, 30, font, white, renderer);
        char movesText[50];
        sprintf(movesText, "Nombre de deplacements : %d", game->player.moveCount);
        IO_printText(100, WINDOW_HEIGHT/2 + 40, movesText, 300, 25, font, white, renderer);
    } else {
        IO_printText(80, WINDOW_HEIGHT/2 - 90, "DEFAITE", 340, 50, font, white, renderer);
        IO_printText(60, WINDOW_HEIGHT/2, "Vous avez depasse le nombre", 380, 30, font, white, renderer);
        IO_printText(80, WINDOW_HEIGHT/2 + 35, "maximum de deplacements.", 340, 30, font, white, renderer);
    }

    IO_printText(40, WINDOW_HEIGHT/2 + 120, "Appuyez sur une touche pour quitter.", 420, 25, font, white, renderer);
    IO_updateDisplay(renderer);

    bool wait = true;
    SDL_Event event;
    while (wait) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN || event.type == SDL_QUIT) {
                wait = false;
            }
        }
    }
}

void Game_destroy(Game *game) {
    if (game != NULL) {
        Character_destroy(&game->player);
        Grid_destroy();
    }
}
