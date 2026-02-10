#include "hud.h"
#include "render.h"
#include <string.h>

static char s_messageBuffer[128] = "";
static int  s_messageTimer = 0;

static int getHudYPosition(void) {
    return WINDOW_HEIGHT - WINDOW_TEXTAREA_HEIGHT;
}

static void renderHudBackground(SDL_Renderer* renderer, const SDL_Color* bgColor) {
    const int hudY = getHudYPosition();
    SDL_Rect hudBackground = {0, hudY, WINDOW_WIDTH, WINDOW_TEXTAREA_HEIGHT};
    SDL_SetRenderDrawColor(renderer, bgColor->r, bgColor->g, bgColor->b, bgColor->a);
    SDL_RenderFillRect(renderer, &hudBackground);
}

static void renderHudSeparator(SDL_Renderer* renderer, const SDL_Color* separatorColor) {
    const int hudY = getHudYPosition();
    SDL_SetRenderDrawColor(renderer, separatorColor->r, separatorColor->g,
                          separatorColor->b, separatorColor->a);
    SDL_RenderDrawLine(renderer, 0, hudY, WINDOW_WIDTH, hudY);
}

static void drawKey(SDL_Renderer* renderer, TTF_Font* font, int x, int y,
                    const char* key, bool highlight) {
    const int keySize = 22;

    SDL_Color bgColor = highlight ? (SDL_Color){100, 150, 255, 255} : (SDL_Color){60, 60, 70, 255};
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_Rect keyRect = {x, y, keySize, keySize};
    SDL_RenderFillRect(renderer, &keyRect);

    SDL_SetRenderDrawColor(renderer, 120, 120, 130, 255);
    SDL_RenderDrawRect(renderer, &keyRect);

    SDL_SetRenderDrawColor(renderer, 90, 90, 100, 255);
    SDL_RenderDrawLine(renderer, x + 1, y + 1, x + keySize - 2, y + 1);

    if (font && key) {
        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Surface* surface = TTF_RenderText_Solid(font, key, textColor);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect textRect = {
                    x + (keySize - surface->w) / 2,
                    y + (keySize - surface->h) / 2,
                    surface->w, surface->h
                };
                SDL_RenderCopy(renderer, texture, NULL, &textRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }
}

static void drawControls(SDL_Renderer* renderer, TTF_Font* font, int startX, int startY) {
    const int keySize = 22;
    const int spacing = 2;

    drawKey(renderer, font, startX + keySize + spacing, startY, "^", false);

    int row2Y = startY + keySize + spacing;
    drawKey(renderer, font, startX, row2Y, "<", false);
    drawKey(renderer, font, startX + keySize + spacing, row2Y, "v", false);
    drawKey(renderer, font, startX + (keySize + spacing) * 2, row2Y, ">", false);

    int row3Y = startY + (keySize + spacing) * 2 + 5;
    drawKey(renderer, font, startX, row3Y, "F", false);
    drawKey(renderer, font, startX + keySize + spacing + 5, row3Y, "P", false);

    SDL_Color labelColor = {150, 150, 150, 255};
    if (font) {
        SDL_Surface* surface = TTF_RenderText_Solid(font, "ATK", labelColor);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect rect = {startX + (keySize + spacing) * 2 + 15, row3Y + 3, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, NULL, &rect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }
}

void HUD_initConfig(HUDConfig* config) {
    if (!config) return;

    config->backgroundColor = (SDL_Color){HUD_BG_COLOR_R, HUD_BG_COLOR_G,
                                          HUD_BG_COLOR_B, HUD_BG_COLOR_A};
    config->separatorColor = (SDL_Color){HUD_SEPARATOR_COLOR_R, HUD_SEPARATOR_COLOR_G,
                                         HUD_SEPARATOR_COLOR_B, HUD_SEPARATOR_COLOR_A};
    config->textColor = (SDL_Color){255, 255, 255, 255};
    config->lineSpacing = HUD_LINE_SPACING;
    config->showDebugInfo = false;
}

void HUD_render(const RenderState* render, const PlayerStats* stats, int lives, const int currentRoom[2]) {
    HUDConfig config;
    HUD_initConfig(&config);
    HUD_renderWithConfig(render, stats, lives, currentRoom, &config);
}

void HUD_renderWithConfig(const RenderState* render, const PlayerStats* stats,
                          int lives, const int currentRoom[2], const HUDConfig* config) {
    if (!render || !stats || !currentRoom || !config) return;

    const int hudY = getHudYPosition();
    const int lineSpacing = config->lineSpacing;

    renderHudBackground(render->renderer, &config->backgroundColor);

    renderHudSeparator(render->renderer, &config->separatorColor);

    char buffer[64];

    for (int i = 0; i < GAME_INITIAL_LIVES; i++) {
        int heartX = HUD_MARGIN_LEFT + i * 25;
        int heartY = hudY + HUD_MARGIN_TOP;

        if (i < lives) {
            SDL_SetRenderDrawColor(render->renderer, 255, 50, 50, 255);
        } else {
            SDL_SetRenderDrawColor(render->renderer, 80, 80, 80, 255);
        }

        SDL_Rect heart1 = {heartX, heartY + 3, 8, 8};
        SDL_Rect heart2 = {heartX + 8, heartY + 3, 8, 8};
        SDL_RenderFillRect(render->renderer, &heart1);
        SDL_RenderFillRect(render->renderer, &heart2);

        for (int j = 0; j < 8; j++) {
            SDL_RenderDrawLine(render->renderer,
                heartX + j, heartY + 11 + j / 2,
                heartX + 16 - j, heartY + 11 + j / 2);
        }
    }

    snprintf(buffer, sizeof(buffer), "Score : %d", stats->score);
    printTextWithFont(HUD_MARGIN_LEFT, hudY + HUD_MARGIN_TOP + lineSpacing, buffer, render->font, render->renderer);

    snprintf(buffer, sizeof(buffer), "Kills : %d", stats->kills);
    printTextWithFont(HUD_COLUMN_2_X, hudY + HUD_MARGIN_TOP, buffer, render->font, render->renderer);

    const int seconds = stats->playtime / 60;
    snprintf(buffer, sizeof(buffer), "Temps : %02d:%02d", seconds / 60, seconds % 60);
    printTextWithFont(HUD_COLUMN_2_X, hudY + HUD_MARGIN_TOP + lineSpacing, buffer, render->font, render->renderer);

    snprintf(buffer, sizeof(buffer), "Salle : [%d, %d]", currentRoom[0], currentRoom[1]);
    printTextWithFont(HUD_COLUMN_3_X, hudY + HUD_MARGIN_TOP, buffer, render->font, render->renderer);

    snprintf(buffer, sizeof(buffer), "Moves : %d", stats->moves);
    printTextWithFont(HUD_COLUMN_3_X, hudY + HUD_MARGIN_TOP + lineSpacing, buffer, render->font, render->renderer);

    drawControls(render->renderer, render->font, HUD_CONTROLS_X, hudY + HUD_MARGIN_TOP);
}

void HUD_renderDebugInfo(const RenderState* render, int fps, int entityCount) {
    if (!render) return;

    const int hudY = getHudYPosition();
    char buffer[64];

    snprintf(buffer, sizeof(buffer), "FPS: %d", fps);
    printTextWithFont(WINDOW_WIDTH - 100, hudY + HUD_MARGIN_TOP, buffer, render->font, render->renderer);

    snprintf(buffer, sizeof(buffer), "Entites: %d", entityCount);
    printTextWithFont(WINDOW_WIDTH - 100, hudY + HUD_MARGIN_TOP + HUD_LINE_SPACING,
                     buffer, render->font, render->renderer);
}

void HUD_showMessage(const RenderState* render, const char* message, int duration) {
    (void)render;
    if (!message) return;

    strncpy(s_messageBuffer, message, sizeof(s_messageBuffer) - 1);
    s_messageBuffer[sizeof(s_messageBuffer) - 1] = '\0';
    s_messageTimer = duration;
}

void HUD_renderHealthBar(const RenderState* render, int x, int y,
                         int currentLives, int maxLives, int width, int height) {
    if (!render || maxLives <= 0) return;

    if (currentLives > maxLives) currentLives = maxLives;
    if (currentLives < 0) currentLives = 0;

    const int filledWidth = (width * currentLives) / maxLives;

    SDL_Rect backgroundRect = {x, y, width, height};
    SDL_SetRenderDrawColor(render->renderer, 80, 0, 0, 255);
    SDL_RenderFillRect(render->renderer, &backgroundRect);

    if (filledWidth > 0) {
        SDL_Rect filledRect = {x, y, filledWidth, height};

        if (currentLives > maxLives / 2) {

            SDL_SetRenderDrawColor(render->renderer, 0, 200, 0, 255);
        } else if (currentLives > maxLives / 4) {

            SDL_SetRenderDrawColor(render->renderer, 255, 165, 0, 255);
        } else {

            SDL_SetRenderDrawColor(render->renderer, 200, 0, 0, 255);
        }

        SDL_RenderFillRect(render->renderer, &filledRect);
    }

    SDL_Rect borderRect = {x, y, width, height};
    SDL_SetRenderDrawColor(render->renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(render->renderer, &borderRect);
}
