#ifndef GRID_H
#define GRID_H

#include "character.h"
#include "settings.h"
#include "IOManager.h"

/**
 * @brief Met à jour le plateau après un déplacement.
 * @param grid Grille à mettre à jour
 * @param character Personnage ayant bougé
 * @param direction Direction du déplacement
 */
void Grid_update(char grid[GRID_COLS][GRID_ROWS], Character character, Direction direction);

/**
 * @brief Trace graphiquement le plateau complet.
 * @param renderer Renderer SDL utilisé pour dessiner
 * @param endPosition Coordonnées (x,y) de la case de fin
 */
void Grid_draw(SDL_Renderer *renderer, int endPosition[2]);

/**
 * @brief Libère les ressources de la grille.
 */
void Grid_destroy(void);

#endif