#ifndef NUPRC_HUD_H
#define NUPRC_HUD_H

#include "core.h"

#define HUD_HEIGHT              WINDOW_TEXTAREA_HEIGHT
#define HUD_LINE_SPACING        25

#define HUD_MARGIN_LEFT         10
#define HUD_MARGIN_TOP          10
#define HUD_COLUMN_2_X          180
#define HUD_COLUMN_3_X          360
#define HUD_CONTROLS_X          550

#define HUD_BG_COLOR_R          40
#define HUD_BG_COLOR_G          40
#define HUD_BG_COLOR_B          40
#define HUD_BG_COLOR_A          255

#define HUD_SEPARATOR_COLOR_R   200
#define HUD_SEPARATOR_COLOR_G   200
#define HUD_SEPARATOR_COLOR_B   200
#define HUD_SEPARATOR_COLOR_A   255

typedef struct {
    SDL_Color backgroundColor;
    SDL_Color separatorColor;
    SDL_Color textColor;
    int       lineSpacing;
    bool      showDebugInfo;
} HUDConfig;

void HUD_render(const RenderState* render, const PlayerStats* stats, int lives, const int currentRoom[2]);
void HUD_initConfig(HUDConfig* config);
void HUD_renderWithConfig(const RenderState* render, const PlayerStats* stats, int lives, const int currentRoom[2], const HUDConfig* config);
void HUD_renderDebugInfo(const RenderState* render, int fps, int entityCount);
void HUD_showMessage(const RenderState* render, const char* message, int duration);
void HUD_renderHealthBar(const RenderState* render, int x, int y, int currentLives, int maxLives, int width, int height);

#endif
