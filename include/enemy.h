/**
 * @file enemy.h
 * @brief Gestion des ennemis
 */

#ifndef NUPRC_ENEMY_H
#define NUPRC_ENEMY_H

#include "character.h"
#include "animation.h"

//==============================================================================
// CONSTANTES
//==============================================================================

/** Durée d'invincibilité de Link après avoir été touché (en frames) */
#define ENEMY_HIT_COOLDOWN      90

/** Points gagnés pour avoir tué un ennemi */
#define ENEMY_KILL_SCORE        100

//==============================================================================
// ÉNUMÉRATIONS
//==============================================================================

/** Type d'ennemi (affecte vitesse et résistance) */
typedef enum {
    ENEMY_TYPE_BASIC,   /**< Ennemi standard : 1 vie, vitesse normale */
    ENEMY_TYPE_FAST,    /**< Ennemi rapide : 1 vie, vitesse élevée */
    ENEMY_TYPE_TANK     /**< Ennemi tank : 3 vies, vitesse lente */
} EnemyType;

/** Stratégie de déplacement de l'ennemi */
typedef enum {
    ENEMY_AI_RANDOM,    /**< Déplacement aléatoire */
    ENEMY_AI_CHASE      /**< Poursuite du joueur (chemin le plus court) */
} EnemyAI;

//==============================================================================
// STRUCTURES
//==============================================================================

/** Structure d'un ennemi (hérite de Character) */
typedef struct {
    Character       base;           /**< Données de base du personnage */
    EnemyType       enemyType;      /**< Type d'ennemi */
    EnemyAI         ai;             /**< Stratégie de déplacement */
    int             moveTimer;      /**< Compteur de frames pour le déplacement */
    int             targetPos[2];   /**< Position cible (pour l'IA) */
    bool            isActive;       /**< true si l'ennemi est actif */
    int             hitTimer;       /**< Timer de clignotement après un hit */
    SpriteSet       sprites;        /**< Ensemble des sprites animés */
    AnimationState  animation;      /**< État de l'animation */
} Enemy;

//==============================================================================
// FONCTIONS
//==============================================================================

/** Initialise un ennemi à une position donnée */
void Enemy_init(Enemy* enemy, EnemyType type, EnemyAI ai, Map* map, const int initialPos[2]);

/** Met à jour l'état de l'ennemi avec gestion des collisions entre ennemis */
void Enemy_update(Enemy* enemy, const int playerPos[2], const Enemy* allEnemies, int enemyCount, int selfIndex);

/** Vérifie si l'ennemi est en collision avec une position */
bool Enemy_collidesWith(const Enemy* enemy, const int pos[2]);

/** Vérifie si une position est occupée par un autre ennemi */
bool Enemy_isPositionOccupied(const int pos[2], const Enemy* allEnemies, int enemyCount, int excludeIndex);

/** Inflige des dégâts à l'ennemi, retourne true si l'ennemi est mort */
bool Enemy_takeDamage(Enemy* enemy, int damage);

/** Dessine l'ennemi avec l'animation appropriée */
void Enemy_draw(const Enemy* enemy, SDL_Renderer* renderer);

/** Libère les ressources allouées pour un ennemi */
void Enemy_destroy(Enemy* enemy);

#endif // NUPRC_ENEMY_H
