#include "utils.h"

void worldToScreen(const int worldPos[2], int screenPos[2], const int roomPos[2]) {
    screenPos[0] = (worldPos[0] - roomPos[0] * GRID_ROOM_WIDTH) * GRID_CELL_SIZE;
    screenPos[1] = (worldPos[1] - roomPos[1] * GRID_ROOM_HEIGHT) * GRID_CELL_SIZE;
}

void screenToWorld(const int screenPos[2], int worldPos[2], const int roomPos[2]) {
    worldPos[0] = screenPos[0] / GRID_CELL_SIZE + roomPos[0] * GRID_ROOM_WIDTH;
    worldPos[1] = screenPos[1] / GRID_CELL_SIZE + roomPos[1] * GRID_ROOM_HEIGHT;
}

int manhattanDistance(const int pos1[2], const int pos2[2]) {
    int dx = pos1[0] > pos2[0] ? pos1[0] - pos2[0] : pos2[0] - pos1[0];
    int dy = pos1[1] > pos2[1] ? pos1[1] - pos2[1] : pos2[1] - pos1[1];
    return dx + dy;
}

bool positionsEqual(const int pos1[2], const int pos2[2]) {
    return pos1[0] == pos2[0] && pos1[1] == pos2[1];
}

void copyPosition(const int src[2], int dest[2]) {
    dest[0] = src[0];
    dest[1] = src[1];
}
