#include "map.h"
#include "render.h"

static float absf(const float value) {
    return value < 0.0f ? -value : value;
}

static int roundToInt(const float value) {
    return (int)(value + (value >= 0.0f ? 0.5f : -0.5f));
}

static float clampf(const float value, const float minValue, const float maxValue) {
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

static Tile createTile(const char blockingChar, const int textureId) {
    Tile tile;
    tile.isBlocking = (blockingChar == 'X');
    tile.textureId = (char)textureId;
    return tile;
}

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

static Room createRoom(const int roomX, const int roomY) {
    Room room;
    room.startX = roomX * GRID_ROOM_WIDTH;
    room.startY = roomY * GRID_ROOM_HEIGHT;
    room.position[0] = roomX;
    room.position[1] = roomY;
    return room;
}

static void drawGrid(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 80);

    int gameHeight = WINDOW_HEIGHT - WINDOW_TEXTAREA_HEIGHT;

    for (int row = 1; row < gameHeight / GRID_CELL_SIZE; row++) {
        SDL_RenderDrawLine(renderer,
            0, row * GRID_CELL_SIZE,
            WINDOW_WIDTH - 1, row * GRID_CELL_SIZE
        );
    }

    for (int col = 1; col < WINDOW_WIDTH / GRID_CELL_SIZE; col++) {
        SDL_RenderDrawLine(renderer,
            col * GRID_CELL_SIZE, 0,
            col * GRID_CELL_SIZE, gameHeight - 1
        );
    }
}

void Camera_init(Camera* cam) {
    cam->x = 0.0f;
    cam->y = 0.0f;
    cam->targetX = 0.0f;
    cam->targetY = 0.0f;
}

void Camera_followF(Camera* cam, float playerX, float playerY) {
    int gameHeight = WINDOW_HEIGHT - WINDOW_TEXTAREA_HEIGHT;

    cam->targetX = playerX * GRID_CELL_SIZE - WINDOW_WIDTH / 2.0f + GRID_CELL_SIZE / 2.0f;
    cam->targetY = playerY * GRID_CELL_SIZE - gameHeight / 2.0f + GRID_CELL_SIZE / 2.0f;

    const float maxX = GRID_WORLD_WIDTH * GRID_CELL_SIZE - WINDOW_WIDTH;
    const float maxY = GRID_WORLD_HEIGHT * GRID_CELL_SIZE - gameHeight;

    cam->targetX = clampf(cam->targetX, 0.0f, maxX);
    cam->targetY = clampf(cam->targetY, 0.0f, maxY);

    const float smoothFactor = 0.25f;
    cam->x += (cam->targetX - cam->x) * smoothFactor;
    cam->y += (cam->targetY - cam->y) * smoothFactor;

    if (absf(cam->targetX - cam->x) < 0.1f) {
        cam->x = cam->targetX;
    }
    if (absf(cam->targetY - cam->y) < 0.1f) {
        cam->y = cam->targetY;
    }
}

void Camera_follow(Camera* cam, const int playerPos[2]) {
    Camera_followF(cam, (float)playerPos[0], (float)playerPos[1]);
}

void Camera_worldToScreenF(const Camera* cam, float worldX, float worldY, int screenPos[2]) {
    screenPos[0] = roundToInt(worldX * GRID_CELL_SIZE - cam->x);
    screenPos[1] = roundToInt(worldY * GRID_CELL_SIZE - cam->y);
}

void Camera_worldToScreen(const Camera* cam, const int worldPos[2], int screenPos[2]) {
    Camera_worldToScreenF(cam, (float)worldPos[0], (float)worldPos[1], screenPos);
}

void Map_init(Map* map, SDL_Renderer* renderer) {
    map->renderer = renderer;

    loadMapData(map);

    map->textures = loadTileTextures(ASSET_MAP_TILES, renderer);
    if (map->textures == NULL) {
        fprintf(stderr, "Erreur: impossible de charger les textures de la carte\n");
        exit(EXIT_FAILURE);
    }

    const int maxRoomX = GRID_WORLD_WIDTH / GRID_ROOM_WIDTH;
    const int maxRoomY = GRID_WORLD_HEIGHT / GRID_ROOM_HEIGHT;

    for (int row = 0; row < maxRoomY; row++) {
        for (int col = 0; col < maxRoomX; col++) {
            map->rooms[row][col] = createRoom(col, row);
        }
    }

    map->currentRoom[0] = 0;
    map->currentRoom[1] = 0;

    Camera_init(&map->camera);
}

void Map_draw(const Map* map, bool showGrid) {
    int gameHeight = WINDOW_HEIGHT - WINDOW_TEXTAREA_HEIGHT;

    int startTileX = (int)(map->camera.x / GRID_CELL_SIZE);
    int startTileY = (int)(map->camera.y / GRID_CELL_SIZE);
    int endTileX = (int)((map->camera.x + WINDOW_WIDTH) / GRID_CELL_SIZE) + 2;
    int endTileY = (int)((map->camera.y + gameHeight) / GRID_CELL_SIZE) + 2;

    if (startTileX < 0) startTileX = 0;
    if (startTileY < 0) startTileY = 0;
    if (endTileX > GRID_WORLD_WIDTH) endTileX = GRID_WORLD_WIDTH;
    if (endTileY > GRID_WORLD_HEIGHT) endTileY = GRID_WORLD_HEIGHT;

    for (int row = startTileY; row < endTileY; row++) {
        for (int col = startTileX; col < endTileX; col++) {
            const Tile tile = map->world[row][col];
            const int tileIndex = (int)tile.textureId;

            int screenX = roundToInt(col * GRID_CELL_SIZE - map->camera.x);
            int screenY = roundToInt(row * GRID_CELL_SIZE - map->camera.y);

            if (tileIndex >= 0 && tileIndex < MAP_TILES_COUNT && map->textures[tileIndex] != NULL) {
                renderTexture(
                    map->textures[tileIndex], map->renderer,
                    screenX, screenY,
                    GRID_CELL_SIZE, GRID_CELL_SIZE
                );
            } else {

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

    int roomX = charPos[0] / GRID_ROOM_WIDTH;
    int roomY = charPos[1] / GRID_ROOM_HEIGHT;

    const int maxRoomX = GRID_WORLD_WIDTH / GRID_ROOM_WIDTH;
    const int maxRoomY = GRID_WORLD_HEIGHT / GRID_ROOM_HEIGHT;

    if (roomX >= 0 && roomX < maxRoomX && roomY >= 0 && roomY < maxRoomY) {
        map->currentRoom[0] = roomX;
        map->currentRoom[1] = roomY;
    }
}
