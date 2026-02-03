/**
 * @file utils.h
 * @brief Fonctions utilitaires (conversion de coordonnées, mathématiques)
 */

#ifndef NUPRC_UTILS_H
#define NUPRC_UTILS_H

#include "core.h"

//==============================================================================
// FONCTIONS - CONVERSION DE COORDONNÉES
//==============================================================================

/** Convertit des coordonnées du monde vers des coordonnées écran (pixels) */
void worldToScreen(const int worldPos[2], int screenPos[2], const int roomPos[2]);

/** Convertit des coordonnées écran vers des coordonnées du monde */
void screenToWorld(const int screenPos[2], int worldPos[2], const int roomPos[2]);

//==============================================================================
// FONCTIONS - MATHÉMATIQUES
//==============================================================================

/** Calcule la distance de Manhattan entre deux positions */
int manhattanDistance(const int pos1[2], const int pos2[2]);

/** Vérifie si deux positions sont identiques */
bool positionsEqual(const int pos1[2], const int pos2[2]);

/** Copie une position dans une autre */
void copyPosition(const int src[2], int dest[2]);

#endif // NUPRC_UTILS_H
