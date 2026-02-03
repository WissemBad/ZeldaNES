/**
 * @file map.h
 * @brief Gestion de la carte du monde et des salles
 */

#ifndef NUPRC_MAP_H
#define NUPRC_MAP_H

#include "core.h"

typedef struct {
    bool isBlocking;
    char textureId;
} Tile;

typedef struct {
    int startX;
    int startY;
    int position[2];
} Room;

typedef struct {
    SDL_Renderer* renderer;
    int currentRoom[2];
    Room rooms[GRID_WORLD_HEIGHT / GRID_ROOM_HEIGHT][GRID_WORLD_WIDTH / GRID_ROOM_WIDTH];
    Tile world[GRID_WORLD_HEIGHT][GRID_WORLD_WIDTH];
    SDL_Texture** textures;
} Map;

void Map_init(Map* map, SDL_Renderer* renderer);
void Map_draw(const Map* map, bool drawGrid);
void Map_destroy(Map* map);
bool Map_isBlocking(const Map* map, const int pos[2]);
Room* Map_getRoom(Map* map, const int pos[2]);

void Room_getCenter(const Room* room, int center[2]);
bool Room_isInside(const Room* room, const int pos[2]);
void Room_handleTransition(Map* map, const int charPos[2]);

#endif
