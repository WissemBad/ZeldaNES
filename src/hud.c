/**
 * @file hud.c
 * @brief Implémentation de l'affichage du HUD
 */

#include "hud.h"
#include "render.h"
#include <string.h>

//==============================================================================
// VARIABLES STATIQUES (pour les messages temporaires)
//==============================================================================

static char s_messageBuffer[128] = "";
static int  s_messageTimer = 0;

//==============================================================================
// FONCTIONS PRIVÉES
//==============================================================================

/**
 * @brief Calcule la position Y du début de la zone HUD
 */
static int getHudYPosition(void) {
    return WINDOW_HEIGHT - WINDOW_TEXTAREA_HEIGHT;
}

/**
 * @brief Affiche le fond du HUD (rectangle coloré)
 */
static void renderHudBackground(SDL_Renderer* renderer, const SDL_Color* bgColor) {
    const int hudY = getHudYPosition();
    SDL_Rect hudBackground = {0, hudY, WINDOW_WIDTH, WINDOW_TEXTAREA_HEIGHT};
    SDL_SetRenderDrawColor(renderer, bgColor->r, bgColor->g, bgColor->b, bgColor->a);
    SDL_RenderFillRect(renderer, &hudBackground);
}

/**
 * @brief Affiche la ligne de séparation entre le jeu et le HUD
 */
static void renderHudSeparator(SDL_Renderer* renderer, const SDL_Color* separatorColor) {
    const int hudY = getHudYPosition();
    SDL_SetRenderDrawColor(renderer, separatorColor->r, separatorColor->g,
                          separatorColor->b, separatorColor->a);
    SDL_RenderDrawLine(renderer, 0, hudY, WINDOW_WIDTH, hudY);
}

//==============================================================================
// FONCTIONS PUBLIQUES
//==============================================================================

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

    // Fond du HUD
    renderHudBackground(render->renderer, &config->backgroundColor);

    // Ligne de séparation
    renderHudSeparator(render->renderer, &config->separatorColor);

    // Textes d'information
    char buffer[64];

    // Colonne 1 - Vies et Score
    snprintf(buffer, sizeof(buffer), "Vies : %d", lives);
    printTextWithFont(HUD_MARGIN_LEFT, hudY + HUD_MARGIN_TOP, buffer, render->font, render->renderer);

    snprintf(buffer, sizeof(buffer), "Score : %d", stats->score);
    printTextWithFont(HUD_MARGIN_LEFT, hudY + HUD_MARGIN_TOP + lineSpacing, buffer, render->font, render->renderer);

    // Colonne 2 - Kills et Temps
    snprintf(buffer, sizeof(buffer), "Kills : %d", stats->kills);
    printTextWithFont(HUD_COLUMN_2_X, hudY + HUD_MARGIN_TOP, buffer, render->font, render->renderer);

    const int seconds = stats->playtime / 60;
    snprintf(buffer, sizeof(buffer), "Temps : %02d:%02d", seconds / 60, seconds % 60);
    printTextWithFont(HUD_COLUMN_2_X, hudY + HUD_MARGIN_TOP + lineSpacing, buffer, render->font, render->renderer);

    // Colonne 3 - Salle actuelle
    snprintf(buffer, sizeof(buffer), "Salle : [%d, %d]", currentRoom[0], currentRoom[1]);
    printTextWithFont(HUD_COLUMN_3_X, hudY + HUD_MARGIN_TOP, buffer, render->font, render->renderer);

    // Afficher le message temporaire s'il existe
    if (s_messageTimer > 0) {
        printTextWithFont(HUD_COLUMN_3_X, hudY + HUD_MARGIN_TOP + lineSpacing,
                         s_messageBuffer, render->font, render->renderer);
        s_messageTimer--;
    }
}

void HUD_renderDebugInfo(const RenderState* render, int fps, int entityCount) {
    if (!render) return;

    const int hudY = getHudYPosition();
    char buffer[64];

    // Afficher les FPS
    snprintf(buffer, sizeof(buffer), "FPS: %d", fps);
    printTextWithFont(WINDOW_WIDTH - 100, hudY + HUD_MARGIN_TOP, buffer, render->font, render->renderer);

    // Afficher le nombre d'entités
    snprintf(buffer, sizeof(buffer), "Entites: %d", entityCount);
    printTextWithFont(WINDOW_WIDTH - 100, hudY + HUD_MARGIN_TOP + HUD_LINE_SPACING,
                     buffer, render->font, render->renderer);
}

void HUD_showMessage(const RenderState* render, const char* message, int duration) {
    (void)render;  // Paramètre réservé pour usage futur
    if (!message) return;

    strncpy(s_messageBuffer, message, sizeof(s_messageBuffer) - 1);
    s_messageBuffer[sizeof(s_messageBuffer) - 1] = '\0';
    s_messageTimer = duration;
}

void HUD_renderHealthBar(const RenderState* render, int x, int y,
                         int currentLives, int maxLives, int width, int height) {
    if (!render || maxLives <= 0) return;

    // Limiter les vies courantes au maximum
    if (currentLives > maxLives) currentLives = maxLives;
    if (currentLives < 0) currentLives = 0;

    // Calculer la largeur de la barre de vie remplie
    const int filledWidth = (width * currentLives) / maxLives;

    // Fond de la barre (rouge sombre)
    SDL_Rect backgroundRect = {x, y, width, height};
    SDL_SetRenderDrawColor(render->renderer, 80, 0, 0, 255);
    SDL_RenderFillRect(render->renderer, &backgroundRect);

    // Partie remplie de la barre (vert ou rouge selon la vie restante)
    if (filledWidth > 0) {
        SDL_Rect filledRect = {x, y, filledWidth, height};

        // Changer la couleur selon le pourcentage de vie
        if (currentLives > maxLives / 2) {
            // Plus de 50% : vert
            SDL_SetRenderDrawColor(render->renderer, 0, 200, 0, 255);
        } else if (currentLives > maxLives / 4) {
            // Entre 25% et 50% : orange
            SDL_SetRenderDrawColor(render->renderer, 255, 165, 0, 255);
        } else {
            // Moins de 25% : rouge
            SDL_SetRenderDrawColor(render->renderer, 200, 0, 0, 255);
        }

        SDL_RenderFillRect(render->renderer, &filledRect);
    }

    // Bordure de la barre (blanc)
    SDL_Rect borderRect = {x, y, width, height};
    SDL_SetRenderDrawColor(render->renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(render->renderer, &borderRect);
}
