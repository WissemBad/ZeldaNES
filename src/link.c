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
    link->base.texture = loadTexture(ASSET_TEXTURE_LINK, map->renderer);
}

void Link_update(Link* link) {
    if (link->attackCooldown > 0) {
        link->attackCooldown--;
    }

    if (link->isAttacking && link->attackCooldown == 0) {
        link->isAttacking = false;
    }
}

void Link_attack(Link* link) {
    if (link->isAttacking || link->attackCooldown > 0) {
        return;
    }

    link->isAttacking = true;
    link->attackCooldown = 30;
}

void Link_destroy(Link* link) {
    Character_destroy(&link->base);
}
