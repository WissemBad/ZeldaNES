#include "iomanager.h"

static InputAction keyToAction(SDL_Keycode key) {
    switch (key) {
        case SDLK_UP:
        case SDLK_w:        return INPUT_ACTION_MOVE_UP;
        case SDLK_DOWN:
        case SDLK_s:        return INPUT_ACTION_MOVE_DOWN;
        case SDLK_LEFT:
        case SDLK_a:        return INPUT_ACTION_MOVE_LEFT;
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

void inputPollContinuous(InputState* state, bool* quit, bool* pause) {
    if (!state || !quit || !pause) return;

    state->moveUp = false;
    state->moveDown = false;
    state->moveLeft = false;
    state->moveRight = false;
    state->attack = false;
    state->interact = false;
    *quit = false;
    *pause = false;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *quit = true;
            return;
        }

        if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
            if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_p) {
                *pause = true;
            }
        }
    }

    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

    state->moveUp = keyboardState[SDL_SCANCODE_UP] || keyboardState[SDL_SCANCODE_W];
    state->moveDown = keyboardState[SDL_SCANCODE_DOWN] || keyboardState[SDL_SCANCODE_S];
    state->moveLeft = keyboardState[SDL_SCANCODE_LEFT] || keyboardState[SDL_SCANCODE_A];
    state->moveRight = keyboardState[SDL_SCANCODE_RIGHT] || keyboardState[SDL_SCANCODE_D];
    state->attack = keyboardState[SDL_SCANCODE_F];
    state->interact = keyboardState[SDL_SCANCODE_E];
}

