/**
 * @file game.c
 * @brief Implémentation du cycle de vie et de la boucle principale du jeu
 */

#include "game.h"
#include "render.h"
#include "iomanager.h"
#include "hud.h"

#include <stdlib.h>
#include <time.h>

//==============================================================================
// CONSTANTES PRIVÉES
//==============================================================================

/** Nombre d'ennemis à spawner par salle */
#define ENEMIES_PER_ROOM    3

/** Distance minimale de spawn entre ennemi et joueur */
#define ENEMY_SPAWN_MIN_DISTANCE    4

//==============================================================================
// FONCTIONS PRIVÉES
//==============================================================================

/**
 * @brief Génère une position aléatoire valide dans la salle actuelle
 */
static void getRandomPositionInRoom(const Map* map, const int playerPos[2], int outPos[2]) {
    const Room* room = &map->rooms[map->currentRoom[1]][map->currentRoom[0]];
    int attempts = 0;
    const int maxAttempts = 50;

    do {
        outPos[0] = room->startX + 1 + (rand() % (GRID_ROOM_WIDTH - 2));
        outPos[1] = room->startY + 1 + (rand() % (GRID_ROOM_HEIGHT - 2));
        attempts++;

        // Vérifie que la position n'est pas bloquante et assez loin du joueur
        int dx = outPos[0] - playerPos[0];
        int dy = outPos[1] - playerPos[1];
        int distance = (dx > 0 ? dx : -dx) + (dy > 0 ? dy : -dy);

        if (!Map_isBlocking(map, outPos) && distance >= ENEMY_SPAWN_MIN_DISTANCE) {
            return;
        }
    } while (attempts < maxAttempts);
}

/**
 * @brief Spawn les ennemis pour la salle actuelle
 */
static void spawnEnemiesForRoom(Game* game) {
    // Nettoyer les anciens ennemis
    for (int i = 0; i < game->enemyCount; i++) {
        Enemy_destroy(&game->enemies[i]);
    }
    game->enemyCount = 0;

    // Spawner de nouveaux ennemis
    for (int i = 0; i < ENEMIES_PER_ROOM && game->enemyCount < GAME_MAX_ENEMIES; i++) {
        int spawnPos[2];
        getRandomPositionInRoom(&game->map, game->player.base.pos, spawnPos);

        // Alterner entre les types d'ennemis et d'IA
        EnemyType type;
        EnemyAI ai;

        switch (i % 3) {
            case 0:
                type = ENEMY_TYPE_BASIC;
                ai = ENEMY_AI_RANDOM;
                break;
            case 1:
                type = ENEMY_TYPE_FAST;
                ai = ENEMY_AI_CHASE;
                break;
            case 2:
                type = ENEMY_TYPE_TANK;
                ai = ENEMY_AI_CHASE;
                break;
            default:
                type = ENEMY_TYPE_BASIC;
                ai = ENEMY_AI_RANDOM;
                break;
        }

        Enemy_init(&game->enemies[game->enemyCount], type, ai, &game->map, spawnPos);
        game->enemyCount++;
    }
}

/**
 * @brief Initialise les ressources du gameplay (carte, joueur, ennemis)
 */
static void initGameplayResources(Game* game) {
    // Seed pour le random
    srand((int)time(NULL));

    // Initialisation de la carte
    Map_init(&game->map, game->render.renderer);

    // Configuration de la salle initiale
    const int initialRoom[2] = GAME_INITIAL_ROOM;
    game->map.currentRoom[0] = initialRoom[0];
    game->map.currentRoom[1] = initialRoom[1];

    // Initialisation du joueur
    Link_init(&game->player, &game->map);
    Room_getCenter(&game->map.rooms[initialRoom[1]][initialRoom[0]], game->player.base.pos);

    // Spawn des ennemis
    spawnEnemiesForRoom(game);
}

/**
 * @brief Réinitialise les statistiques du joueur
 */
static void resetPlayerStats(Game* game) {
    game->stats.score = GAME_INITIAL_SCORE;
    game->stats.kills = 0;
    game->stats.playtime = 0;
    game->stats.moves = 0;
}

/**
 * @brief Vérifie et gère les collisions entre Link et les ennemis
 */
static void checkEnemyCollisions(Game* game) {
    for (int i = 0; i < game->enemyCount; i++) {
        if (!game->enemies[i].isActive) continue;

        if (Enemy_collidesWith(&game->enemies[i], game->player.base.pos)) {
            Link_takeDamage(&game->player, 1);
        }
    }
}

/**
 * @brief Gère l'attaque de Link et les dégâts aux ennemis
 * L'attaque touche pendant toute sa durée active et a une zone d'effet en arc
 */
