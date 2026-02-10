#include "animation.h"
#include "render.h"
#include <string.h>

static const char* LINK_WALK[4][2] = {
    {"textures/characters/link0.bmp", "textures/characters/link1.bmp"},
    {"textures/characters/link2.bmp", "textures/characters/link3.bmp"},
    {"textures/characters/link4.bmp", "textures/characters/link5.bmp"},
    {"textures/characters/link6.bmp", "textures/characters/link7.bmp"}
};

static const char* LINK_ATTACK[4] = {
    "textures/characters/linkSwordRight.bmp",
    "textures/characters/linkSwordUp.bmp",
    "textures/characters/linkSwordLeft.bmp",
    "textures/characters/linkSwordDown.bmp"
};

static const char* ENEMY_WALK[4][2] = {
    {"textures/characters/enemy0.bmp", "textures/characters/enemy1.bmp"},
    {"textures/characters/enemy2.bmp", "textures/characters/enemy3.bmp"},
    {"textures/characters/enemy4.bmp", "textures/characters/enemy5.bmp"},
    {"textures/characters/enemy6.bmp", "textures/characters/enemy7.bmp"}
};

void Animation_init(AnimationState* anim) {
    if (!anim) return;
    anim->direction = ANIM_DIR_DOWN;
    anim->state = ANIM_STATE_IDLE;
    anim->currentFrame = 0;
    anim->frameCounter = 0;
    anim->isAnimating = false;
}

void Animation_update(AnimationState* anim) {
    if (!anim || !anim->isAnimating) return;

    anim->frameCounter++;
    if (anim->frameCounter >= ANIM_SPEED) {
        anim->frameCounter = 0;
        anim->currentFrame++;

        if (anim->state == ANIM_STATE_WALKING && anim->currentFrame >= ANIM_WALK_FRAMES) {
            anim->currentFrame = 0;
        }
        else if (anim->state == ANIM_STATE_ATTACKING && anim->currentFrame >= ANIM_ATTACK_FRAMES) {
            anim->state = ANIM_STATE_IDLE;
            anim->isAnimating = false;
            anim->currentFrame = 0;
        }
    }
}

void Animation_setDirection(AnimationState* anim, AnimDirection dir) {
    if (anim) anim->direction = dir;
}

void Animation_setState(AnimationState* anim, AnimState state) {
    if (!anim || anim->state == state) return;
    anim->state = state;
    anim->currentFrame = 0;
    anim->frameCounter = 0;
    anim->isAnimating = (state != ANIM_STATE_IDLE);
}

void Animation_startWalk(AnimationState* anim, AnimDirection dir) {
    if (!anim) return;
    anim->direction = dir;
    if (anim->state != ANIM_STATE_ATTACKING) {
        anim->state = ANIM_STATE_WALKING;
        anim->isAnimating = true;
    }
}

void Animation_startAttack(AnimationState* anim) {
    if (!anim) return;
    anim->state = ANIM_STATE_ATTACKING;
    anim->currentFrame = 0;
    anim->frameCounter = 0;
    anim->isAnimating = true;
}

void Animation_stop(AnimationState* anim) {
    if (!anim || anim->state == ANIM_STATE_ATTACKING) return;
    anim->state = ANIM_STATE_IDLE;
    anim->isAnimating = false;
    anim->currentFrame = 0;
}

SDL_Texture* Animation_getCurrentTexture(const AnimationState* anim, const SpriteSet* sprites) {
    if (!anim || !sprites) return NULL;

    int dir = (int)anim->direction;

    if (anim->state == ANIM_STATE_ATTACKING && sprites->attack[dir]) {
        return sprites->attack[dir];
    }
    if (anim->state == ANIM_STATE_WALKING) {
        return sprites->walk[dir][anim->currentFrame % ANIM_WALK_FRAMES];
    }
    return sprites->walk[dir][0];
}

void SpriteSet_loadLink(SpriteSet* sprites, SDL_Renderer* renderer) {
    if (!sprites || !renderer) return;
    memset(sprites, 0, sizeof(SpriteSet));

    for (int d = 0; d < 4; d++) {
        for (int f = 0; f < ANIM_WALK_FRAMES; f++) {
            sprites->walk[d][f] = loadTexture(LINK_WALK[d][f], renderer);
        }
        sprites->attack[d] = loadTexture(LINK_ATTACK[d], renderer);
    }
}

void SpriteSet_loadEnemy(SpriteSet* sprites, SDL_Renderer* renderer) {
    if (!sprites || !renderer) return;
    memset(sprites, 0, sizeof(SpriteSet));

    for (int d = 0; d < 4; d++) {
        for (int f = 0; f < ANIM_WALK_FRAMES; f++) {
            sprites->walk[d][f] = loadTexture(ENEMY_WALK[d][f], renderer);
        }
        sprites->attack[d] = NULL;
    }
}

void SpriteSet_destroy(SpriteSet* sprites) {
    if (!sprites) return;

    for (int d = 0; d < 4; d++) {
        for (int f = 0; f < ANIM_WALK_FRAMES; f++) {
            if (sprites->walk[d][f]) {
                SDL_DestroyTexture(sprites->walk[d][f]);
                sprites->walk[d][f] = NULL;
            }
        }
        if (sprites->attack[d]) {
            SDL_DestroyTexture(sprites->attack[d]);
            sprites->attack[d] = NULL;
        }
    }
}
