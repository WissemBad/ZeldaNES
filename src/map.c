/**
 * @file map.c
 * @brief Implémentation de la gestion de la carte et des salles
 */

#include "map.h"
#include "render.h"

//==============================================================================
// FONCTIONS PRIVÉES
//==============================================================================

/**
 * Crée une tuile à partir des données de blocage et de texture
 */
static Tile createTile(const char blockingChar, const int textureId) {
    Tile tile;
    tile.isBlocking = (blockingChar == 'X');
    tile.textureId = (char)textureId;
    return tile;
}

/**
 * Charge les données complètes de la carte depuis les fichiers
 */
static void loadMapData(Map* map) {
    char blocking[GRID_WORLD_HEIGHT][GRID_WORLD_WIDTH];
    int overworld[GRID_WORLD_HEIGHT][GRID_WORLD_WIDTH];

    loadBlockingMap(ASSET_MAP_BLOCKING, blocking);
    loadWorldMap(ASSET_MAP_WORLD, overworld);

    for (int row = 0; row < GRID_WORLD_HEIGHT; row++) {
        for (int col = 0; col < GRID_WORLD_WIDTH; col++) {
            map->world[row][col] = createTile(blocking[row][col], overworld[row][col]);
        }
    }
}

/**
 * Initialise une salle à partir de sa position dans la grille
 */
static Room createRoom(const int roomX, const int roomY) {
    Room room;
    room.startX = roomX * GRID_ROOM_WIDTH;
    room.startY = roomY * GRID_ROOM_HEIGHT;
    room.position[0] = roomX;
    room.position[1] = roomY;
    return room;
}

/**
 * Affiche la grille de la salle (pour debug)
 */
static void drawRoomGrid(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Lignes horizontales
    for (int row = 1; row < GRID_ROOM_HEIGHT; row++) {
        SDL_RenderDrawLine(renderer,
            0, row * GRID_CELL_SIZE,
            WINDOW_WIDTH - 1, row * GRID_CELL_SIZE
        );
    }

    // Lignes verticales
    for (int col = 1; col < GRID_ROOM_WIDTH; col++) {
        SDL_RenderDrawLine(renderer,
            col * GRID_CELL_SIZE, 0,
            col * GRID_CELL_SIZE, WINDOW_HEIGHT - WINDOW_TEXTAREA_HEIGHT - 1
        );
    }

    // Bordure
    const SDL_Rect border = {0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT - WINDOW_TEXTAREA_HEIGHT};
    SDL_RenderDrawRect(renderer, &border);
}

/**
 * @brief Change la salle active si la nouvelle position est valide
 */
static void changeRoom(Map* map, int roomX, int roomY) {
    const int maxRoomX = GRID_WORLD_WIDTH / GRID_ROOM_WIDTH;
    const int maxRoomY = GRID_WORLD_HEIGHT / GRID_ROOM_HEIGHT;

    if (roomX < 0 || roomX >= maxRoomX || roomY < 0 || roomY >= maxRoomY) {
        return;
    }

    map->currentRoom[0] = roomX;
    map->currentRoom[1] = roomY;
}

//==============================================================================
// FONCTIONS PUBLIQUES - GESTION DE LA CARTE
//==============================================================================

void Map_init(Map* map, SDL_Renderer* renderer) {
    map->renderer = renderer;

    // Charger les données de la carte
    loadMapData(map);

    // Charger les textures des tuiles
    map->textures = loadTileTextures(ASSET_MAP_TILES, renderer);
    if (map->textures == NULL) {
        fprintf(stderr, "Erreur: impossible de charger les textures de la carte\n");
        exit(EXIT_FAILURE);
    }

    // Initialiser toutes les salles
    const int maxRoomX = GRID_WORLD_WIDTH / GRID_ROOM_WIDTH;
    const int maxRoomY = GRID_WORLD_HEIGHT / GRID_ROOM_HEIGHT;

    for (int row = 0; row < maxRoomY; row++) {
        for (int col = 0; col < maxRoomX; col++) {
            map->rooms[row][col] = createRoom(col, row);
        }
    }

    // Salle initiale
    map->currentRoom[0] = 0;
    map->currentRoom[1] = 0;
}

void Map_draw(const Map* map, bool drawGrid) {
    const Room* room = &map->rooms[map->currentRoom[1]][map->currentRoom[0]];

    // Dessiner chaque tuile de la salle
    for (int row = 0; row < GRID_ROOM_HEIGHT; row++) {
        for (int col = 0; col < GRID_ROOM_WIDTH; col++) {
            const Tile tile = map->world[room->startY + row][room->startX + col];
            const int tileIndex = (int)tile.textureId;

            renderTexture(
                map->textures[tileIndex], map->renderer,
                col * GRID_CELL_SIZE, row * GRID_CELL_SIZE,
                GRID_CELL_SIZE, GRID_CELL_SIZE
            );
        }
    }

    if (drawGrid) {
        drawRoomGrid(map->renderer);
    }
}

void Map_destroy(Map* map) {
    if (map->textures == NULL) {
        return;
    }

    for (int i = 0; i < MAP_TILES_COUNT; i++) {
        if (map->textures[i] != NULL) {
            SDL_DestroyTexture(map->textures[i]);
        }
    }

    free(map->textures);
    map->textures = NULL;
}

bool Map_isBlocking(const Map* map, const int pos[2]) {
    if (pos[0] < 0 || pos[0] >= GRID_WORLD_WIDTH ||
        pos[1] < 0 || pos[1] >= GRID_WORLD_HEIGHT) {
        return true;
    }
    return map->world[pos[1]][pos[0]].isBlocking;
}

Room* Map_getRoom(Map* map, const int pos[2]) {
    const int maxRoomX = GRID_WORLD_WIDTH / GRID_ROOM_WIDTH;
    const int maxRoomY = GRID_WORLD_HEIGHT / GRID_ROOM_HEIGHT;

    if (pos[0] < 0 || pos[0] >= maxRoomX ||
        pos[1] < 0 || pos[1] >= maxRoomY) {
        return NULL;
    }

    return &map->rooms[pos[1]][pos[0]];
}

//==============================================================================
// FONCTIONS PUBLIQUES - GESTION DES SALLES
//==============================================================================

void Room_getCenter(const Room* room, int center[2]) {
    center[0] = room->startX + GRID_ROOM_WIDTH / 2;
    center[1] = room->startY + GRID_ROOM_HEIGHT / 2;
}

bool Room_isInside(const Room* room, const int pos[2]) {
    return pos[0] >= room->startX &&
           pos[0] < room->startX + GRID_ROOM_WIDTH &&
           pos[1] >= room->startY &&
           pos[1] < room->startY + GRID_ROOM_HEIGHT;
}

void Room_handleTransition(Map* map, const int charPos[2]) {
    Room* currentRoom = Map_getRoom(map, map->currentRoom);
    if (currentRoom == NULL || Room_isInside(currentRoom, charPos)) {
        return;
    }

    int nextRoomX = map->currentRoom[0];
    int nextRoomY = map->currentRoom[1];

    if (charPos[0] < currentRoom->startX) {
        nextRoomX--;
    } else if (charPos[0] >= currentRoom->startX + GRID_ROOM_WIDTH) {
        nextRoomX++;
    } else if (charPos[1] < currentRoom->startY) {
        nextRoomY--;
    } else if (charPos[1] >= currentRoom->startY + GRID_ROOM_HEIGHT) {
        nextRoomY++;
    }

    changeRoom(map, nextRoomX, nextRoomY);
}
