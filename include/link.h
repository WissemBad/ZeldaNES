/**
 * @file link.h
 * @brief Gestion du personnage jouable (Link)
 */

#ifndef NUPRC_LINK_H
#define NUPRC_LINK_H

#include "character.h"

//==============================================================================
// ÉNUMÉRATIONS
//==============================================================================

/** Direction dans laquelle Link regarde */
typedef enum {
    LINK_DIR_UP,
    LINK_DIR_DOWN,
    LINK_DIR_LEFT,
    LINK_DIR_RIGHT
} LinkDirection;

//==============================================================================
// STRUCTURES
//==============================================================================

/** Structure du joueur (hérite de Character) */
typedef struct {
    Character     base;             /**< Données de base du personnage */
    LinkDirection direction;        /**< Direction actuelle */
    bool          isAttacking;      /**< true si en train d'attaquer */
    int           attackCooldown;   /**< Frames restantes avant prochaine attaque */
} Link;

//==============================================================================
// FONCTIONS
//==============================================================================

/** Initialise le personnage Link */
void Link_init(Link* link, Map* map);

/** Met à jour l'état de Link (cooldowns, animations) */
void Link_update(Link* link);

/** Déclenche une attaque de Link */
void Link_attack(Link* link);

/** Libère les ressources allouées pour Link */
void Link_destroy(Link* link);

#endif // NUPRC_LINK_H
