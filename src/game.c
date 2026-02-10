#include "game.h"
#include "render.h"
#include "iomanager.h"
#include "hud.h"
#include "audio.h"
#include "assets.h"

#include <stdlib.h>
#include <time.h>

#define ENEMIES_PER_ZONE    5
#define ENEMY_SPAWN_MIN_DISTANCE    4
#define ENEMY_SPAWN_MAX_DISTANCE    10
#define ENEMY_DESPAWN_DISTANCE      20

static float absf(float value) {
    return value < 0.0f ? -value : value;
}

static void getRandomPositionNearPlayer(const Map* map, const int playerPos[2], int outPos[2]) {
    for (int attempts = 0; attempts < 100; attempts++) {
        int offsetX = (rand() % (2 * ENEMY_SPAWN_MAX_DISTANCE + 1)) - ENEMY_SPAWN_MAX_DISTANCE;
        int offsetY = (rand() % (2 * ENEMY_SPAWN_MAX_DISTANCE + 1)) - ENEMY_SPAWN_MAX_DISTANCE;

        outPos[0] = playerPos[0] + offsetX;
        outPos[1] = playerPos[1] + offsetY;

        if (outPos[0] < 1 || outPos[0] >= GRID_WORLD_WIDTH - 1 ||
            outPos[1] < 1 || outPos[1] >= GRID_WORLD_HEIGHT - 1) {
            continue;
        }

        int dx = outPos[0] - playerPos[0];
        int dy = outPos[1] - playerPos[1];
        int distance = (dx > 0 ? dx : -dx) + (dy > 0 ? dy : -dy);

        if (!Map_isBlocking(map, outPos) && distance >= ENEMY_SPAWN_MIN_DISTANCE) {
            return;
        }
    }

    outPos[0] = playerPos[0] + ENEMY_SPAWN_MIN_DISTANCE;
    outPos[1] = playerPos[1];
}

