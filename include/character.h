/**
 * @file character.h
 * @brief Structure de base pour les personnages (joueur et ennemis)
 */

#ifndef NUPRC_CHARACTER_H
#define NUPRC_CHARACTER_H

#include "core.h"
#include "map.h"

typedef enum {
    CHAR_HERO,
    CHAR_ENEMY
} CharacterType;

typedef struct {
    CharacterType type;
    float         posX;          // Position X en coordonnées flottantes
    float         posY;          // Position Y en coordonnées flottantes
    int           lives;
    SDL_Texture*  texture;
    Map*          map;
} Character;

void Character_init(Character* c, CharacterType type, int lives, Map* map);
void Character_moveSmooth(Character* c, float deltaX, float deltaY);
void Character_move(Character* c, const int delta[2]);
void Character_draw(const Character* c, SDL_Renderer* renderer);
void Character_destroy(Character* c);
void Character_getGridPos(const Character* c, int gridPos[2]);

#endif
