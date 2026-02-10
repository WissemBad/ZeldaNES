/**
 * @file character.c
 * @brief Implémentation des personnages
 */

#include "character.h"
#include "render.h"
#include "utils.h"

void Character_init(Character* c, CharacterType type, int lives, Map* map) {
    if (!c) return;
    c->type = type;
    c->lives = lives;
    c->posX = 0.0f;
    c->posY = 0.0f;
    c->texture = NULL;
    c->map = map;
}

void Character_getGridPos(const Character* c, int gridPos[2]) {
    if (!c || !gridPos) return;
    gridPos[0] = (int)(c->posX + 0.5f);  // Arrondi à l'entier le plus proche
    gridPos[1] = (int)(c->posY + 0.5f);
}

void Character_moveSmooth(Character* c, float deltaX, float deltaY) {
    if (!c || !c->map) return;

    // Calcule la nouvelle position
    float newX = c->posX + deltaX;
    float newY = c->posY + deltaY;

    // Vérifie les collisions sur la position arrondie
    int gridPos[2] = {(int)(newX + 0.5f), (int)(newY + 0.5f)};

    if (!Map_isBlocking(c->map, gridPos)) {
        c->posX = newX;
        c->posY = newY;
    }
}

void Character_move(Character* c, const int delta[2]) {
    if (!c || !c->map) return;

    int gridPos[2];
    Character_getGridPos(c, gridPos);
    int newPos[2] = { gridPos[0] + delta[0], gridPos[1] + delta[1] };

    if (!Map_isBlocking(c->map, newPos)) {
        c->posX = (float)newPos[0];
        c->posY = (float)newPos[1];
    }
}

void Character_draw(const Character* c, SDL_Renderer* renderer) {
    if (!c || !c->texture || !renderer) return;

    int gridPos[2];
    Character_getGridPos(c, gridPos);

    int screenPos[2];
    worldToScreen(gridPos, screenPos, c->map->currentRoom);
    renderTexture(c->texture, renderer, screenPos[0], screenPos[1],
                  GRID_CELL_SIZE, GRID_CELL_SIZE);
}

void Character_destroy(Character* c) {
    if (!c) return;
    if (c->texture) {
        SDL_DestroyTexture(c->texture);
        c->texture = NULL;
    }
}
