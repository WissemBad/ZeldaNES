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
    c->pos[0] = 0;
    c->pos[1] = 0;
    c->texture = NULL;
    c->map = map;
}

void Character_move(Character* c, const int delta[2]) {
    if (!c || !c->map) return;

    int newPos[2] = { c->pos[0] + delta[0], c->pos[1] + delta[1] };

    if (!Map_isBlocking(c->map, newPos)) {
        c->pos[0] = newPos[0];
        c->pos[1] = newPos[1];
    }
}

void Character_draw(const Character* c, SDL_Renderer* renderer) {
    if (!c || !c->texture || !renderer) return;

    int screenPos[2];
    worldToScreen(c->pos, screenPos, c->map->currentRoom);
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
