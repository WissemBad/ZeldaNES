/**
 * @file enemy.c
 * @brief Implémentation de la gestion des ennemis
 */

#include "enemy.h"
#include "render.h"

//==============================================================================
// CONSTANTES PRIVÉES
//==============================================================================

/** Frames entre déplacements (ennemi basique) */
#define ENEMY_BASIC_MOVE_INTERVAL   60

/** Frames entre déplacements (ennemi rapide) */
#define ENEMY_FAST_MOVE_INTERVAL    30

/** Frames entre déplacements (ennemi tank) */
#define ENEMY_TANK_MOVE_INTERVAL    90

//==============================================================================
// FONCTIONS PRIVÉES
//==============================================================================

/**
 * Retourne l'intervalle de déplacement selon le type d'ennemi
 */
static int getMoveInterval(const EnemyType type) {
    switch (type) {
        case ENEMY_TYPE_FAST: return ENEMY_FAST_MOVE_INTERVAL;
        case ENEMY_TYPE_TANK: return ENEMY_TANK_MOVE_INTERVAL;
        default:              return ENEMY_BASIC_MOVE_INTERVAL;
    }
}

/**
 * Retourne le nombre de vies selon le type d'ennemi
 */
static int getEnemyLives(const EnemyType type) {
    switch (type) {
        case ENEMY_TYPE_TANK: return 3;
        default:              return 1;
    }
}

//==============================================================================
// FONCTIONS PUBLIQUES
//==============================================================================

void Enemy_init(Enemy* enemy, EnemyType type, Map* map, const int initialPos[2]) {
    Character_init(&enemy->base, CHAR_ENEMY, getEnemyLives(type), map);

    enemy->base.pos[0] = initialPos[0];
    enemy->base.pos[1] = initialPos[1];
    enemy->enemyType = type;
    enemy->moveTimer = 0;
    enemy->targetPos[0] = initialPos[0];
    enemy->targetPos[1] = initialPos[1];

    enemy->base.texture = loadTexture(ASSET_TEXTURE_ENEMY, map->renderer);
}

void Enemy_update(Enemy* enemy) {
    enemy->moveTimer++;

    if (enemy->moveTimer < getMoveInterval(enemy->enemyType)) {
        return;
    }

    enemy->moveTimer = 0;

    // Déplacement aléatoire
    const int direction = rand() % 4;
    const int deltas[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    Character_move(&enemy->base, deltas[direction]);
}

void Enemy_destroy(Enemy* enemy) {
    Character_destroy(&enemy->base);
}
