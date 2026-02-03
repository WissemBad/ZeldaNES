/**
 * @file enemy.h
 * @brief Gestion des ennemis
 */

#ifndef NUPRC_ENEMY_H
#define NUPRC_ENEMY_H

#include "character.h"

//==============================================================================
// ÉNUMÉRATIONS
//==============================================================================

/** Type d'ennemi (affecte vitesse et résistance) */
typedef enum {
    ENEMY_TYPE_BASIC,   /**< Ennemi standard : 1 vie, vitesse normale */
    ENEMY_TYPE_FAST,    /**< Ennemi rapide : 1 vie, vitesse élevée */
    ENEMY_TYPE_TANK     /**< Ennemi tank : 3 vies, vitesse lente */
} EnemyType;

//==============================================================================
// STRUCTURES
//==============================================================================

/** Structure d'un ennemi (hérite de Character) */
typedef struct {
    Character base;         /**< Données de base du personnage */
    EnemyType enemyType;    /**< Type d'ennemi */
    int       moveTimer;    /**< Compteur de frames pour le déplacement */
    int       targetPos[2]; /**< Position cible (pour l'IA) */
} Enemy;

//==============================================================================
// FONCTIONS
//==============================================================================

/** Initialise un ennemi à une position donnée */
void Enemy_init(Enemy* enemy, EnemyType type, Map* map, const int initialPos[2]);

/** Met à jour l'état de l'ennemi (IA, déplacement aléatoire) */
void Enemy_update(Enemy* enemy);

/** Libère les ressources allouées pour un ennemi */
void Enemy_destroy(Enemy* enemy);

#endif // NUPRC_ENEMY_H
