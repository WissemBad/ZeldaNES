#ifndef NUPRC_ANIMATION_H
#define NUPRC_ANIMATION_H

#include "core.h"

#define ANIM_WALK_FRAMES    2
#define ANIM_ATTACK_FRAMES  1
#define ANIM_SPEED          8

typedef enum {
    ANIM_DIR_RIGHT = 0,
    ANIM_DIR_UP    = 1,
    ANIM_DIR_LEFT  = 2,
    ANIM_DIR_DOWN  = 3
} AnimDirection;

typedef enum {
    ANIM_STATE_IDLE,
    ANIM_STATE_WALKING,
    ANIM_STATE_ATTACKING
} AnimState;

typedef struct {
    SDL_Texture* walk[4][ANIM_WALK_FRAMES];
    SDL_Texture* attack[4];
} SpriteSet;

typedef struct {
    AnimDirection direction;
    AnimState     state;
    int           currentFrame;
    int           frameCounter;
    bool          isAnimating;
} AnimationState;

void Animation_init(AnimationState* anim);
void Animation_update(AnimationState* anim);
void Animation_setDirection(AnimationState* anim, AnimDirection dir);
void Animation_setState(AnimationState* anim, AnimState state);
void Animation_startWalk(AnimationState* anim, AnimDirection dir);
void Animation_startAttack(AnimationState* anim);
void Animation_stop(AnimationState* anim);
SDL_Texture* Animation_getCurrentTexture(const AnimationState* anim, const SpriteSet* sprites);

void SpriteSet_loadLink(SpriteSet* sprites, SDL_Renderer* renderer);
void SpriteSet_loadEnemy(SpriteSet* sprites, SDL_Renderer* renderer);
void SpriteSet_destroy(SpriteSet* sprites);

#endif
