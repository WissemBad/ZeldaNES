/**
 * @file iomanager.c
 * @brief Gestion des entrées clavier
 */

#include "iomanager.h"

static InputAction keyToAction(SDL_Keycode key) {
    switch (key) {
        case SDLK_UP:
        case SDLK_z:        return INPUT_ACTION_MOVE_UP;
        case SDLK_DOWN:
        case SDLK_s:        return INPUT_ACTION_MOVE_DOWN;
        case SDLK_LEFT:
        case SDLK_q:        return INPUT_ACTION_MOVE_LEFT;
        case SDLK_RIGHT:
        case SDLK_d:        return INPUT_ACTION_MOVE_RIGHT;
        case SDLK_ESCAPE:
        case SDLK_p:        return INPUT_ACTION_PAUSE_TOGGLE;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
        case SDLK_SPACE:    return INPUT_ACTION_CONFIRM;
        case SDLK_BACKSPACE: return INPUT_ACTION_CANCEL;
        case SDLK_e:        return INPUT_ACTION_INTERACT;
        case SDLK_f:        return INPUT_ACTION_ATTACK;
        case SDLK_i:
        case SDLK_TAB:      return INPUT_ACTION_INVENTORY_TOGGLE;
        default:            return INPUT_ACTION_NONE;
    }
}

InputAction inputPoll(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return INPUT_ACTION_QUIT;
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
            return keyToAction(event.key.keysym.sym);
        }
    }

    return INPUT_ACTION_NONE;
}

