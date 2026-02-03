/**
 * @file character.h
 * @brief Structure et fonctions de base pour tous les personnages
 */

#ifndef NUPRC_CHARACTER_H
#define NUPRC_CHARACTER_H

#include "core.h"
#include "map.h"

//==============================================================================
// ÉNUMÉRATIONS
//==============================================================================

/** Type de personnage */
typedef enum {
    CHAR_HERO,
    CHAR_ENEMY
} CharacterType;

//==============================================================================
// STRUCTURES
//==============================================================================

/** Structure de base pour tous les personnages (joueur et ennemis) */
typedef struct {
    CharacterType type;         /**< Type du personnage */
    int           pos[2];       /**< Position dans le monde [x, y] */
    int           lives;        /**< Points de vie restants */
    SDL_Texture*  texture;      /**< Sprite du personnage */
    Map*          map;          /**< Référence vers la carte (pour collisions) */
} Character;

//==============================================================================
// FONCTIONS
//==============================================================================

/** Initialise un personnage */
void Character_init(Character* character, CharacterType type, int lives, Map* map);

/** Déplace un personnage selon un vecteur de déplacement (vérifie les collisions) */
void Character_move(Character* character, const int delta[2]);

/** Affiche le personnage à l'écran (à la position correspondant à la salle actuelle) */
void Character_draw(const Character* character, SDL_Renderer* renderer);

/** Libère les ressources allouées pour un personnage */
void Character_destroy(Character* character);

#endif // NUPRC_CHARACTER_H
