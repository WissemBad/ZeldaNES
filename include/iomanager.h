/**
 * @file iomanager.h
 * @brief Gestion des entrées utilisateur (clavier)
 */

#ifndef NUPRC_IOMANAGER_H
#define NUPRC_IOMANAGER_H

#include "core.h"

//==============================================================================
// ÉNUMÉRATIONS
//==============================================================================

/** Actions possibles du joueur */
typedef enum {
    INPUT_ACTION_NONE = 0,
    INPUT_ACTION_QUIT,
    INPUT_ACTION_PAUSE_TOGGLE,
    INPUT_ACTION_CONFIRM,
    INPUT_ACTION_CANCEL,
    INPUT_ACTION_MOVE_UP,
    INPUT_ACTION_MOVE_DOWN,
    INPUT_ACTION_MOVE_LEFT,
    INPUT_ACTION_MOVE_RIGHT,
    INPUT_ACTION_INTERACT,
    INPUT_ACTION_ATTACK,
    INPUT_ACTION_INVENTORY_TOGGLE
} InputAction;

//==============================================================================
// FONCTIONS
//==============================================================================

/** Lit et traite les événements d'entrée SDL, retourne l'action correspondante */
InputAction inputPoll(void);

#endif // NUPRC_IOMANAGER_H
