/**
 * @file link.h
 * @brief Gestion du personnage jouable (Link)
 */

#ifndef NUPRC_LINK_H
#define NUPRC_LINK_H

#include "character.h"
#include "animation.h"

#define LINK_ATTACK_COOLDOWN    25
#define LINK_ATTACK_ACTIVE_TIME 15
#define LINK_INVINCIBILITY_TIME 90
#define LINK_ATTACK_RANGE       1

typedef enum {
    LINK_DIR_UP,
    LINK_DIR_DOWN,
    LINK_DIR_LEFT,
    LINK_DIR_RIGHT
} LinkDirection;

typedef struct {
    Character       base;
    LinkDirection   direction;
    bool            isAttacking;
    int             attackCooldown;
    int             invincibilityTimer;
    bool            isInvincible;
    SpriteSet       sprites;
    AnimationState  animation;
} Link;

void Link_init(Link* link, Map* map);
void Link_update(Link* link);
void Link_attack(Link* link);
void Link_takeDamage(Link* link, int damage);
void Link_getAttackPosition(const Link* link, int attackPos[2]);
bool Link_isAttacking(const Link* link);
void Link_move(Link* link, const int delta[2]);
void Link_draw(const Link* link, SDL_Renderer* renderer);
void Link_destroy(Link* link);

#endif
