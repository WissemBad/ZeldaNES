/**
 * @file link.c
 * @brief Implémentation du personnage jouable (Link)
 */

#include "link.h"
#include "render.h"

//==============================================================================
// FONCTIONS PUBLIQUES
//==============================================================================

void Link_init(Link* link, Map* map) {
    Character_init(&link->base, CHAR_HERO, GAME_INITIAL_LIVES, map);
    link->direction = LINK_DIR_DOWN;
    link->isAttacking = false;
    link->attackCooldown = 0;
    link->invincibilityTimer = 0;
    link->isInvincible = false;
    link->base.texture = loadTexture(ASSET_TEXTURE_LINK, map->renderer);
}

void Link_update(Link* link) {
    // Gestion du cooldown d'attaque
    if (link->attackCooldown > 0) {
        link->attackCooldown--;
    }

    if (link->isAttacking && link->attackCooldown == 0) {
        link->isAttacking = false;
    }

    // Gestion de l'invincibilité
    if (link->invincibilityTimer > 0) {
        link->invincibilityTimer--;
        if (link->invincibilityTimer == 0) {
            link->isInvincible = false;
        }
    }
}

void Link_attack(Link* link) {
    if (link->isAttacking || link->attackCooldown > 0) {
        return;
    }

    link->isAttacking = true;
    link->attackCooldown = LINK_ATTACK_COOLDOWN;
}

void Link_takeDamage(Link* link, int damage) {
    // Ne prend pas de dégâts si invincible
    if (link->isInvincible) {
        return;
    }

    link->base.lives -= damage;

    // Active l'invincibilité temporaire
    link->isInvincible = true;
    link->invincibilityTimer = LINK_INVINCIBILITY_TIME;
}

void Link_getAttackPosition(const Link* link, int attackPos[2]) {
    attackPos[0] = link->base.pos[0];
    attackPos[1] = link->base.pos[1];

    switch (link->direction) {
        case LINK_DIR_UP:
            attackPos[1] -= LINK_ATTACK_RANGE;
            break;
        case LINK_DIR_DOWN:
            attackPos[1] += LINK_ATTACK_RANGE;
            break;
        case LINK_DIR_LEFT:
            attackPos[0] -= LINK_ATTACK_RANGE;
            break;
        case LINK_DIR_RIGHT:
            attackPos[0] += LINK_ATTACK_RANGE;
            break;
    }
}

bool Link_isAttacking(const Link* link) {
    return link->isAttacking;
}

void Link_destroy(Link* link) {
    Character_destroy(&link->base);
}
