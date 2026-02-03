/**
 * @file game.c
 * @brief Implémentation du cycle de vie et de la boucle principale du jeu
 */

#include "game.h"
#include "render.h"
#include "iomanager.h"
#include "hud.h"

//==============================================================================
// FONCTIONS PRIVÉES
//==============================================================================

/**
 * @brief Initialise les ressources du gameplay (carte, joueur)
 */
static void initGameplayResources(Game* game) {
    // Initialisation de la carte
    Map_init(&game->map, game->render.renderer);

    // Configuration de la salle initiale
    const int initialRoom[2] = GAME_INITIAL_ROOM;
    game->map.currentRoom[0] = initialRoom[0];
    game->map.currentRoom[1] = initialRoom[1];

    // Initialisation du joueur
    Link_init(&game->player, &game->map);
    Room_getCenter(&game->map.rooms[initialRoom[1]][initialRoom[0]], game->player.base.pos);
}

/**
 * @brief Réinitialise les statistiques du joueur
 */
static void resetPlayerStats(Game* game) {
    game->stats.score = GAME_INITIAL_SCORE;
    game->stats.kills = 0;
    game->stats.playtime = 0;
}

/**
 * @brief Gère les inputs pendant l'état PLAYING
 */
static void handlePlayingInput(Game* game, InputAction input) {
    switch (input) {
        case INPUT_ACTION_QUIT:
            game->running = false;
            break;

        case INPUT_ACTION_PAUSE_TOGGLE:
            Game_pause(game);
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

/**
 * @brief Gère les actions retournées par les menus
 */
static void handleMenuAction(Game* game, MenuAction action) {
    switch (action) {
        case MENU_ACTION_START:
        case MENU_ACTION_RESTART:
            Game_startNewGame(game);
            break;

        case MENU_ACTION_RESUME:
            Game_resume(game);
            break;

        case MENU_ACTION_QUIT:
            if (game->state == MENU_TYPE_MAIN) {
                game->running = false;
            } else {
                Game_setState(game, STATE_MENU);
            }
            break;

        default:
            break;
    }
}

//==============================================================================
// FONCTIONS PUBLIQUES - CYCLE DE VIE
//==============================================================================

void Game_init(Game* game) {
    // --- État initial ---
    game->state = STATE_MENU;
    game->previousState = STATE_MENU;
    game->enemyCount = 0;
    game->running = true;
    resetPlayerStats(game);

    // --- Initialisation SDL ---
    initSDL();
    game->render.window = createWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
    game->render.renderer = createRenderer(game->render.window);
    game->render.font = TTF_OpenFont(WINDOW_FONT_PATH, WINDOW_FONT_SIZE);

    if (game->render.font == NULL) {
        fprintf(stderr, "Erreur chargement police : %s\n", TTF_GetError());
    }

    // --- Initialisation du menu principal ---
    Menu_initMain(&game->menu);
}

void Game_run(Game* game) {
    while (game->running) {
        Game_handleInput(game);
        Game_update(game);
        Game_render(game);

        // Petit délai pour limiter le CPU (environ 60 FPS)
        SDL_Delay(16);
    }
}

void Game_destroy(Game* game) {
    // --- Libération du joueur ---
    if (game->state != STATE_MENU) {
        Link_destroy(&game->player);
    }

    // --- Libération des ennemis ---
    for (int i = 0; i < game->enemyCount; i++) {
        Enemy_destroy(&game->enemies[i]);
    }

    // --- Libération de la carte ---
    if (game->state != STATE_MENU) {
        Map_destroy(&game->map);
    }

    // --- Libération des ressources SDL ---
    if (game->render.font != NULL) {
        TTF_CloseFont(game->render.font);
    }
    quitSDL(game->render.window, game->render.renderer);
}

//==============================================================================
// FONCTIONS PUBLIQUES - GESTION DES ÉTATS
//==============================================================================

void Game_setState(Game* game, GameState newState) {
    game->previousState = game->state;
    game->state = newState;

    // Initialiser le menu correspondant
    switch (newState) {
        case STATE_MENU:
            Menu_initMain(&game->menu);
            break;

        case STATE_PAUSED:
            Menu_initPause(&game->menu);
            break;

        case STATE_GAMEOVER:
            Menu_initGameOver(&game->menu, game->stats.score);
            break;

        case STATE_PLAYING:
            // Pas de menu à initialiser
            break;
    }
}

void Game_startNewGame(Game* game) {
    // Nettoyer l'ancien état si nécessaire
    if (game->previousState == STATE_PLAYING || game->previousState == STATE_PAUSED) {
        Link_destroy(&game->player);
        Map_destroy(&game->map);
        for (int i = 0; i < game->enemyCount; i++) {
            Enemy_destroy(&game->enemies[i]);
        }
    }

    // Réinitialiser
    resetPlayerStats(game);
    game->enemyCount = 0;
    initGameplayResources(game);

    // Passer en mode jeu
    game->state = STATE_PLAYING;
    game->previousState = STATE_PLAYING;
}

void Game_pause(Game* game) {
    if (game->state == STATE_PLAYING) {
        Game_setState(game, STATE_PAUSED);
    }
}

void Game_resume(Game* game) {
    if (game->state == STATE_PAUSED) {
        game->state = STATE_PLAYING;
    }
}

//==============================================================================
// FONCTIONS PUBLIQUES - BOUCLE DE JEU
//==============================================================================

void Game_handleInput(Game* game) {
    switch (game->state) {
        case STATE_MENU:
        case STATE_PAUSED:
        case STATE_GAMEOVER: {
            MenuAction action = Menu_handleInput(&game->menu);
            handleMenuAction(game, action);
            break;
        }

        case STATE_PLAYING: {
            InputAction input = inputPoll();
            handlePlayingInput(game, input);
            break;
        }
    }
}

void Game_update(Game* game) {
    // Ne mettre à jour que si on est en mode jeu
    if (game->state != STATE_PLAYING) {
        return;
    }

    // Mise à jour du joueur
    Link_update(&game->player);

    // Gestion des transitions de salles
    Room_handleTransition(&game->map, game->player.base.pos);

    // Mise à jour des ennemis
    for (int i = 0; i < game->enemyCount; i++) {
        Enemy_update(&game->enemies[i]);
    }

    // Temps de jeu (en frames)
    game->stats.playtime++;

    // Vérification game over (exemple: plus de vies)
    if (game->player.base.lives <= 0) {
        Game_setState(game, STATE_GAMEOVER);
    }
}

void Game_render(Game* game) {
    switch (game->state) {
        case STATE_MENU:
        case STATE_GAMEOVER:
            Menu_render(&game->menu, &game->render);
            break;

        case STATE_PAUSED:
            // Afficher le jeu en arrière-plan (assombri)
            SDL_SetRenderDrawColor(game->render.renderer, 0, 0, 0, 255);
            clearRenderer(game->render.renderer);
            Map_draw(&game->map, false);
            Character_draw(&game->player.base, game->render.renderer);

            // Overlay semi-transparent
            SDL_SetRenderDrawBlendMode(game->render.renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(game->render.renderer, 0, 0, 0, 180);
            SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            SDL_RenderFillRect(game->render.renderer, &overlay);

            // Menu pause par-dessus
            Menu_render(&game->menu, &game->render);
            break;

        case STATE_PLAYING:
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
            break;
    }
}
