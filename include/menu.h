/**
 * @file menu.h
 * @brief Gestion des menus du jeu (principal, pause, game over)
 */

#ifndef NUPRC_MENU_H
#define NUPRC_MENU_H

#include "core.h"

//==============================================================================
// CONSTANTES
//==============================================================================

/** Nombre maximum d'options dans un menu */
#define MENU_MAX_OPTIONS    6

/** Dimensions des boutons de menu */
#define MENU_BUTTON_WIDTH   200
#define MENU_BUTTON_HEIGHT  40
#define MENU_BUTTON_SPACING 15

/** Couleurs du menu */
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

//==============================================================================
// ÉNUMÉRATIONS
//==============================================================================

/** Type de menu affiché */
typedef enum {
    MENU_TYPE_MAIN,         /**< Menu principal (démarrage) */
    MENU_TYPE_PAUSE,        /**< Menu pause (en jeu) */
    MENU_TYPE_GAMEOVER      /**< Menu game over */
} MenuType;

/** Actions possibles retournées par le menu */
typedef enum {
    MENU_ACTION_NONE,       /**< Aucune action */
    MENU_ACTION_START,      /**< Démarrer une nouvelle partie */
    MENU_ACTION_RESUME,     /**< Reprendre la partie */
    MENU_ACTION_RESTART,    /**< Recommencer la partie */
    MENU_ACTION_QUIT        /**< Quitter le jeu */
} MenuAction;

//==============================================================================
// STRUCTURES
//==============================================================================

/** Option de menu */
typedef struct {
    char        label[32];      /**< Texte de l'option */
    MenuAction  action;         /**< Action associée */
    bool        enabled;        /**< Option activée ou grisée */
} MenuOption;

/** État d'un menu */
typedef struct {
    MenuType    type;                           /**< Type de menu */
    MenuOption  options[MENU_MAX_OPTIONS];      /**< Liste des options */
    int         optionCount;                    /**< Nombre d'options */
    int         selectedIndex;                  /**< Index de l'option sélectionnée */
    char        title[64];                      /**< Titre du menu */
    char        subtitle[128];                  /**< Sous-titre (score, message...) */
} Menu;

//==============================================================================
// FONCTIONS
//==============================================================================

/** Initialise le menu principal */
void Menu_initMain(Menu* menu);

/** Initialise le menu pause */
void Menu_initPause(Menu* menu);

/** Initialise le menu game over avec le score final */
void Menu_initGameOver(Menu* menu, int finalScore);

/** Gère les entrées utilisateur sur le menu, retourne l'action choisie */
MenuAction Menu_handleInput(Menu* menu);

/** Affiche le menu à l'écran */
void Menu_render(const Menu* menu, const RenderState* render);

/** Déplace la sélection vers le haut */
void Menu_moveUp(Menu* menu);

/** Déplace la sélection vers le bas */
void Menu_moveDown(Menu* menu);

/** Confirme la sélection actuelle et retourne l'action */
MenuAction Menu_confirm(const Menu* menu);

#endif // NUPRC_MENU_H
