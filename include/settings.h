#ifndef SETTINGS_H
#define SETTINGS_H

#define ASSET_TEXTURE_PLAYER_DEFAULT "assets/images/perso.bmp"
#define ASSET_TEXTURE_END_DEFAULT "assets/images/arrivee.bmp"

#define GRID_CELL_SIZE 50
#define GRID_COLS 10
#define GRID_ROWS 10

#define WINDOW_TITLE  "My SDL2 VideoGame"
#define WINDOW_WIDTH  (GRID_CELL_SIZE * GRID_COLS)
#define WINDOW_HEIGHT (GRID_CELL_SIZE * GRID_ROWS)

#define WINDOW_FONT_PATH "assets/fonts/DejaVuSans-Bold.ttf"
#define WINDOW_FONT_SIZE 20

#define CELL_CHAR_PLAYER 'X'
#define CELL_CHAR_EMPTY  ' '
#define CELL_CHAR_END    'A'
#define CELL_CHAR_VMOVE  '!'
#define CELL_CHAR_HMOVE  '-'

#define GAME_MAX_MOVES  20
#define GAME_MAX_LEVELS 10

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

#endif
