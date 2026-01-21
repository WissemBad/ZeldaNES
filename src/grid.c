#include "grid.h"
#include "IOManager.h"

static SDL_Texture *g_endTexture = NULL;
static SDL_Texture* Grid_getEndTexture(SDL_Renderer *renderer) {
    if (g_endTexture == NULL && renderer != NULL) {
        g_endTexture = IO_loadTexture(ASSET_TEXTURE_END_DEFAULT, renderer);
    }
    return g_endTexture;
}

void Grid_update(char grid[GRID_COLS][GRID_ROWS], Character character, Direction direction) {
    int oldX = character.position[0];
    int oldY = character.position[1];

    switch (direction) {
        case DIR_UP:
            oldY++;
            break;
        case DIR_DOWN:
            oldY--;
            break;
        case DIR_LEFT:
            oldX++;
            break;
        case DIR_RIGHT:
            oldX--;
            break;
        default:
            return;
    }

    if (oldX >= 0 && oldX < GRID_COLS && oldY >= 0 && oldY < GRID_ROWS) {
        if (grid[oldX][oldY] != CELL_CHAR_END) {
            grid[oldX][oldY] = CELL_CHAR_EMPTY;
        }
    }

    grid[character.position[0]][character.position[1]] = CELL_CHAR_PLAYER;
}

static void Grid_renderEnd(SDL_Renderer *renderer, int endPosition[2]) {
    SDL_Texture* texture = Grid_getEndTexture(renderer);
    if (texture != NULL) {
        IO_renderTexture(texture, renderer, endPosition[0] * GRID_CELL_SIZE,
                        endPosition[1] * GRID_CELL_SIZE, GRID_CELL_SIZE, GRID_CELL_SIZE);
    }
}

void Grid_draw(SDL_Renderer *renderer, int endPosition[2]) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int i = 1; i < GRID_ROWS; i++)
    { SDL_RenderDrawLine(renderer, 0, i * GRID_CELL_SIZE, WINDOW_WIDTH - 1, i * GRID_CELL_SIZE); }

    for (int i = 1; i < GRID_COLS; i++)
    { SDL_RenderDrawLine(renderer, i * GRID_CELL_SIZE, 0, i * GRID_CELL_SIZE, WINDOW_HEIGHT - 1); }

    SDL_Rect border = {0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1};
    SDL_RenderDrawRect(renderer, &border);
    Grid_renderEnd(renderer, endPosition);
}

void Grid_destroy(void) {
    if (g_endTexture != NULL) {
        SDL_DestroyTexture(g_endTexture);
        g_endTexture = NULL;
    }
}