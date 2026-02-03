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
 * Affiche la grille (pour debug)
 */
static void drawGrid(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 80);

    int gameHeight = WINDOW_HEIGHT - WINDOW_TEXTAREA_HEIGHT;

    // Lignes horizontales
    for (int row = 1; row < gameHeight / GRID_CELL_SIZE; row++) {
        SDL_RenderDrawLine(renderer,
            0, row * GRID_CELL_SIZE,
            WINDOW_WIDTH - 1, row * GRID_CELL_SIZE
        );
    }

    // Lignes verticales
    for (int col = 1; col < WINDOW_WIDTH / GRID_CELL_SIZE; col++) {
        SDL_RenderDrawLine(renderer,
            col * GRID_CELL_SIZE, 0,
            col * GRID_CELL_SIZE, gameHeight - 1
        );
    }
}

//==============================================================================
// FONCTIONS PUBLIQUES - CAMÉRA
//==============================================================================

void Camera_init(Camera* cam) {
    cam->x = 0;
    cam->y = 0;
    cam->targetX = 0;
    cam->targetY = 0;
}

void Camera_follow(Camera* cam, const int playerPos[2]) {
    // Calculer la position cible : centrer le joueur à l'écran
    int gameHeight = WINDOW_HEIGHT - WINDOW_TEXTAREA_HEIGHT;

    cam->targetX = playerPos[0] * GRID_CELL_SIZE - WINDOW_WIDTH / 2 + GRID_CELL_SIZE / 2;
    cam->targetY = playerPos[1] * GRID_CELL_SIZE - gameHeight / 2 + GRID_CELL_SIZE / 2;

    // Limites de la carte (ne pas montrer en dehors)
    int maxX = GRID_WORLD_WIDTH * GRID_CELL_SIZE - WINDOW_WIDTH;
    int maxY = GRID_WORLD_HEIGHT * GRID_CELL_SIZE - gameHeight;

    if (cam->targetX < 0) cam->targetX = 0;
    if (cam->targetY < 0) cam->targetY = 0;
    if (cam->targetX > maxX) cam->targetX = maxX;
    if (cam->targetY > maxY) cam->targetY = maxY;

    // Lissage de la caméra (interpolation linéaire)
    int dx = cam->targetX - cam->x;
    int dy = cam->targetY - cam->y;

    // Seuil de lissage (ajuster pour plus ou moins de fluidité)
    int smoothFactor = 4;

    if (dx != 0) {
        cam->x += dx / smoothFactor;
        if (dx > 0 && cam->x > cam->targetX) cam->x = cam->targetX;
        if (dx < 0 && cam->x < cam->targetX) cam->x = cam->targetX;
    }
    if (dy != 0) {
        cam->y += dy / smoothFactor;
        if (dy > 0 && cam->y > cam->targetY) cam->y = cam->targetY;
        if (dy < 0 && cam->y < cam->targetY) cam->y = cam->targetY;
    }
}

void Camera_worldToScreen(const Camera* cam, const int worldPos[2], int screenPos[2]) {
    screenPos[0] = worldPos[0] * GRID_CELL_SIZE - cam->x;
    screenPos[1] = worldPos[1] * GRID_CELL_SIZE - cam->y;
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

    // Initialiser la caméra
    Camera_init(&map->camera);
}

void Map_draw(const Map* map, bool showGrid) {
    // Calculer les tuiles visibles basées sur la position de la caméra
    int gameHeight = WINDOW_HEIGHT - WINDOW_TEXTAREA_HEIGHT;

    int startTileX = map->camera.x / GRID_CELL_SIZE;
    int startTileY = map->camera.y / GRID_CELL_SIZE;
    int endTileX = (map->camera.x + WINDOW_WIDTH) / GRID_CELL_SIZE + 1;
    int endTileY = (map->camera.y + gameHeight) / GRID_CELL_SIZE + 1;

    // Clamper aux limites du monde
    if (startTileX < 0) startTileX = 0;
    if (startTileY < 0) startTileY = 0;
    if (endTileX > GRID_WORLD_WIDTH) endTileX = GRID_WORLD_WIDTH;
    if (endTileY > GRID_WORLD_HEIGHT) endTileY = GRID_WORLD_HEIGHT;

    // Dessiner uniquement les tuiles visibles
    for (int row = startTileY; row < endTileY; row++) {
        for (int col = startTileX; col < endTileX; col++) {
            const Tile tile = map->world[row][col];
            const int tileIndex = (int)tile.textureId;

            // Convertir position monde -> écran
            int screenX = col * GRID_CELL_SIZE - map->camera.x;
            int screenY = row * GRID_CELL_SIZE - map->camera.y;

            // Vérifier que l'index est valide et que la texture existe
            if (tileIndex >= 0 && tileIndex < MAP_TILES_COUNT && map->textures[tileIndex] != NULL) {
                renderTexture(
                    map->textures[tileIndex], map->renderer,
                    screenX, screenY,
                    GRID_CELL_SIZE, GRID_CELL_SIZE
                );
            } else {
                // Texture manquante - afficher un carré magenta (debug)
                SDL_SetRenderDrawColor(map->renderer, 255, 0, 255, 255);
                SDL_Rect rect = {screenX, screenY, GRID_CELL_SIZE, GRID_CELL_SIZE};
                SDL_RenderFillRect(map->renderer, &rect);
            }
        }
    }

    if (showGrid) {
        drawGrid(map->renderer);
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
// FONCTIONS PUBLIQUES - GESTION DES SALLES (conservées pour compatibilité)
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
    // En mode open world, on met juste à jour currentRoom pour référence
    int roomX = charPos[0] / GRID_ROOM_WIDTH;
    int roomY = charPos[1] / GRID_ROOM_HEIGHT;

    const int maxRoomX = GRID_WORLD_WIDTH / GRID_ROOM_WIDTH;
    const int maxRoomY = GRID_WORLD_HEIGHT / GRID_ROOM_HEIGHT;

    if (roomX >= 0 && roomX < maxRoomX && roomY >= 0 && roomY < maxRoomY) {
        map->currentRoom[0] = roomX;
        map->currentRoom[1] = roomY;
    }
}
