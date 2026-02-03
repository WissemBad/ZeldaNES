/**
 * @file animation.h
 * @brief Gestion des animations de sprites
 */

#ifndef NUPRC_ANIMATION_H
#define NUPRC_ANIMATION_H

#include "core.h"

//==============================================================================
// CONSTANTES
//==============================================================================

/** Nombre de frames par animation de marche */
#define ANIM_WALK_FRAMES        2

/** Nombre de frames pour l'animation d'attaque */
#define ANIM_ATTACK_FRAMES      1

/** Vitesse d'animation (frames de jeu par frame d'animation) */
#define ANIM_SPEED              8

/** Nombre maximum de textures par direction */
#define ANIM_MAX_TEXTURES       8

//==============================================================================
// ÉNUMÉRATIONS
//==============================================================================

/** Direction d'animation */
typedef enum {
    ANIM_DIR_RIGHT = 0,
    ANIM_DIR_UP    = 1,
    ANIM_DIR_LEFT  = 2,
    ANIM_DIR_DOWN  = 3
} AnimDirection;

/** État de l'animation */
typedef enum {
    ANIM_STATE_IDLE,
    ANIM_STATE_WALKING,
    ANIM_STATE_ATTACKING
} AnimState;

//==============================================================================
// STRUCTURES
//==============================================================================

/** Ensemble de sprites pour un personnage */
typedef struct {
    SDL_Texture* walk[4][ANIM_WALK_FRAMES];    /**< Sprites de marche [direction][frame] */
    SDL_Texture* attack[4];                     /**< Sprites d'attaque [direction] */
} SpriteSet;

/** État d'animation d'un personnage */
typedef struct {
    AnimDirection direction;        /**< Direction actuelle */
    AnimState     state;            /**< État actuel de l'animation */
    int           currentFrame;     /**< Frame actuelle de l'animation */
    int           frameCounter;     /**< Compteur pour la vitesse d'animation */
    bool          isAnimating;      /**< true si en cours d'animation */
} AnimationState;

//==============================================================================
// FONCTIONS
//==============================================================================

/** Initialise l'état d'animation */
void Animation_init(AnimationState* anim);

/** Met à jour l'animation (appelé chaque frame) */
void Animation_update(AnimationState* anim);

/** Change la direction de l'animation */
void Animation_setDirection(AnimationState* anim, AnimDirection dir);

/** Change l'état de l'animation */
void Animation_setState(AnimationState* anim, AnimState state);

/** Démarre une animation de marche */
void Animation_startWalk(AnimationState* anim, AnimDirection dir);

/** Démarre une animation d'attaque */
void Animation_startAttack(AnimationState* anim);

/** Arrête l'animation (retour à idle) */
void Animation_stop(AnimationState* anim);

/** Récupère la texture actuelle à afficher */
SDL_Texture* Animation_getCurrentTexture(const AnimationState* anim, const SpriteSet* sprites);

/** Charge les sprites de Link */
void SpriteSet_loadLink(SpriteSet* sprites, SDL_Renderer* renderer);

/** Charge les sprites d'un ennemi */
void SpriteSet_loadEnemy(SpriteSet* sprites, SDL_Renderer* renderer);

/** Libère les ressources d'un SpriteSet */
void SpriteSet_destroy(SpriteSet* sprites);

#endif // NUPRC_ANIMATION_H
