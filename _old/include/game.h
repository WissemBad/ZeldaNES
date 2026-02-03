#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "character.h"
#include "settings.h"

/**
 * @brief Structure principale du jeu.
 */
typedef struct gameStruct {
    Character player;
    char grid[GRID_COLS][GRID_ROWS];
    int endPosition[2];
    bool isRunning;
    bool hasWon;
} Game;

/**
 * @brief Initialise la structure principale du jeu.
 * @param game Pointeur vers la structure Game à initialiser
 * @param renderer Renderer SDL utilisé pour charger les textures
 * @param playerName Nom du joueur
 */
void Game_init(Game *game, SDL_Renderer *renderer, char* playerName);

/**
 * @brief Initialise la grille de jeu avec les positions du joueur et de l'arrivée.
 * @param grid Grille à initialiser
 * @param player Personnage du joueur
 * @param endPosition Position de l'arrivée
 */
void Game_initGrid(char grid[GRID_COLS][GRID_ROWS], Character player, int endPosition[2]);

/**
 * @brief Vérifie si la partie est terminée (victoire ou défaite).
 * @param character Personnage à vérifier
 * @param endPosition Position de l'arrivée
 * @param hasWon Pointeur vers le booléen indiquant la victoire
 * @return true si la partie est finie, false sinon
 */
bool Game_isEnd(Character character, int endPosition[2], bool *hasWon);

/**
 * @brief Vérifie si un déplacement ferait sortir le personnage du plateau.
 * @param character Personnage à vérifier
 * @param direction Direction du déplacement envisagé
 * @return true si le déplacement sort du plateau, false sinon
 */
bool Game_isExceed(Character character, Direction direction);

/**
 * @brief Met à jour le jeu après un déplacement du joueur.
 * @param game Pointeur vers la structure Game
 * @param direction Direction du déplacement
 */
void Game_update(Game *game, Direction direction);

/**
 * @brief Dessine le jeu (grille + joueur).
 * @param game Pointeur vers la structure Game
 * @param renderer Renderer SDL
 */
void Game_draw(Game *game, SDL_Renderer *renderer);

/**
 * @brief Affiche l'écran de fin de partie et attend une action utilisateur.
 * @param game Pointeur vers la structure Game
 * @param renderer Renderer SDL
 * @param font Police utilisée pour l'affichage
 */
void Game_end(Game *game, SDL_Renderer *renderer, TTF_Font *font);

/**
 * @brief Libère toutes les ressources allouées au jeu.
 * @param game Pointeur vers la structure Game
 */
void Game_destroy(Game *game);

#endif
