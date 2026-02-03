/**
 * @file main.c
 * @brief Point d'entrée du programme
 */

#include "game.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    Game game;
    Game_init(&game);
    Game_run(&game);
    Game_destroy(&game);

    return EXIT_SUCCESS;
}

