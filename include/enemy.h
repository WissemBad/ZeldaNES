/**
 * @file enemy.h
 * @brief Gestion des ennemis et de leur IA
 */

#ifndef NUPRC_ENEMY_H
#define NUPRC_ENEMY_H

#include "character.h"
#include "animation.h"

#define ENEMY_HIT_COOLDOWN  90
#define ENEMY_KILL_SCORE    100

typedef enum {
    ENEMY_TYPE_BASIC,
    ENEMY_TYPE_FAST,
    ENEMY_TYPE_TANK
} EnemyType;

typedef enum {
    ENEMY_AI_RANDOM,
    ENEMY_AI_CHASE
} EnemyAI;

typedef struct {
    Character       base;
    EnemyType       enemyType;
    EnemyAI         ai;
    int             moveTimer;
    int             targetPos[2];
    bool            isActive;
    int             hitTimer;
    SpriteSet       sprites;
    AnimationState  animation;
} Enemy;

void Enemy_init(Enemy* e, EnemyType type, EnemyAI ai, Map* map, const int pos[2]);
void Enemy_update(Enemy* e, const int playerPos[2], const Enemy* all, int count, int self);
bool Enemy_collidesWith(const Enemy* e, const int pos[2]);
bool Enemy_isPositionOccupied(const int pos[2], const Enemy* all, int count, int exclude);
bool Enemy_takeDamage(Enemy* e, int damage);
void Enemy_draw(const Enemy* e, SDL_Renderer* renderer);
void Enemy_destroy(Enemy* e);

#endif
