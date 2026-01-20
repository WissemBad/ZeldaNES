//
// Created by Wissem.
// Game Settings
//

#ifndef SETTINGS_H
#define SETTINGS_H

#define SETTINGS_TRAY_WIDTH 10
#define SETTINGS_TRAY_HEIGHT 10

#define SETTINGS_CHAR_PLAYER 'X'
#define SETTINGS_CHAR_EMPTY ' '
#define SETTINGS_CHAR_END 'A'
#define SETTINGS_CHAR_VMOVE '!'
#define SETTINGS_CHAR_HMOVE '-'

#define SETTINGS_MAX_MOVE 20
#define SETTINGS_MAX_LEVEL 10

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

#endif