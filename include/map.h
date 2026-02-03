/**
 * @file map.h
 * @brief Gestion de la carte du monde et des salles
 */

#ifndef NUPRC_MAP_H
#define NUPRC_MAP_H

#include "core.h"

//==============================================================================
// STRUCTURES
//==============================================================================

/** Une tuile de la carte */
typedef struct {
    bool isBlocking;    /**< true si la tuile bloque le passage */
    char textureId;     /**< Index de la texture dans l'atlas (0-159) */
} Tile;

/** Une salle de la carte (16x11 tuiles) */
typedef struct {
    int startX;         /**< Coordonnée X de départ dans le monde */
    int startY;         /**< Coordonnée Y de départ dans le monde */
    int position[2];    /**< Position de la salle dans la grille des salles */
} Room;

/** Carte complète du monde */
typedef struct {
    SDL_Renderer* renderer;
    int currentRoom[2];
    Room rooms[GRID_WORLD_HEIGHT / GRID_ROOM_HEIGHT][GRID_WORLD_WIDTH / GRID_ROOM_WIDTH];
    Tile world[GRID_WORLD_HEIGHT][GRID_WORLD_WIDTH];
    SDL_Texture** textures;
} Map;

//==============================================================================
// FONCTIONS - GESTION DE LA CARTE
//==============================================================================

/** Initialise la carte avec ses salles et tuiles */
void Map_init(Map* map, SDL_Renderer* renderer);

/** Affiche la salle actuelle de la carte */
void Map_draw(const Map* map, bool drawGrid);

/** Libère les ressources allouées pour la carte */
void Map_destroy(Map* map);

/** Vérifie si une position dans le monde est bloquante */
bool Map_isBlocking(const Map* map, const int pos[2]);

/** Récupère une salle à partir de sa position dans la grille */
Room* Map_getRoom(Map* map, const int pos[2]);

//==============================================================================
// FONCTIONS - GESTION DES SALLES
//==============================================================================

/** Calcule la position centrale d'une salle */
void Room_getCenter(const Room* room, int center[2]);

/** Vérifie si une position se trouve à l'intérieur d'une salle */
bool Room_isInside(const Room* room, const int pos[2]);

/** Gère les transitions entre salles selon la position du personnage */
void Room_handleTransition(Map* map, const int charPos[2]);

#endif // NUPRC_MAP_H
