/**
 * @file iomanager.c
 * @brief Implémentation de la gestion des entrées utilisateur
 */

#include "iomanager.h"

//==============================================================================
// FONCTIONS PRIVÉES
//==============================================================================

/**
 * @brief Convertit une touche SDL en action de jeu
 */
static InputAction keyToAction(SDL_Keycode key) {
    switch (key) {
        // Déplacement (flèches et ZQSD)
        case SDLK_UP:
        case SDLK_z:    return INPUT_ACTION_MOVE_UP;
        case SDLK_DOWN:
        case SDLK_s:    return INPUT_ACTION_MOVE_DOWN;
        case SDLK_LEFT:
        case SDLK_q:    return INPUT_ACTION_MOVE_LEFT;
        case SDLK_RIGHT:
        case SDLK_d:    return INPUT_ACTION_MOVE_RIGHT;

        // Actions méta
        case SDLK_ESCAPE:   return INPUT_ACTION_QUIT;
        case SDLK_p:        return INPUT_ACTION_PAUSE_TOGGLE;

        // Interface utilisateur
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
        case SDLK_SPACE:    return INPUT_ACTION_CONFIRM;
        case SDLK_BACKSPACE:return INPUT_ACTION_CANCEL;

        // Actions de gameplay
        case SDLK_e:        return INPUT_ACTION_INTERACT;
        case SDLK_f:        return INPUT_ACTION_ATTACK;
        case SDLK_i:
        case SDLK_TAB:      return INPUT_ACTION_INVENTORY_TOGGLE;

        default:            return INPUT_ACTION_NONE;
    }
}

//==============================================================================
// FONCTIONS PUBLIQUES
//==============================================================================

InputAction inputPoll(void) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return INPUT_ACTION_QUIT;
        }

        if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
            return keyToAction(event.key.keysym.sym);
        }
    }

    return INPUT_ACTION_NONE;
}