static void spawnEnemiesNearPlayer(Game* game) {
    for (int i = 0; i < game->enemyCount; i++) {
        Enemy_destroy(&game->enemies[i]);
    }
    game->enemyCount = 0;

    int playerPos[2];
    Character_getGridPos(&game->player.base, playerPos);

    for (int i = 0; i < ENEMIES_PER_ZONE && game->enemyCount < GAME_MAX_ENEMIES; i++) {
        int spawnPos[2];
        getRandomPositionNearPlayer(&game->map, playerPos, spawnPos);

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

static void spawnSingleEnemy(Game* game) {
    int slot = -1;

    for (int i = 0; i < game->enemyCount; i++) {
        if (!game->enemies[i].isActive) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        if (game->enemyCount >= GAME_MAX_ENEMIES) return;
        slot = game->enemyCount;
        game->enemyCount++;
    }

    int spawnPos[2];
    int playerPos[2];
    Character_getGridPos(&game->player.base, playerPos);
    getRandomPositionNearPlayer(&game->map, playerPos, spawnPos);

    EnemyType type;
    EnemyAI ai;
    int r = rand() % 3;

    switch (r) {
        case 0:
            type = ENEMY_TYPE_BASIC;
            ai = ENEMY_AI_RANDOM;
            break;
        case 1:
            type = ENEMY_TYPE_FAST;
            ai = ENEMY_AI_CHASE;
            break;
        default:
            type = ENEMY_TYPE_TANK;
            ai = ENEMY_AI_CHASE;
            break;
    }

    Enemy_init(&game->enemies[slot], type, ai, &game->map, spawnPos);
}

static void despawnDistantEnemies(Game* game) {
    int playerPos[2];
    Character_getGridPos(&game->player.base, playerPos);

    for (int i = 0; i < game->enemyCount; i++) {
        if (!game->enemies[i].isActive) continue;

        int enemyPos[2];
        Character_getGridPos(&game->enemies[i].base, enemyPos);

        int dx = enemyPos[0] - playerPos[0];
        int dy = enemyPos[1] - playerPos[1];
        int distance = (dx > 0 ? dx : -dx) + (dy > 0 ? dy : -dy);

        if (distance > ENEMY_DESPAWN_DISTANCE) {
            game->enemies[i].isActive = false;
        }
    }
}

static void initGameplayResources(Game* game) {
    srand((int)time(NULL));

    Map_init(&game->map, game->render.renderer);

    const int initialRoom[2] = GAME_INITIAL_ROOM;
    game->map.currentRoom[0] = initialRoom[0];
    game->map.currentRoom[1] = initialRoom[1];

    Link_init(&game->player, &game->map);

    int centerPos[2];
    Room_getCenter(&game->map.rooms[initialRoom[1]][initialRoom[0]], centerPos);
    game->player.base.posX = (float)centerPos[0];
    game->player.base.posY = (float)centerPos[1];

    Camera_followF(&game->map.camera, game->player.base.posX, game->player.base.posY);
    game->map.camera.x = game->map.camera.targetX;
    game->map.camera.y = game->map.camera.targetY;

    spawnEnemiesNearPlayer(game);
}

static void resetPlayerStats(Game* game) {
    game->stats.score = GAME_INITIAL_SCORE;
    game->stats.kills = 0;
    game->stats.playtime = 0;
    game->stats.moves = 0;
}

static void checkEnemyCollisions(Game* game) {
    int playerPos[2];
    Character_getGridPos(&game->player.base, playerPos);

    for (int i = 0; i < game->enemyCount; i++) {
        if (!game->enemies[i].isActive) continue;

        int livesBefore = game->player.base.lives;
        if (Enemy_collidesWith(&game->enemies[i], playerPos)) {
            Link_takeDamage(&game->player, 1);
            if (game->player.base.lives < livesBefore) {
                Audio_playSfx(AUDIO_SFX_PLAYER_HIT);
            }
        }
    }
}

static void handleAttack(Game* game) {
    if (!Link_isAttacking(&game->player)) return;
    if (game->player.attackCooldown < LINK_ATTACK_COOLDOWN - LINK_ATTACK_ACTIVE_TIME) return;

    int attackPos[2];
    Link_getAttackPosition(&game->player, attackPos);

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

    for (int i = 0; i < game->enemyCount; i++) {
        if (!game->enemies[i].isActive) continue;
        if (game->enemies[i].hitTimer > 0) continue;

        for (int z = 0; z < 3; z++) {
            if (Enemy_collidesWith(&game->enemies[i], attackZone[z])) {
                if (Enemy_takeDamage(&game->enemies[i], 1)) {
                    game->stats.kills++;
                    game->stats.score += ENEMY_KILL_SCORE;
                    Audio_playSfx(AUDIO_SFX_ENEMY_KILLED);
                }
                break;
            }
        }
    }
}

static int countActiveEnemies(const Game* game) {
    int count = 0;
    for (int i = 0; i < game->enemyCount; i++) {
        if (game->enemies[i].isActive) {
            count++;
        }
    }
    return count;
}

static void handlePlayingInput(Game* game, InputState* input, bool quit, bool pause) {
    if (quit) {
        Audio_updateWalk(false);
        game->running = false;
        return;
    }

    if (pause) {
        Audio_updateWalk(false);
        Game_pause(game);
        return;
    }

    int gridPos[2];
    Character_getGridPos(&game->player.base, gridPos);
    int oldPos[2] = {gridPos[0], gridPos[1]};
    float oldX = game->player.base.posX;
    float oldY = game->player.base.posY;

    float deltaX = 0.0f;
    float deltaY = 0.0f;

    if (input->moveUp) deltaY -= MOVEMENT_SPEED;
    if (input->moveDown) deltaY += MOVEMENT_SPEED;
    if (input->moveLeft) deltaX -= MOVEMENT_SPEED;
    if (input->moveRight) deltaX += MOVEMENT_SPEED;

    if (deltaX != 0.0f && deltaY != 0.0f) {
        const float diagonalFactor = 0.70710678f;
        deltaX *= diagonalFactor;
        deltaY *= diagonalFactor;
    }

    if (deltaX != 0.0f || deltaY != 0.0f) {
        Link_moveSmooth(&game->player, deltaX, deltaY);
    } else if (!Link_isAttacking(&game->player)) {
        Animation_stop(&game->player.animation);
    }

    const bool isMoving = absf(game->player.base.posX - oldX) > 0.0001f ||
                          absf(game->player.base.posY - oldY) > 0.0001f;
    Audio_updateWalk(isMoving);

    if (input->attack) {
        const bool wasAttacking = Link_isAttacking(&game->player);
        Link_attack(&game->player);
        if (!wasAttacking && Link_isAttacking(&game->player)) {
            Audio_playSfx(AUDIO_SFX_ATTACK);
        }
    }

    Character_getGridPos(&game->player.base, gridPos);
    if (oldPos[0] != gridPos[0] || oldPos[1] != gridPos[1]) {
        game->stats.moves++;
    }
}

static void handleMenuAction(Game* game, MenuAction action) {
    switch (action) {
        case MENU_ACTION_START:
        case MENU_ACTION_RESTART:
            Audio_playSfx(AUDIO_SFX_MENU_CLICK);
            Game_startNewGame(game);
            break;

        case MENU_ACTION_RESUME:
            Audio_playSfx(AUDIO_SFX_MENU_CLICK);
            Game_resume(game);
            break;

        case MENU_ACTION_QUIT:
            Audio_playSfx(AUDIO_SFX_MENU_CLICK);
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

static void drawAttackEffect(const Game* game) {
    if (!Link_isAttacking(&game->player)) return;

    int attackPos[2];
    Link_getAttackPosition(&game->player, attackPos);

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

    for (int z = 0; z < 3; z++) {
        int screenPos[2];
        Camera_worldToScreen(&game->map.camera, attackZone[z], screenPos);

        int alpha = (z == 0) ? 180 : 100;
        SDL_SetRenderDrawColor(game->render.renderer, 255, 220, 50, alpha);
        SDL_Rect attackRect = {screenPos[0] + 3, screenPos[1] + 3, GRID_CELL_SIZE - 6, GRID_CELL_SIZE - 6};
        SDL_RenderFillRect(game->render.renderer, &attackRect);

        SDL_SetRenderDrawColor(game->render.renderer, 255, 150, 0, alpha);
        SDL_RenderDrawRect(game->render.renderer, &attackRect);
    }
}

static void drawEnemies(const Game* game) {
    for (int i = 0; i < game->enemyCount; i++) {
        Enemy_draw(&game->enemies[i], game->render.renderer);
    }
}

static void drawPlayer(const Game* game) {
    Link_draw(&game->player, game->render.renderer);
}

void Game_init(Game* game) {
    game->state = STATE_MENU;
    game->previousState = STATE_MENU;
    game->enemyCount = 0;
    game->running = true;
    resetPlayerStats(game);

    initSDL();
    if (!assets_init()) {
        fprintf(stderr, "Impossible d'initialiser le resolver d'assets\n");
        game->running = false;
        return;
    }
    game->render.window = createWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
    game->render.renderer = createRenderer(game->render.window);
    game->render.font = TTF_OpenFont(asset_full(WINDOW_FONT_PATH), WINDOW_FONT_SIZE);

    if (game->render.font == NULL) {
        fprintf(stderr, "Erreur chargement police : %s\n", TTF_GetError());
    }

    Audio_init(ASSET_AUDIO_CONFIG);
    Audio_setMusicTrack(AUDIO_MUSIC_MENU);
    Menu_initMain(&game->menu);
}

void Game_run(Game* game) {
    while (game->running) {
        Game_handleInput(game);
        Game_update(game);
        Game_render(game);
        SDL_Delay(16);
    }
}

void Game_destroy(Game* game) {
    if (game->previousState == STATE_PLAYING || game->previousState == STATE_PAUSED ||
        game->state == STATE_PLAYING || game->state == STATE_PAUSED ||
        game->state == STATE_GAMEOVER || game->state == STATE_WIN) {
        Link_destroy(&game->player);
        Map_destroy(&game->map);
    }

    for (int i = 0; i < game->enemyCount; i++) {
        Enemy_destroy(&game->enemies[i]);
    }

    if (game->render.font != NULL) {
        TTF_CloseFont(game->render.font);
    }
    Audio_shutdown();
    quitSDL(game->render.window, game->render.renderer);
}

void Game_setState(Game* game, GameState newState) {
    game->previousState = game->state;
    game->state = newState;

    switch (newState) {
        case STATE_MENU:
            Menu_initMain(&game->menu);
            Audio_updateWalk(false);
            Audio_setMusicTrack(AUDIO_MUSIC_MENU);
            break;

        case STATE_PAUSED:
            Menu_initPause(&game->menu);
            Audio_updateWalk(false);
            Audio_setMusicTrack(AUDIO_MUSIC_MENU);
            break;

        case STATE_GAMEOVER:
            Menu_initGameOver(&game->menu, game->stats.score);
            Audio_updateWalk(false);
            Audio_playSfx(AUDIO_SFX_GAME_OVER);
            Audio_setMusicTrack(AUDIO_MUSIC_GAMEOVER);
            break;

        case STATE_WIN:
            Menu_initWin(&game->menu, game->stats.score, GAME_WIN_KILLS);
            Audio_updateWalk(false);
            Audio_setMusicTrack(AUDIO_MUSIC_GAMEOVER);
            break;

        case STATE_PLAYING:
            Audio_setMusicTrack(AUDIO_MUSIC_GAMEPLAY);
            break;
    }
}

void Game_startNewGame(Game* game) {
    if (game->previousState == STATE_PLAYING || game->previousState == STATE_PAUSED ||
        game->previousState == STATE_GAMEOVER || game->previousState == STATE_WIN) {
        Link_destroy(&game->player);
        Map_destroy(&game->map);
        for (int i = 0; i < game->enemyCount; i++) {
            Enemy_destroy(&game->enemies[i]);
        }
        game->enemyCount = 0;
    }

    resetPlayerStats(game);
    initGameplayResources(game);

    game->state = STATE_PLAYING;
    game->previousState = STATE_PLAYING;
    Audio_setMusicTrack(AUDIO_MUSIC_GAMEPLAY);
}

void Game_pause(Game* game) {
    if (game->state == STATE_PLAYING) {
        Game_setState(game, STATE_PAUSED);
    }
}

void Game_resume(Game* game) {
    if (game->state == STATE_PAUSED) {
        game->state = STATE_PLAYING;
        Audio_setMusicTrack(AUDIO_MUSIC_GAMEPLAY);
    }
}

void Game_handleInput(Game* game) {
    switch (game->state) {
        case STATE_MENU:
        case STATE_PAUSED:
        case STATE_GAMEOVER:
        case STATE_WIN: {
            MenuAction action = Menu_handleInput(&game->menu);
            handleMenuAction(game, action);
            break;
        }

        case STATE_PLAYING: {
            InputState inputState;
            bool quit = false;
            bool pause = false;
            inputPollContinuous(&inputState, &quit, &pause);
            handlePlayingInput(game, &inputState, quit, pause);
            break;
        }
    }
}

void Game_update(Game* game) {
    if (game->state != STATE_PLAYING) {
        return;
    }

    Link_update(&game->player);
    handleAttack(game);

    int playerPos[2];
    Character_getGridPos(&game->player.base, playerPos);

    Camera_followF(&game->map.camera, game->player.base.posX, game->player.base.posY);
    Room_handleTransition(&game->map, playerPos);

    despawnDistantEnemies(game);

    int activeCount = countActiveEnemies(game);
    if (activeCount < ENEMIES_PER_ZONE) {
        int toSpawn = ENEMIES_PER_ZONE - activeCount;
        for (int i = 0; i < toSpawn; i++) {
            spawnSingleEnemy(game);
        }
    }

    for (int i = 0; i < game->enemyCount; i++) {
        if (game->enemies[i].isActive) {
            Enemy_update(&game->enemies[i], playerPos, game->enemies, game->enemyCount, i);
        }
    }

    checkEnemyCollisions(game);
    game->stats.playtime++;

    if (game->player.base.lives <= 0) {
        Game_setState(game, STATE_GAMEOVER);
    } else if (game->stats.kills >= GAME_WIN_KILLS) {
        Game_setState(game, STATE_WIN);
    }
}

void Game_render(Game* game) {
    switch (game->state) {
        case STATE_MENU:
        case STATE_GAMEOVER:
        case STATE_WIN:
            Menu_render(&game->menu, &game->render);
            break;

        case STATE_PAUSED: {
            SDL_SetRenderDrawColor(game->render.renderer, 0, 0, 0, 255);
            clearRenderer(game->render.renderer);
            Map_draw(&game->map, false);
            drawEnemies(game);
            drawPlayer(game);

            SDL_SetRenderDrawBlendMode(game->render.renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(game->render.renderer, 0, 0, 0, 180);
            SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            SDL_RenderFillRect(game->render.renderer, &overlay);

            Menu_render(&game->menu, &game->render);
            break;
        }

        case STATE_PLAYING:
            SDL_SetRenderDrawColor(game->render.renderer, 0, 0, 0, 255);
            clearRenderer(game->render.renderer);

            Map_draw(&game->map, false);
            drawAttackEffect(game);
            drawEnemies(game);
            drawPlayer(game);

            HUD_render(&game->render, &game->stats, game->player.base.lives, game->map.currentRoom);

            updateDisplay(game->render.renderer);
            break;
    }
}
