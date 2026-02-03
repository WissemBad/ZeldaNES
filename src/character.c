/**
 * @file character.c
 * @brief Implémentation des fonctions de base des personnages
 */

#include "character.h"
#include "render.h"
#include "utils.h"

//==============================================================================
// FONCTIONS PUBLIQUES
//==============================================================================

void Character_init(Character* character, CharacterType type, int lives, Map* map) {
    character->type = type;
    character->lives = lives;
    character->pos[0] = 0;
    character->pos[1] = 0;
    character->texture = NULL;
    character->map = map;
}

void Character_move(Character* character, const int delta[2]) {
    const int newPos[2] = {
        character->pos[0] + delta[0],
        character->pos[1] + delta[1]
    };

    if (Map_isBlocking(character->map, newPos)) {
        return;
    }

    character->pos[0] = newPos[0];
    character->pos[1] = newPos[1];
}

void Character_draw(const Character* character, SDL_Renderer* renderer) {
    if (character->texture == NULL) {
        return;
    }

    int screenPos[2];
    worldToScreen(character->pos, screenPos, character->map->currentRoom);
    renderTexture(character->texture, renderer,
                  screenPos[0], screenPos[1],
                  GRID_CELL_SIZE, GRID_CELL_SIZE);
}

void Character_destroy(Character* character) {
    if (character->texture != NULL) {
        SDL_DestroyTexture(character->texture);
        character->texture = NULL;
    }
}
