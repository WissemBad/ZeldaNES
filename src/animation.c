/**
 * @file animation.c
 * @brief Implémentation des animations de sprites
 */

#include "animation.h"
#include "render.h"

#include <string.h>

//==============================================================================
// CHEMINS DES SPRITES
//==============================================================================

/** Chemins des sprites de Link (marche) */
static const char* LINK_WALK_PATHS[4][2] = {
    {"assets/textures/characters/link0.bmp", "assets/textures/characters/link1.bmp"},  // RIGHT
    {"assets/textures/characters/link2.bmp", "assets/textures/characters/link3.bmp"},  // UP
    {"assets/textures/characters/link4.bmp", "assets/textures/characters/link5.bmp"},  // LEFT
    {"assets/textures/characters/link6.bmp", "assets/textures/characters/link7.bmp"}   // DOWN
};

/** Chemins des sprites de Link (attaque) */
static const char* LINK_ATTACK_PATHS[4] = {
    "assets/textures/characters/linkSwordRight.bmp",  // RIGHT
    "assets/textures/characters/linkSwordUp.bmp",     // UP
    "assets/textures/characters/linkSwordLeft.bmp",   // LEFT
    "assets/textures/characters/linkSwordDown.bmp"    // DOWN
};

/** Chemins des sprites d'ennemi (marche) */
static const char* ENEMY_WALK_PATHS[4][2] = {
    {"assets/textures/characters/enemy0.bmp", "assets/textures/characters/enemy1.bmp"},  // RIGHT
    {"assets/textures/characters/enemy2.bmp", "assets/textures/characters/enemy3.bmp"},  // UP
    {"assets/textures/characters/enemy4.bmp", "assets/textures/characters/enemy5.bmp"},  // LEFT
    {"assets/textures/characters/enemy6.bmp", "assets/textures/characters/enemy7.bmp"}   // DOWN
};

//==============================================================================
// FONCTIONS PUBLIQUES
//==============================================================================

void Animation_init(AnimationState* anim) {
    anim->direction = ANIM_DIR_DOWN;
    anim->state = ANIM_STATE_IDLE;
    anim->currentFrame = 0;
    anim->frameCounter = 0;
    anim->isAnimating = false;
}

void Animation_update(AnimationState* anim) {
    if (!anim->isAnimating) return;

    anim->frameCounter++;

    if (anim->frameCounter >= ANIM_SPEED) {
        anim->frameCounter = 0;
        anim->currentFrame++;

        // Boucler l'animation de marche
        if (anim->state == ANIM_STATE_WALKING) {
            if (anim->currentFrame >= ANIM_WALK_FRAMES) {
                anim->currentFrame = 0;
            }
        }
        // L'attaque s'arrête automatiquement
        else if (anim->state == ANIM_STATE_ATTACKING) {
            if (anim->currentFrame >= ANIM_ATTACK_FRAMES) {
                anim->state = ANIM_STATE_IDLE;
                anim->isAnimating = false;
                anim->currentFrame = 0;
            }
        }
    }
}

void Animation_setDirection(AnimationState* anim, AnimDirection dir) {
    anim->direction = dir;
}

void Animation_setState(AnimationState* anim, AnimState state) {
    if (anim->state != state) {
        anim->state = state;
        anim->currentFrame = 0;
        anim->frameCounter = 0;
        anim->isAnimating = (state != ANIM_STATE_IDLE);
    }
}

void Animation_startWalk(AnimationState* anim, AnimDirection dir) {
    anim->direction = dir;
    if (anim->state != ANIM_STATE_ATTACKING) {
        anim->state = ANIM_STATE_WALKING;
        anim->isAnimating = true;
    }
}

void Animation_startAttack(AnimationState* anim) {
    anim->state = ANIM_STATE_ATTACKING;
    anim->currentFrame = 0;
    anim->frameCounter = 0;
    anim->isAnimating = true;
}

void Animation_stop(AnimationState* anim) {
    if (anim->state != ANIM_STATE_ATTACKING) {
        anim->state = ANIM_STATE_IDLE;
        anim->isAnimating = false;
        anim->currentFrame = 0;
    }
}

SDL_Texture* Animation_getCurrentTexture(const AnimationState* anim, const SpriteSet* sprites) {
    if (!sprites) return NULL;

    int dir = (int)anim->direction;

    switch (anim->state) {
        case ANIM_STATE_ATTACKING:
            if (sprites->attack[dir]) {
                return sprites->attack[dir];
            }
            // Fallback sur le sprite de marche
            return sprites->walk[dir][0];

        case ANIM_STATE_WALKING:
            return sprites->walk[dir][anim->currentFrame % ANIM_WALK_FRAMES];

        case ANIM_STATE_IDLE:
        default:
            // En idle, on affiche la première frame de la direction actuelle
            return sprites->walk[dir][0];
    }
}

void SpriteSet_loadLink(SpriteSet* sprites, SDL_Renderer* renderer) {
    memset(sprites, 0, sizeof(SpriteSet));

    // Charger les sprites de marche
    for (int dir = 0; dir < 4; dir++) {
        for (int frame = 0; frame < ANIM_WALK_FRAMES; frame++) {
            sprites->walk[dir][frame] = loadTexture(LINK_WALK_PATHS[dir][frame], renderer);
        }
    }

    // Charger les sprites d'attaque
    for (int dir = 0; dir < 4; dir++) {
        sprites->attack[dir] = loadTexture(LINK_ATTACK_PATHS[dir], renderer);
    }
}

void SpriteSet_loadEnemy(SpriteSet* sprites, SDL_Renderer* renderer) {
    memset(sprites, 0, sizeof(SpriteSet));

    // Charger les sprites de marche
    for (int dir = 0; dir < 4; dir++) {
        for (int frame = 0; frame < ANIM_WALK_FRAMES; frame++) {
            sprites->walk[dir][frame] = loadTexture(ENEMY_WALK_PATHS[dir][frame], renderer);
        }
    }

    // Les ennemis n'ont pas de sprites d'attaque, mettre à NULL
    for (int dir = 0; dir < 4; dir++) {
        sprites->attack[dir] = NULL;
    }
}

void SpriteSet_destroy(SpriteSet* sprites) {
    if (!sprites) return;

    // Libérer les sprites de marche
    for (int dir = 0; dir < 4; dir++) {
        for (int frame = 0; frame < ANIM_WALK_FRAMES; frame++) {
            if (sprites->walk[dir][frame]) {
                SDL_DestroyTexture(sprites->walk[dir][frame]);
                sprites->walk[dir][frame] = NULL;
            }
        }
    }

    // Libérer les sprites d'attaque
    for (int dir = 0; dir < 4; dir++) {
        if (sprites->attack[dir]) {
            SDL_DestroyTexture(sprites->attack[dir]);
            sprites->attack[dir] = NULL;
        }
    }
}
