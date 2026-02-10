#include "character.h"
#include "render.h"

static float absf(float value) {
    return value < 0.0f ? -value : value;
}

static int floorToInt(float value) {
    const int truncated = (int)value;
    return value < (float)truncated ? truncated - 1 : truncated;
}

static int roundToGrid(float value) {
    return (int)(value + (value >= 0.0f ? 0.5f : -0.5f));
}

static bool isBlockingAt(Map* map, float x, float y) {
    int pos[2] = {floorToInt(x), floorToInt(y)};
    return Map_isBlocking(map, pos);
}

static bool isRectBlocking(Map* map, float x, float y) {
    const float margin = 0.08f;
    const float left = x + margin;
    const float right = x + 1.0f - margin;
    const float top = y + margin;
    const float bottom = y + 1.0f - margin;
    return isBlockingAt(map, left, top) ||
           isBlockingAt(map, right, top) ||
           isBlockingAt(map, left, bottom) ||
           isBlockingAt(map, right, bottom);
}

static void moveOnAxis(Character* c, float delta, bool horizontal) {
    if (delta == 0.0f) return;

    const float maxStep = 0.02f;
    float remaining = delta;
    const int maxIterations = 128;

    for (int i = 0; i < maxIterations && absf(remaining) > 0.0001f; i++) {
        float step = remaining;
        if (step > maxStep) step = maxStep;
        if (step < -maxStep) step = -maxStep;

        const float nextX = horizontal ? c->posX + step : c->posX;
        const float nextY = horizontal ? c->posY : c->posY + step;

        if (isRectBlocking(c->map, nextX, nextY)) {
            break;
        }

        if (horizontal) {
            c->posX = nextX;
        } else {
            c->posY = nextY;
        }
        remaining -= step;
    }
}

void Character_init(Character* c, CharacterType type, int lives, Map* map) {
    if (!c) return;
    c->type = type;
    c->lives = lives;
    c->posX = 0.0f;
    c->posY = 0.0f;
    c->texture = NULL;
    c->map = map;
}

void Character_getGridPos(const Character* c, int gridPos[2]) {
    if (!c || !gridPos) return;
    gridPos[0] = roundToGrid(c->posX);
    gridPos[1] = roundToGrid(c->posY);
}

void Character_moveSmooth(Character* c, float deltaX, float deltaY) {
    if (!c || !c->map) return;

    moveOnAxis(c, deltaX, true);
    moveOnAxis(c, deltaY, false);
}

void Character_move(Character* c, const int delta[2]) {
    if (!c || !c->map) return;

    int gridPos[2];
    Character_getGridPos(c, gridPos);
    int newPos[2] = { gridPos[0] + delta[0], gridPos[1] + delta[1] };

    if (!Map_isBlocking(c->map, newPos)) {
        c->posX = (float)newPos[0];
        c->posY = (float)newPos[1];
    }
}

void Character_draw(const Character* c, SDL_Renderer* renderer) {
    if (!c || !c->texture || !renderer || !c->map) return;

    int screenPos[2];
    Camera_worldToScreenF(&c->map->camera, c->posX, c->posY, screenPos);
    renderTexture(c->texture, renderer, screenPos[0], screenPos[1],
                  GRID_CELL_SIZE, GRID_CELL_SIZE);
}

void Character_destroy(Character* c) {
    if (!c) return;
    if (c->texture) {
        SDL_DestroyTexture(c->texture);
        c->texture = NULL;
    }
}
