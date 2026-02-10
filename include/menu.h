#ifndef NUPRC_MENU_H
#define NUPRC_MENU_H

#include "core.h"

#define MENU_MAX_OPTIONS    6
#define MENU_BUTTON_WIDTH   200
#define MENU_BUTTON_HEIGHT  40
#define MENU_BUTTON_SPACING 15

#define MENU_BG_R           20
#define MENU_BG_G           20
#define MENU_BG_B           30
#define MENU_BG_A           255

#define MENU_HIGHLIGHT_R    80
#define MENU_HIGHLIGHT_G    120
#define MENU_HIGHLIGHT_B    200
#define MENU_HIGHLIGHT_A    255

#define MENU_TEXT_R         255
#define MENU_TEXT_G         255
#define MENU_TEXT_B         255
#define MENU_TEXT_A         255

typedef enum {
    MENU_TYPE_MAIN,
    MENU_TYPE_PAUSE,
    MENU_TYPE_GAMEOVER
} MenuType;

typedef enum {
    MENU_ACTION_NONE,
    MENU_ACTION_START,
    MENU_ACTION_RESUME,
    MENU_ACTION_RESTART,
    MENU_ACTION_QUIT
} MenuAction;

typedef struct {
    char        label[32];
    MenuAction  action;
    bool        enabled;
} MenuOption;

typedef struct {
    MenuType    type;
    MenuOption  options[MENU_MAX_OPTIONS];
    int         optionCount;
    int         selectedIndex;
    char        title[64];
    char        subtitle[128];
} Menu;

void Menu_initMain(Menu* menu);
void Menu_initPause(Menu* menu);
void Menu_initGameOver(Menu* menu, int finalScore);
MenuAction Menu_handleInput(Menu* menu);
void Menu_render(const Menu* menu, const RenderState* render);
void Menu_moveUp(Menu* menu);
void Menu_moveDown(Menu* menu);
MenuAction Menu_confirm(const Menu* menu);

#endif
