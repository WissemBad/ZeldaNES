#include "settings.h"
#include "IOManager.h"
#include "game.h"

int main(void) {
    IO_initSDL();
    IO_initTTF();

    SDL_Window *window = IO_createWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_Renderer *renderer = IO_createRenderer(window);
    TTF_Font *font = IO_createFont(WINDOW_FONT_PATH, WINDOW_FONT_SIZE);

    Game game;
    Game_init(&game, renderer, "Wissem.");

    while (game.isRunning) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        IO_clearRenderer(renderer);

        Game_draw(&game, renderer);
        IO_updateDisplay(renderer);

        Direction direction = IO_inputDirection();
        if (direction == -2) break;

        Game_update(&game, direction);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    IO_clearRenderer(renderer);

    Game_end(&game, renderer, font);
    Game_destroy(&game);

    TTF_CloseFont(font);
    TTF_Quit();
    IO_quit(window, renderer);

    return 0;
}
