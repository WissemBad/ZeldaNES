/**
 * @file enemy.c
 * @brief Implémentation de la gestion des ennemis
 */

#include "enemy.h"
#include "render.h"

#include <stdlib.h>

//==============================================================================
// CONSTANTES PRIVÉES
//==============================================================================

/** Frames entre déplacements (ennemi basique) */
#define ENEMY_BASIC_MOVE_INTERVAL   45

/** Frames entre déplacements (ennemi rapide) */
#define ENEMY_FAST_MOVE_INTERVAL    25

/** Frames entre déplacements (ennemi tank) */
#define ENEMY_TANK_MOVE_INTERVAL    70

/** Durée avant de changer de direction (mode aléatoire) */
#define ENEMY_RANDOM_CHANGE_TIME    120

//==============================================================================
// FONCTIONS PRIVÉES
//==============================================================================

/**
 * @brief Retourne l'intervalle de déplacement selon le type d'ennemi
 */
static int getMoveInterval(EnemyType type) {
    switch (type) {
        case ENEMY_TYPE_FAST: return ENEMY_FAST_MOVE_INTERVAL;
        case ENEMY_TYPE_TANK: return ENEMY_TANK_MOVE_INTERVAL;
        default:              return ENEMY_BASIC_MOVE_INTERVAL;
    }
}

/**
 * @brief Retourne le nombre de vies selon le type d'ennemi
 */
static int getEnemyLives(EnemyType type) {
    switch (type) {
        case ENEMY_TYPE_TANK: return 3;
        case ENEMY_TYPE_FAST: return 1;
        default:              return 2;
    }
}

/**
 * @brief Calcule le meilleur déplacement vers la cible (poursuite)
 */
static void calculateChaseMove(const Enemy* enemy, const int targetPos[2], int delta[2]) {
    int dx = targetPos[0] - enemy->base.pos[0];
    int dy = targetPos[1] - enemy->base.pos[1];

    delta[0] = 0;
    delta[1] = 0;

    // Priorité au mouvement le plus grand (Manhattan)
    if (abs(dx) > abs(dy)) {
        delta[0] = (dx > 0) ? 1 : -1;
    } else if (dy != 0) {
        delta[1] = (dy > 0) ? 1 : -1;
    } else if (dx != 0) {
        delta[0] = (dx > 0) ? 1 : -1;
    }
}

/**
 * @brief Calcule un déplacement aléatoire
 */
static void calculateRandomMove(int delta[2]) {
    const int deltas[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    int direction = rand() % 4;
    delta[0] = deltas[direction][0];
    delta[1] = deltas[direction][1];
}

//==============================================================================
// FONCTIONS PUBLIQUES
//==============================================================================

void Enemy_init(Enemy* enemy, EnemyType type, EnemyAI ai, Map* map, const int initialPos[2]) {
    Character_init(&enemy->base, CHAR_ENEMY, getEnemyLives(type), map);

    enemy->base.pos[0] = initialPos[0];
    enemy->base.pos[1] = initialPos[1];
    enemy->enemyType = type;
    enemy->ai = ai;
    enemy->moveTimer = 0;
    enemy->targetPos[0] = initialPos[0];
    enemy->targetPos[1] = initialPos[1];
    enemy->isActive = true;

    enemy->base.texture = loadTexture(ASSET_TEXTURE_ENEMY, map->renderer);
}

void Enemy_update(Enemy* enemy, const int playerPos[2]) {
    if (!enemy->isActive) return;

    enemy->moveTimer++;

    if (enemy->moveTimer < getMoveInterval(enemy->enemyType)) {
        return;
    }

    enemy->moveTimer = 0;

    int delta[2] = {0, 0};

    switch (enemy->ai) {
        case ENEMY_AI_CHASE:
            calculateChaseMove(enemy, playerPos, delta);
            break;

        case ENEMY_AI_RANDOM:
        default:
            calculateRandomMove(delta);
            break;
    }

    // Tenter de se déplacer
    Character_move(&enemy->base, delta);
}

bool Enemy_collidesWith(const Enemy* enemy, const int pos[2]) {
    if (!enemy->isActive) return false;

    return (enemy->base.pos[0] == pos[0] && enemy->base.pos[1] == pos[1]);
}

bool Enemy_takeDamage(Enemy* enemy, int damage) {
    if (!enemy->isActive) return false;

    enemy->base.lives -= damage;

    if (enemy->base.lives <= 0) {
        enemy->isActive = false;
        return true;  // Ennemi mort
    }

    return false;
}

void Enemy_destroy(Enemy* enemy) {
    Character_destroy(&enemy->base);
    enemy->isActive = false;
}
