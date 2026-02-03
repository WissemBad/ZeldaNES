/**
 * @file enemy.c
 * @brief Gestion des ennemis
 */

#include "enemy.h"
#include "render.h"
#include "utils.h"
#include <stdlib.h>

#define ENEMY_BASIC_MOVE_INTERVAL   70
#define ENEMY_FAST_MOVE_INTERVAL    45
#define ENEMY_TANK_MOVE_INTERVAL    100

static int getMoveInterval(EnemyType type) {
    switch (type) {
        case ENEMY_TYPE_FAST: return ENEMY_FAST_MOVE_INTERVAL;
        case ENEMY_TYPE_TANK: return ENEMY_TANK_MOVE_INTERVAL;
        default:              return ENEMY_BASIC_MOVE_INTERVAL;
    }
}

static int getEnemyLives(EnemyType type) {
    switch (type) {
        case ENEMY_TYPE_TANK: return 2;
        default:              return 1;
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

/**
 * @brief Convertit un delta de mouvement en AnimDirection
 */
static AnimDirection deltaToAnimDir(const int delta[2]) {
    if (delta[0] > 0) return ANIM_DIR_RIGHT;
    if (delta[0] < 0) return ANIM_DIR_LEFT;
    if (delta[1] > 0) return ANIM_DIR_DOWN;
    if (delta[1] < 0) return ANIM_DIR_UP;
    return ANIM_DIR_DOWN;
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
    enemy->hitTimer = 0;

    // Initialiser les animations
    Animation_init(&enemy->animation);
    SpriteSet_loadEnemy(&enemy->sprites, map->renderer);

    // La texture de base n'est plus utilisée directement
    enemy->base.texture = NULL;
}

void Enemy_update(Enemy* enemy, const int playerPos[2], const Enemy* allEnemies, int enemyCount, int selfIndex) {
    if (!enemy->isActive) return;

    // Mise à jour de l'animation
    Animation_update(&enemy->animation);

    // Mise à jour du timer de hit (clignotement)
    if (enemy->hitTimer > 0) {
        enemy->hitTimer--;
    }

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

    // Calculer la nouvelle position
    int newPos[2] = {
        enemy->base.pos[0] + delta[0],
        enemy->base.pos[1] + delta[1]
    };

    // Vérifier si la position est occupée par un autre ennemi
    if (Enemy_isPositionOccupied(newPos, allEnemies, enemyCount, selfIndex)) {
        // Ne pas bouger, mais mettre à jour la direction de l'animation
        AnimDirection animDir = deltaToAnimDir(delta);
        Animation_setDirection(&enemy->animation, animDir);
        return;
    }

    // Sauvegarder l'ancienne position
    int oldPos[2] = {enemy->base.pos[0], enemy->base.pos[1]};

    // Tenter de se déplacer (vérifie aussi les murs)
    Character_move(&enemy->base, delta);

    // Mettre à jour l'animation si on a bougé
    if (oldPos[0] != enemy->base.pos[0] || oldPos[1] != enemy->base.pos[1]) {
        AnimDirection animDir = deltaToAnimDir(delta);
        Animation_startWalk(&enemy->animation, animDir);
    }
}

bool Enemy_isPositionOccupied(const int pos[2], const Enemy* allEnemies, int enemyCount, int excludeIndex) {
    for (int i = 0; i < enemyCount; i++) {
        if (i == excludeIndex) continue;
        if (!allEnemies[i].isActive) continue;

        if (allEnemies[i].base.pos[0] == pos[0] && allEnemies[i].base.pos[1] == pos[1]) {
            return true;
        }
    }
    return false;
}

bool Enemy_collidesWith(const Enemy* enemy, const int pos[2]) {
    if (!enemy->isActive) return false;
    return (enemy->base.pos[0] == pos[0] && enemy->base.pos[1] == pos[1]);
}

bool Enemy_takeDamage(Enemy* enemy, int damage) {
    if (!enemy->isActive) return false;

    enemy->base.lives -= damage;
    enemy->hitTimer = 20;  // Effet de clignotement pendant 20 frames

    if (enemy->base.lives <= 0) {
        enemy->isActive = false;
        return true;
    }

    return false;
}

void Enemy_draw(const Enemy* enemy, SDL_Renderer* renderer) {
    if (!enemy->isActive) return;

    // Effet de clignotement quand touché
    if (enemy->hitTimer > 0 && (enemy->hitTimer / 3) % 2 == 0) {
        return;  // Ne pas dessiner (clignotement)
    }

    // Récupérer la texture animée
    SDL_Texture* texture = Animation_getCurrentTexture(&enemy->animation, &enemy->sprites);
    if (!texture) return;

    // Convertir en coordonnées écran
    int screenPos[2];
    worldToScreen(enemy->base.pos, screenPos, enemy->base.map->currentRoom);

    // Dessiner avec teinte rouge si récemment touché
    if (enemy->hitTimer > 0) {
        SDL_SetTextureColorMod(texture, 255, 100, 100);
    }

    renderTexture(texture, renderer, screenPos[0], screenPos[1], GRID_CELL_SIZE, GRID_CELL_SIZE);

    // Réinitialiser la teinte
    if (enemy->hitTimer > 0) {
        SDL_SetTextureColorMod(texture, 255, 255, 255);
    }

    // Barre de vie pour les ennemis avec plus d'une vie (tanks)
    int maxLives = getEnemyLives(enemy->enemyType);
    if (maxLives > 1) {
        int barWidth = GRID_CELL_SIZE - 10;
        int barHeight = 4;
        int barX = screenPos[0] + 5;
        int barY = screenPos[1] - 6;

        // Fond de la barre (gris)
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_Rect bgRect = {barX, barY, barWidth, barHeight};
        SDL_RenderFillRect(renderer, &bgRect);

        // Barre de vie (vert -> jaune -> rouge selon les PV)
        int healthWidth = (enemy->base.lives * barWidth) / maxLives;
        int r = 255 - (enemy->base.lives * 255 / maxLives);
        int g = (enemy->base.lives * 255 / maxLives);
        SDL_SetRenderDrawColor(renderer, r, g, 0, 255);
        SDL_Rect healthRect = {barX, barY, healthWidth, barHeight};
        SDL_RenderFillRect(renderer, &healthRect);

        // Bordure
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &bgRect);
    }
}

void Enemy_destroy(Enemy* enemy) {
    SpriteSet_destroy(&enemy->sprites);
    Character_destroy(&enemy->base);
    enemy->isActive = false;
}
