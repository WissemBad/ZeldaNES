#include "character.h"
#include "IOManager.h"
#include <stdio.h>

Character Character_init(char* name, char* texturePath, SDL_Renderer *renderer) {
    Character character;
    character.name = name;
    character.position[0] = 4;
    character.position[1] = 4;
    character.moveCount = 0;

    if (texturePath != NULL) {
        character.texture = IO_loadTexture(texturePath, renderer);
    } else {
        character.texture = IO_loadTexture(ASSET_TEXTURE_PLAYER_DEFAULT, renderer);
    }

    return character;
}

Character Character_move(Character character, Direction direction) {
    switch (direction) {
        case DIR_RIGHT:
            character.position[0] += 1;
            character.moveCount++;
            break;
        case DIR_LEFT:
            character.position[0] -= 1;
            character.moveCount++;
            break;
        case DIR_UP:
            character.position[1] -= 1;
            character.moveCount++;
            break;
        case DIR_DOWN:
            character.position[1] += 1;
            character.moveCount++;
            break;
        default:
            break;
    }
    return character;
}

void Character_draw(Character character, SDL_Renderer *renderer) {
    int pixelX = character.position[0] * GRID_CELL_SIZE;
    int pixelY = character.position[1] * GRID_CELL_SIZE;
    IO_renderTexture(character.texture, renderer, pixelX, pixelY, GRID_CELL_SIZE, GRID_CELL_SIZE);
}

void Character_print(Character character) {
    printf("-- Informations du personnage --\n");
    printf("Nom : %s\n", character.name);
    printf("Position (x, y) : %d, %d\n", character.position[0], character.position[1]);
    printf("Deplacements : %d\n", character.moveCount);
    printf("--------------------------------\n");
}

void Character_destroy(Character *character) {
    if (character != NULL && character->texture != NULL) {
        SDL_DestroyTexture(character->texture);
        character->texture = NULL;
    }
}

