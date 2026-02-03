/**
 * @file game.c
 * @brief Implémentation du cycle de vie et de la boucle principale du jeu
 */

#include "game.h"
#include "render.h"
#include "iomanager.h"
#include "hud.h"

//==============================================================================
// FONCTIONS PUBLIQUES - CYCLE DE VIE
//==============================================================================

void Game_init(Game* game) {
    // --- État initial ---
    game->state = STATE_PLAYING;
    game->stats.score = GAME_INITIAL_SCORE;
    game->stats.kills = 0;
    game->stats.playtime = 0;
    game->enemyCount = 0;
    game->running = true;

    // --- Initialisation SDL ---
    initSDL();
    game->render.window = createWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
    game->render.renderer = createRenderer(game->render.window);
    game->render.font = TTF_OpenFont(WINDOW_FONT_PATH, WINDOW_FONT_SIZE);

    if (game->render.font == NULL) {
        fprintf(stderr, "Erreur chargement police : %s\n", TTF_GetError());
    }

    // --- Initialisation de la carte ---
    Map_init(&game->map, game->render.renderer);

    // --- Configuration de la salle initiale ---
    const int initialRoom[2] = GAME_INITIAL_ROOM;
    game->map.currentRoom[0] = initialRoom[0];
    game->map.currentRoom[1] = initialRoom[1];

    // --- Initialisation du joueur ---
    Link_init(&game->player, &game->map);
    Room_getCenter(&game->map.rooms[initialRoom[1]][initialRoom[0]], game->player.base.pos);
}

void Game_run(Game* game) {
    while (game->running) {
        Game_handleInput(game);
        Game_update(game);
        Game_render(game);
    }
}

void Game_destroy(Game* game) {
    // --- Libération du joueur ---
    Link_destroy(&game->player);

    // --- Libération des ennemis ---
    for (int i = 0; i < game->enemyCount; i++) {
        Enemy_destroy(&game->enemies[i]);
    }

    // --- Libération de la carte ---
    Map_destroy(&game->map);

    // --- Libération des ressources SDL ---
    if (game->render.font != NULL) {
        TTF_CloseFont(game->render.font);
    }
    quitSDL(game->render.window, game->render.renderer);
}

//==============================================================================
// FONCTIONS PUBLIQUES - BOUCLE DE JEU
//==============================================================================

void Game_handleInput(Game* game) {
    const InputAction input = inputPoll();

    switch (input) {
        case INPUT_ACTION_QUIT:
            game->running = false;
            break;

        case INPUT_ACTION_MOVE_UP: {
            const int delta[2] = {0, -1};
            Character_move(&game->player.base, delta);
            game->player.direction = LINK_DIR_UP;
            break;
        }

        case INPUT_ACTION_MOVE_DOWN: {
            const int delta[2] = {0, 1};
            Character_move(&game->player.base, delta);
            game->player.direction = LINK_DIR_DOWN;
            break;
        }

        case INPUT_ACTION_MOVE_LEFT: {
            const int delta[2] = {-1, 0};
            Character_move(&game->player.base, delta);
            game->player.direction = LINK_DIR_LEFT;
            break;
        }

        case INPUT_ACTION_MOVE_RIGHT: {
            const int delta[2] = {1, 0};
            Character_move(&game->player.base, delta);
            game->player.direction = LINK_DIR_RIGHT;
            break;
        }

        case INPUT_ACTION_ATTACK:
            Link_attack(&game->player);
            break;

        default:
            break;
    }
}

void Game_update(Game* game) {
    // Mise à jour du joueur
    Link_update(&game->player);

    // Gestion des transitions de salles
    Room_handleTransition(&game->map, game->player.base.pos);

    // Mise à jour des ennemis
    for (int i = 0; i < game->enemyCount; i++) {
        Enemy_update(&game->enemies[i]);
    }

    // Temps de jeu
    game->stats.playtime++;
}

void Game_render(Game* game) {
    // --- Effacement de l'écran ---
    SDL_SetRenderDrawColor(game->render.renderer, 0, 0, 0, 255);
    clearRenderer(game->render.renderer);

    // --- Carte ---
    Map_draw(&game->map, true);

    // --- Ennemis ---
    for (int i = 0; i < game->enemyCount; i++) {
        Character_draw(&game->enemies[i].base, game->render.renderer);
    }

    // --- Joueur ---
    Character_draw(&game->player.base, game->render.renderer);

    // --- HUD ---
    HUD_render(&game->render, &game->stats, game->player.base.lives, game->map.currentRoom);

    // --- Affichage ---
    updateDisplay(game->render.renderer);
}
