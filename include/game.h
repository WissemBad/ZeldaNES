/**
 * @file game.h
 * @brief Structure principale du jeu et fonctions du cycle de vie
 */

#ifndef NUPRC_GAME_H
#define NUPRC_GAME_H

#include "core.h"
#include "map.h"
#include "link.h"
#include "enemy.h"

//==============================================================================
// STRUCTURES
//==============================================================================

/** Structure principale du jeu */
typedef struct {
    RenderState render;                 /**< État du système de rendu */
    GameState   state;                  /**< État actuel du jeu */
    PlayerStats stats;                  /**< Statistiques du joueur */
    Map         map;                    /**< Carte du monde */
    Link        player;                 /**< Joueur (Link) */
    Enemy       enemies[GAME_MAX_ENEMIES]; /**< Tableau des ennemis */
    int         enemyCount;             /**< Nombre d'ennemis actifs */
    bool        running;                /**< true si le jeu tourne */
} Game;

//==============================================================================
// FONCTIONS - CYCLE DE VIE
//==============================================================================

/** Initialise le jeu et toutes ses ressources */
void Game_init(Game* game);

/** Lance la boucle principale du jeu */
void Game_run(Game* game);

/** Libère toutes les ressources et ferme le jeu proprement */
void Game_destroy(Game* game);

//==============================================================================
// FONCTIONS - BOUCLE DE JEU
//==============================================================================

/** Traite les entrées utilisateur (clavier) */
void Game_handleInput(Game* game);

/** Met à jour l'état du jeu (logique, IA, transitions) */
void Game_update(Game* game);

/** Affiche tous les éléments du jeu à l'écran (carte, personnages, HUD) */
void Game_render(Game* game);

#endif // NUPRC_GAME_H