static void handleAttack(Game* game) {
    // L'attaque ne touche que pendant la phase active (début de l'attaque)
    if (!Link_isAttacking(&game->player)) return;
    if (game->player.attackCooldown < LINK_ATTACK_COOLDOWN - LINK_ATTACK_ACTIVE_TIME) return;

    int attackPos[2];
    Link_getAttackPosition(&game->player, attackPos);

    // Zone d'attaque élargie : position principale + positions adjacentes
    int attackZone[3][2];
    attackZone[0][0] = attackPos[0];
    attackZone[0][1] = attackPos[1];

    // Ajouter les cases adjacentes selon la direction (arc de cercle)
    switch (game->player.direction) {
        case LINK_DIR_UP:
        case LINK_DIR_DOWN:
            attackZone[1][0] = attackPos[0] - 1;
            attackZone[1][1] = attackPos[1];
            attackZone[2][0] = attackPos[0] + 1;
            attackZone[2][1] = attackPos[1];
            break;
        case LINK_DIR_LEFT:
        case LINK_DIR_RIGHT:
            attackZone[1][0] = attackPos[0];
            attackZone[1][1] = attackPos[1] - 1;
            attackZone[2][0] = attackPos[0];
            attackZone[2][1] = attackPos[1] + 1;
            break;
    }

    for (int i = 0; i < game->enemyCount; i++) {
        if (!game->enemies[i].isActive) continue;
        // Éviter de toucher plusieurs fois le même ennemi pendant une attaque
        if (game->enemies[i].hitTimer > 0) continue;

        // Vérifier si l'ennemi est dans la zone d'attaque
        for (int z = 0; z < 3; z++) {
            if (Enemy_collidesWith(&game->enemies[i], attackZone[z])) {
                if (Enemy_takeDamage(&game->enemies[i], 1)) {
                    // Ennemi mort
                    game->stats.kills++;
                    game->stats.score += ENEMY_KILL_SCORE;
                }
                break;  // Un seul hit par ennemi par attaque
            }
        }
    }
}

/**
 * @brief Compte le nombre d'ennemis actifs
 */
static int countActiveEnemies(const Game* game) {
    int count = 0;
    for (int i = 0; i < game->enemyCount; i++) {
        if (game->enemies[i].isActive) {
            count++;
        }
    }
    return count;
}

/**
 * @brief Gère les inputs pendant l'état PLAYING
 */
static void handlePlayingInput(Game* game, InputAction input) {
    // Sauvegarde de la position avant mouvement
    int oldPos[2] = {game->player.base.pos[0], game->player.base.pos[1]};

    switch (input) {
        case INPUT_ACTION_QUIT:
            game->running = false;
            break;

        case INPUT_ACTION_PAUSE_TOGGLE:
            Game_pause(game);
            break;

        case INPUT_ACTION_MOVE_UP: {
            const int delta[2] = {0, -1};
            Link_move(&game->player, delta);
            break;
        }

        case INPUT_ACTION_MOVE_DOWN: {
            const int delta[2] = {0, 1};
            Link_move(&game->player, delta);
            break;
        }

        case INPUT_ACTION_MOVE_LEFT: {
            const int delta[2] = {-1, 0};
            Link_move(&game->player, delta);
            break;
        }

        case INPUT_ACTION_MOVE_RIGHT: {
            const int delta[2] = {1, 0};
            Link_move(&game->player, delta);
            break;
        }

        case INPUT_ACTION_ATTACK:
            Link_attack(&game->player);
            break;

        default:
            break;
    }

    // Incrémenter le compteur de mouvements si le joueur a bougé
    if (oldPos[0] != game->player.base.pos[0] || oldPos[1] != game->player.base.pos[1]) {
        game->stats.moves++;
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
            if (game->state == STATE_MENU) {
                game->running = false;
            } else {
                Game_setState(game, STATE_MENU);
            }
            break;

        default:
            break;
    }
}

/**
 * @brief Dessine l'effet d'attaque de Link avec la zone en arc
 */
static void drawAttackEffect(const Game* game) {
    if (!Link_isAttacking(&game->player)) return;

    int attackPos[2];
    Link_getAttackPosition(&game->player, attackPos);

    // Zone d'attaque élargie : position principale + positions adjacentes
    int attackZone[3][2];
    attackZone[0][0] = attackPos[0];
    attackZone[0][1] = attackPos[1];

    switch (game->player.direction) {
        case LINK_DIR_UP:
        case LINK_DIR_DOWN:
            attackZone[1][0] = attackPos[0] - 1;
            attackZone[1][1] = attackPos[1];
            attackZone[2][0] = attackPos[0] + 1;
            attackZone[2][1] = attackPos[1];
            break;
        case LINK_DIR_LEFT:
        case LINK_DIR_RIGHT:
            attackZone[1][0] = attackPos[0];
            attackZone[1][1] = attackPos[1] - 1;
            attackZone[2][0] = attackPos[0];
            attackZone[2][1] = attackPos[1] + 1;
            break;
    }

    SDL_SetRenderDrawBlendMode(game->render.renderer, SDL_BLENDMODE_BLEND);

    // Dessiner la zone d'attaque avec un dégradé (centre plus lumineux)
    for (int z = 0; z < 3; z++) {
        int screenX = (attackZone[z][0] - game->map.currentRoom[0] * GRID_ROOM_WIDTH) * GRID_CELL_SIZE;
        int screenY = (attackZone[z][1] - game->map.currentRoom[1] * GRID_ROOM_HEIGHT) * GRID_CELL_SIZE;

        // Centre plus lumineux, côtés plus sombres
        int alpha = (z == 0) ? 180 : 100;
        SDL_SetRenderDrawColor(game->render.renderer, 255, 220, 50, alpha);
        SDL_Rect attackRect = {screenX + 3, screenY + 3, GRID_CELL_SIZE - 6, GRID_CELL_SIZE - 6};
        SDL_RenderFillRect(game->render.renderer, &attackRect);

        // Bordure
        SDL_SetRenderDrawColor(game->render.renderer, 255, 150, 0, alpha);
        SDL_RenderDrawRect(game->render.renderer, &attackRect);
    }
}

