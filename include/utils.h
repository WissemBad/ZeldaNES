#ifndef NUPRC_UTILS_H
#define NUPRC_UTILS_H

#include "core.h"

void worldToScreen(const int world[2], int screen[2], const int room[2]);
void screenToWorld(const int screen[2], int world[2], const int room[2]);
int manhattanDistance(const int a[2], const int b[2]);
bool positionsEqual(const int a[2], const int b[2]);
void copyPosition(const int src[2], int dest[2]);

#endif
