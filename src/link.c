#include "link.h"
#include "map.h"
#include "render.h"
#include "utils.h"

static AnimDirection toAnimDir(LinkDirection dir) {
    switch (dir) {
        case LINK_DIR_RIGHT: return ANIM_DIR_RIGHT;
        case LINK_DIR_UP:    return ANIM_DIR_UP;
        case LINK_DIR_LEFT:  return ANIM_DIR_LEFT;
        default:             return ANIM_DIR_DOWN;
    }
}

void Link_init(Link* link, Map* map) {
    if (!link || !map) return;

    Character_init(&link->base, CHAR_HERO, GAME_INITIAL_LIVES, map);
    link->direction = LINK_DIR_DOWN;
    link->isAttacking = false;
    link->attackCooldown = 0;
    link->invincibilityTimer = 0;
    link->isInvincible = false;
    link->base.texture = NULL;

    Animation_init(&link->animation);
    SpriteSet_loadLink(&link->sprites, map->renderer);
}

void Link_update(Link* link) {
    if (!link) return;

    if (link->attackCooldown > 0) {
        link->attackCooldown--;
    }
    if (link->isAttacking && link->attackCooldown == 0) {
        link->isAttacking = false;
        Animation_stop(&link->animation);
    }

    if (link->invincibilityTimer > 0) {
        link->invincibilityTimer--;
        if (link->invincibilityTimer == 0) {
            link->isInvincible = false;
        }
    }

    Animation_update(&link->animation);
}

void Link_attack(Link* link) {
    if (!link || link->isAttacking || link->attackCooldown > 0) return;

    link->isAttacking = true;
    link->attackCooldown = LINK_ATTACK_COOLDOWN;
    Animation_startAttack(&link->animation);
}

void Link_takeDamage(Link* link, int damage) {
    if (!link || link->isInvincible) return;

    link->base.lives -= damage;
    link->isInvincible = true;
    link->invincibilityTimer = LINK_INVINCIBILITY_TIME;
}

void Link_getAttackPosition(const Link* link, int attackPos[2]) {
    if (!link || !attackPos) return;

    Character_getGridPos(&link->base, attackPos);

    switch (link->direction) {
        case LINK_DIR_UP:    attackPos[1]--; break;
        case LINK_DIR_DOWN:  attackPos[1]++; break;
        case LINK_DIR_LEFT:  attackPos[0]--; break;
        case LINK_DIR_RIGHT: attackPos[0]++; break;
    }
}

bool Link_isAttacking(const Link* link) {
    return link && link->isAttacking;
}

void Link_move(Link* link, const int delta[2]) {
    if (!link || !delta) return;

    int gridPos[2];
    Character_getGridPos(&link->base, gridPos);
    int oldPos[2] = {gridPos[0], gridPos[1]};

    Character_move(&link->base, delta);

    Character_getGridPos(&link->base, gridPos);

    if (delta[0] > 0) link->direction = LINK_DIR_RIGHT;
    else if (delta[0] < 0) link->direction = LINK_DIR_LEFT;
    else if (delta[1] > 0) link->direction = LINK_DIR_DOWN;
    else if (delta[1] < 0) link->direction = LINK_DIR_UP;

    AnimDirection animDir = toAnimDir(link->direction);
    Animation_setDirection(&link->animation, animDir);

    if (oldPos[0] != gridPos[0] || oldPos[1] != gridPos[1]) {
        Animation_startWalk(&link->animation, animDir);
    }
}

void Link_moveSmooth(Link* link, float deltaX, float deltaY) {
    if (!link) return;
    if (deltaX == 0.0f && deltaY == 0.0f) return;

    Character_moveSmooth(&link->base, deltaX, deltaY);

    if (deltaX > 0) link->direction = LINK_DIR_RIGHT;
    else if (deltaX < 0) link->direction = LINK_DIR_LEFT;
    else if (deltaY > 0) link->direction = LINK_DIR_DOWN;
    else if (deltaY < 0) link->direction = LINK_DIR_UP;

    AnimDirection animDir = toAnimDir(link->direction);
    Animation_setDirection(&link->animation, animDir);

    if (deltaX != 0.0f || deltaY != 0.0f) {
        Animation_startWalk(&link->animation, animDir);
    }
}

void Link_draw(const Link* link, SDL_Renderer* renderer) {
    if (!link || !renderer) return;
    if (link->isInvincible && (link->invincibilityTimer / 5) % 2 == 0) return;

    SDL_Texture* tex = Animation_getCurrentTexture(&link->animation, &link->sprites);
    if (!tex) return;

    int screen[2];
    Camera_worldToScreenF(&link->base.map->camera, link->base.posX, link->base.posY, screen);
    renderTexture(tex, renderer, screen[0], screen[1], GRID_CELL_SIZE, GRID_CELL_SIZE);
}

void Link_destroy(Link* link) {
    if (!link) return;
    SpriteSet_destroy(&link->sprites);
    Character_destroy(&link->base);
}