/**
 * @brief Dessine les ennemis actifs
 */
static void drawEnemies(const Game* game) {
    for (int i = 0; i < game->enemyCount; i++) {
        Enemy_draw(&game->enemies[i], game->render.renderer);
    }
}

/**
 * @brief Dessine Link avec effet de clignotement si invincible
 */
static void drawPlayer(const Game* game) {
    Link_draw(&game->player, game->render.renderer);
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
    if (game->previousState == STATE_PLAYING || game->previousState == STATE_PAUSED ||
        game->state == STATE_PLAYING || game->state == STATE_PAUSED || game->state == STATE_GAMEOVER) {
        Link_destroy(&game->player);
        Map_destroy(&game->map);
    }

    // --- Libération des ennemis ---
    for (int i = 0; i < game->enemyCount; i++) {
        Enemy_destroy(&game->enemies[i]);
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
    if (game->previousState == STATE_PLAYING || game->previousState == STATE_PAUSED ||
        game->previousState == STATE_GAMEOVER) {
        Link_destroy(&game->player);
        Map_destroy(&game->map);
        for (int i = 0; i < game->enemyCount; i++) {
            Enemy_destroy(&game->enemies[i]);
        }
        game->enemyCount = 0;
    }

    // Réinitialiser
    resetPlayerStats(game);
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

    // Sauvegarde de la salle actuelle pour détecter les changements
    int oldRoom[2] = {game->map.currentRoom[0], game->map.currentRoom[1]};

    // Mise à jour du joueur
    Link_update(&game->player);

    // Gestion de l'attaque
    handleAttack(game);

    // Gestion des transitions de salles
    Room_handleTransition(&game->map, game->player.base.pos);

    // Si changement de salle, spawn de nouveaux ennemis
    if (oldRoom[0] != game->map.currentRoom[0] || oldRoom[1] != game->map.currentRoom[1]) {
        spawnEnemiesForRoom(game);
    }

    // Mise à jour des ennemis (avec gestion des collisions entre eux)
    for (int i = 0; i < game->enemyCount; i++) {
        if (game->enemies[i].isActive) {
            Enemy_update(&game->enemies[i], game->player.base.pos, game->enemies, game->enemyCount, i);
        }
    }

    // Vérification des collisions avec les ennemis
    checkEnemyCollisions(game);

    // Temps de jeu (en frames)
    game->stats.playtime++;

    // Bonus de score si tous les ennemis sont morts
    if (countActiveEnemies(game) == 0) {
        game->stats.score += 50;  // Bonus salle nettoyée
        spawnEnemiesForRoom(game);  // Respawn
    }

    // Vérification game over
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

        case STATE_PAUSED: {
            // Afficher le jeu en arrière-plan (assombri)
            SDL_SetRenderDrawColor(game->render.renderer, 0, 0, 0, 255);
            clearRenderer(game->render.renderer);
            Map_draw(&game->map, false);
            drawEnemies(game);
            drawPlayer(game);

            // Overlay semi-transparent
            SDL_SetRenderDrawBlendMode(game->render.renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(game->render.renderer, 0, 0, 0, 180);
            SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            SDL_RenderFillRect(game->render.renderer, &overlay);

            // Menu pause par-dessus
            Menu_render(&game->menu, &game->render);
            break;
        }

        case STATE_PLAYING:
            // --- Effacement de l'écran ---
            SDL_SetRenderDrawColor(game->render.renderer, 0, 0, 0, 255);
            clearRenderer(game->render.renderer);

            // --- Carte ---
            Map_draw(&game->map, true);

            // --- Effet d'attaque ---
            drawAttackEffect(game);

            // --- Ennemis ---
            drawEnemies(game);

            // --- Joueur ---
            drawPlayer(game);

            // --- HUD ---
            HUD_render(&game->render, &game->stats, game->player.base.lives, game->map.currentRoom);

            // --- Affichage ---
            updateDisplay(game->render.renderer);
            break;
    }
}
