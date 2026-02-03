/**
 * @file link.c
 * @brief Implémentation du personnage jouable (Link)
 */

#include "link.h"
#include "render.h"
#include "utils.h"

//==============================================================================
// FONCTIONS PRIVÉES
//==============================================================================

/**
 * @brief Convertit LinkDirection en AnimDirection
 */
static AnimDirection linkDirToAnimDir(LinkDirection dir) {
    switch (dir) {
        case LINK_DIR_RIGHT: return ANIM_DIR_RIGHT;
        case LINK_DIR_UP:    return ANIM_DIR_UP;
        case LINK_DIR_LEFT:  return ANIM_DIR_LEFT;
        case LINK_DIR_DOWN:  return ANIM_DIR_DOWN;
        default:             return ANIM_DIR_DOWN;
    }
}

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

    // Initialiser les animations
    Animation_init(&link->animation);
    SpriteSet_loadLink(&link->sprites, map->renderer);

    // La texture de base n'est plus utilisée directement
    link->base.texture = NULL;
}

void Link_update(Link* link) {
    // Gestion du cooldown d'attaque
    if (link->attackCooldown > 0) {
        link->attackCooldown--;
    }

    if (link->isAttacking && link->attackCooldown == 0) {
        link->isAttacking = false;
        Animation_stop(&link->animation);
    }

    // Gestion de l'invincibilité
    if (link->invincibilityTimer > 0) {
        link->invincibilityTimer--;
        if (link->invincibilityTimer == 0) {
            link->isInvincible = false;
        }
    }

    // Mise à jour de l'animation
    Animation_update(&link->animation);
}

void Link_attack(Link* link) {
    if (link->isAttacking || link->attackCooldown > 0) {
        return;
    }

    link->isAttacking = true;
    link->attackCooldown = LINK_ATTACK_COOLDOWN;

    // Démarrer l'animation d'attaque
    Animation_startAttack(&link->animation);
}

void Link_takeDamage(Link* link, int damage) {
    if (link->isInvincible) {
        return;
    }

    link->base.lives -= damage;
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

void Link_move(Link* link, const int delta[2]) {
    // Sauvegarder l'ancienne position
    int oldPos[2] = {link->base.pos[0], link->base.pos[1]};

    // Déplacer le personnage
    Character_move(&link->base, delta);

    // Déterminer la direction
    if (delta[0] > 0) link->direction = LINK_DIR_RIGHT;
    else if (delta[0] < 0) link->direction = LINK_DIR_LEFT;
    else if (delta[1] > 0) link->direction = LINK_DIR_DOWN;
    else if (delta[1] < 0) link->direction = LINK_DIR_UP;

    // Mettre à jour l'animation
    AnimDirection animDir = linkDirToAnimDir(link->direction);
    Animation_setDirection(&link->animation, animDir);

    // Démarrer l'animation de marche si on a bougé
    if (oldPos[0] != link->base.pos[0] || oldPos[1] != link->base.pos[1]) {
        Animation_startWalk(&link->animation, animDir);
    }
}

void Link_draw(const Link* link, SDL_Renderer* renderer) {
    // Effet de clignotement pendant l'invincibilité
    if (link->isInvincible && (link->invincibilityTimer / 5) % 2 == 0) {
        return;
    }

    // Récupérer la texture animée
    SDL_Texture* texture = Animation_getCurrentTexture(&link->animation, &link->sprites);
    if (!texture) return;

    // Convertir en coordonnées écran
    int screenPos[2];
    worldToScreen(link->base.pos, screenPos, link->base.map->currentRoom);

    // Dessiner
    renderTexture(texture, renderer, screenPos[0], screenPos[1], GRID_CELL_SIZE, GRID_CELL_SIZE);
}

void Link_destroy(Link* link) {
    SpriteSet_destroy(&link->sprites);
    Character_destroy(&link->base);
}
