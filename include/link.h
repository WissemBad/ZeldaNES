/**
 * @file link.h
 * @brief Gestion du personnage jouable (Link)
 */

#ifndef NUPRC_LINK_H
#define NUPRC_LINK_H

#include "character.h"

//==============================================================================
// CONSTANTES
//==============================================================================

/** Durée du cooldown d'attaque (en frames) */
#define LINK_ATTACK_COOLDOWN    20

/** Durée d'invincibilité après avoir été touché (en frames) */
#define LINK_INVINCIBILITY_TIME 90

/** Portée de l'attaque (en cases) */
#define LINK_ATTACK_RANGE       1

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
    int           invincibilityTimer; /**< Frames d'invincibilité restantes */
    bool          isInvincible;     /**< true si Link est invincible */
} Link;

//==============================================================================
// FONCTIONS
//==============================================================================

/** Initialise le personnage Link */
void Link_init(Link* link, Map* map);

/** Met à jour l'état de Link (cooldowns, animations, invincibilité) */
void Link_update(Link* link);

/** Déclenche une attaque de Link */
void Link_attack(Link* link);

/** Link subit des dégâts (avec gestion de l'invincibilité) */
void Link_takeDamage(Link* link, int damage);

/** Calcule la position d'attaque selon la direction */
void Link_getAttackPosition(const Link* link, int attackPos[2]);

/** Vérifie si Link est actuellement en train d'attaquer */
bool Link_isAttacking(const Link* link);

/** Libère les ressources allouées pour Link */
void Link_destroy(Link* link);

#endif // NUPRC_LINK_H
