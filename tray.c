//
// Created by Wissem.
// Tray Management
//

#include "tray.h"
#include <stdio.h>

void initTray(char tray[SETTINGS_TRAY_HEIGHT][SETTINGS_TRAY_WIDTH], Character c, int endPos[2]) {
    for (int i = 0; i < SETTINGS_TRAY_HEIGHT; i++) {
        for (int j = 0; j < SETTINGS_TRAY_WIDTH; j++) {
            tray[i][j] = SETTINGS_CHAR_EMPTY;
        }
    }

    tray[c.pos[1]][c.pos[0]] = SETTINGS_CHAR_PLAYER;
    tray[endPos[1]][endPos[0]] = SETTINGS_CHAR_END;
}

void showTray(char tray[SETTINGS_TRAY_HEIGHT][SETTINGS_TRAY_WIDTH]) {
    printf("-- Tableau de jeu --\n");
    for (int i = 0; i < SETTINGS_TRAY_HEIGHT; i++) {
        for (int j = 0; j < SETTINGS_TRAY_WIDTH; j++) {
            printf("%c", tray[i][j]);
        }
        putchar('\n');
    }
    printf("--------------------\n");
}

void updateTray(char tray[SETTINGS_TRAY_HEIGHT][SETTINGS_TRAY_WIDTH], Character c, Direction dir) {
    switch (dir) {
        case DIR_UP:
            tray[c.pos[1]][c.pos[0]] = SETTINGS_CHAR_PLAYER;
            tray[c.pos[1]+1][c.pos[0]] = SETTINGS_CHAR_VMOVE;
            break;
        case DIR_DOWN:
            tray[c.pos[1]][c.pos[0]] = SETTINGS_CHAR_PLAYER;
            tray[c.pos[1]-1][c.pos[0]] = SETTINGS_CHAR_VMOVE;
            break;
        case DIR_LEFT:
            tray[c.pos[1]][c.pos[0]] = SETTINGS_CHAR_PLAYER;
            tray[c.pos[1]][c.pos[0]+1] = SETTINGS_CHAR_HMOVE;
            break;
        case DIR_RIGHT:
            tray[c.pos[1]][c.pos[0]] = SETTINGS_CHAR_PLAYER;
            tray[c.pos[1]][c.pos[0]-1] = SETTINGS_CHAR_HMOVE;
            break;
        default:
            break;
    }
}