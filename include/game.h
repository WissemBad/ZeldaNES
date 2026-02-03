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
#include "menu.h"

//==============================================================================
// STRUCTURES
//==============================================================================

/** Structure principale du jeu */
typedef struct {
    RenderState render;                 /**< État du système de rendu */
    GameState   state;                  /**< État actuel du jeu */
    GameState   previousState;          /**< État précédent (pour retour de pause) */
    PlayerStats stats;                  /**< Statistiques du joueur */
    Map         map;                    /**< Carte du monde */
    Link        player;                 /**< Joueur (Link) */
    Enemy       enemies[GAME_MAX_ENEMIES]; /**< Tableau des ennemis */
    int         enemyCount;             /**< Nombre d'ennemis actifs */
    bool        running;                /**< true si le jeu tourne */
    Menu        menu;                   /**< Menu actif (principal, pause, game over) */
} Game;

//==============================================================================
// FONCTIONS - CYCLE DE VIE
//==============================================================================

/** Initialise le jeu et charge toutes les ressources nécessaires */
void Game_init(Game* game);

/** Lance la boucle principale du jeu (input, update, render) */
void Game_run(Game* game);

/** Libère toutes les ressources et ferme le jeu proprement */
void Game_destroy(Game* game);

//==============================================================================
// FONCTIONS - GESTION DES ÉTATS
//==============================================================================

/** Change l'état du jeu et initialise le menu correspondant si nécessaire */
void Game_setState(Game* game, GameState newState);

/** Démarre une nouvelle partie (réinitialise le joueur et la carte) */
void Game_startNewGame(Game* game);

/** Met le jeu en pause */
void Game_pause(Game* game);

/** Reprend le jeu après une pause */
void Game_resume(Game* game);

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
