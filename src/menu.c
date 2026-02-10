#include "menu.h"
#include "render.h"
#include "iomanager.h"
#include "audio.h"
#include <string.h>

static void addOption(Menu* menu, const char* label, MenuAction action, bool enabled) {
    if (menu->optionCount >= MENU_MAX_OPTIONS) return;

    MenuOption* opt = &menu->options[menu->optionCount];
    strncpy(opt->label, label, sizeof(opt->label) - 1);
    opt->label[sizeof(opt->label) - 1] = '\0';
    opt->action = action;
    opt->enabled = enabled;
    menu->optionCount++;
}

static void drawStyledRect(SDL_Renderer* renderer, int x, int y, int w, int h,
                           SDL_Color fillColor, SDL_Color borderColor, bool selected) {

    if (selected) {
        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_Rect border = {x - 2, y - 2, w + 4, h + 4};
        SDL_RenderFillRect(renderer, &border);
    }

    SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);

    if (selected) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 60);
        SDL_RenderDrawLine(renderer, x + 2, y + 2, x + w - 2, y + 2);
    }
}

static void drawTitle(SDL_Renderer* renderer, TTF_Font* font, const char* title, int centerX, int y) {
    if (!font || !title) return;

    SDL_Color shadowColor = {0, 0, 0, 200};
    SDL_Surface* shadowSurface = TTF_RenderText_Solid(font, title, shadowColor);
    if (shadowSurface) {
        SDL_Texture* shadowTex = SDL_CreateTextureFromSurface(renderer, shadowSurface);
        if (shadowTex) {
            SDL_Rect shadowRect = {centerX - shadowSurface->w / 2 + 3, y + 3,
                                   shadowSurface->w, shadowSurface->h};
            SDL_RenderCopy(renderer, shadowTex, NULL, &shadowRect);
            SDL_DestroyTexture(shadowTex);
        }
        SDL_FreeSurface(shadowSurface);
    }

    SDL_Color titleColor = {MENU_TEXT_R, MENU_TEXT_G, MENU_TEXT_B, MENU_TEXT_A};
    SDL_Surface* surface = TTF_RenderText_Solid(font, title, titleColor);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_Rect rect = {centerX - surface->w / 2, y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

static void drawMenuBackground(SDL_Renderer* renderer, MenuType type) {

    SDL_SetRenderDrawColor(renderer, MENU_BG_R, MENU_BG_G, MENU_BG_B, MENU_BG_A);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
    for (int i = 0; i < WINDOW_HEIGHT; i += 4) {
        SDL_RenderDrawLine(renderer, 0, i, WINDOW_WIDTH, i);
    }

    SDL_Color borderColor = {100, 150, 255, 255};
    switch (type) {
        case MENU_TYPE_MAIN:
            borderColor = (SDL_Color){100, 150, 255, 255};
            break;
        case MENU_TYPE_PAUSE:
            borderColor = (SDL_Color){255, 200, 100, 255};
            break;
        case MENU_TYPE_GAMEOVER:
            borderColor = (SDL_Color){255, 80, 80, 255};
            break;
        case MENU_TYPE_WIN:
            borderColor = (SDL_Color){90, 210, 120, 255};
            break;
    }

    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);

    SDL_RenderDrawLine(renderer, 50, 50, WINDOW_WIDTH - 50, 50);
    SDL_RenderDrawLine(renderer, 50, WINDOW_HEIGHT - 50, WINDOW_WIDTH - 50, WINDOW_HEIGHT - 50);

    SDL_Rect cornerTL = {45, 45, 10, 10};
    SDL_Rect cornerTR = {WINDOW_WIDTH - 55, 45, 10, 10};
    SDL_Rect cornerBL = {45, WINDOW_HEIGHT - 55, 10, 10};
    SDL_Rect cornerBR = {WINDOW_WIDTH - 55, WINDOW_HEIGHT - 55, 10, 10};
    SDL_RenderFillRect(renderer, &cornerTL);
    SDL_RenderFillRect(renderer, &cornerTR);
    SDL_RenderFillRect(renderer, &cornerBL);
    SDL_RenderFillRect(renderer, &cornerBR);
}

void Menu_initMain(Menu* menu) {
    memset(menu, 0, sizeof(Menu));
    menu->type = MENU_TYPE_MAIN;
    strncpy(menu->title, "NUPRC", sizeof(menu->title) - 1);
    strncpy(menu->subtitle, "A Zelda-like Adventure", sizeof(menu->subtitle) - 1);
    menu->selectedIndex = 0;

    addOption(menu, "Nouvelle Partie", MENU_ACTION_START, true);
    addOption(menu, "Quitter", MENU_ACTION_QUIT, true);
}

void Menu_initPause(Menu* menu) {
    memset(menu, 0, sizeof(Menu));
    menu->type = MENU_TYPE_PAUSE;
    strncpy(menu->title, "PAUSE", sizeof(menu->title) - 1);
    strncpy(menu->subtitle, "Le jeu est en pause", sizeof(menu->subtitle) - 1);
    menu->selectedIndex = 0;

    addOption(menu, "Reprendre", MENU_ACTION_RESUME, true);
    addOption(menu, "Recommencer", MENU_ACTION_RESTART, true);
    addOption(menu, "Menu Principal", MENU_ACTION_QUIT, true);
}

void Menu_initGameOver(Menu* menu, int finalScore) {
    memset(menu, 0, sizeof(Menu));
    menu->type = MENU_TYPE_GAMEOVER;
    strncpy(menu->title, "GAME OVER", sizeof(menu->title) - 1);
    snprintf(menu->subtitle, sizeof(menu->subtitle), "Score Final : %d", finalScore);
    menu->selectedIndex = 0;

    addOption(menu, "Recommencer", MENU_ACTION_RESTART, true);
    addOption(menu, "Menu Principal", MENU_ACTION_QUIT, true);
}

void Menu_initWin(Menu* menu, int finalScore, int killsTarget) {
    memset(menu, 0, sizeof(Menu));
    menu->type = MENU_TYPE_WIN;
    strncpy(menu->title, "VICTOIRE", sizeof(menu->title) - 1);
    snprintf(menu->subtitle, sizeof(menu->subtitle),
             "Objectif atteint : %d/%d ennemis | Score : %d",
             killsTarget, killsTarget, finalScore);
    menu->selectedIndex = 0;

    addOption(menu, "Rejouer", MENU_ACTION_RESTART, true);
    addOption(menu, "Menu Principal", MENU_ACTION_QUIT, true);
}

void Menu_moveUp(Menu* menu) {
    if (menu->optionCount == 0) return;

    do {
        menu->selectedIndex--;
        if (menu->selectedIndex < 0) {
            menu->selectedIndex = menu->optionCount - 1;
        }
    } while (!menu->options[menu->selectedIndex].enabled && menu->optionCount > 1);
}

void Menu_moveDown(Menu* menu) {
    if (menu->optionCount == 0) return;

    do {
        menu->selectedIndex++;
        if (menu->selectedIndex >= menu->optionCount) {
            menu->selectedIndex = 0;
        }
    } while (!menu->options[menu->selectedIndex].enabled && menu->optionCount > 1);
}

MenuAction Menu_confirm(const Menu* menu) {
    if (menu->optionCount == 0) return MENU_ACTION_NONE;
    if (!menu->options[menu->selectedIndex].enabled) return MENU_ACTION_NONE;

    return menu->options[menu->selectedIndex].action;
}

MenuAction Menu_handleInput(Menu* menu) {
    InputAction input = inputPoll();

    switch (input) {
        case INPUT_ACTION_QUIT:
            return MENU_ACTION_QUIT;

        case INPUT_ACTION_MOVE_UP:
            Menu_moveUp(menu);
            Audio_playSfx(AUDIO_SFX_MENU_MOVE);
            break;

        case INPUT_ACTION_MOVE_DOWN:
            Menu_moveDown(menu);
            Audio_playSfx(AUDIO_SFX_MENU_MOVE);
            break;

        case INPUT_ACTION_CONFIRM:
        case INPUT_ACTION_ATTACK:
            return Menu_confirm(menu);

        case INPUT_ACTION_PAUSE_TOGGLE:
            if (menu->type == MENU_TYPE_PAUSE) {
                return MENU_ACTION_RESUME;
            }
            break;

        default:
            break;
    }

    return MENU_ACTION_NONE;
}

void Menu_render(const Menu* menu, const RenderState* render) {
    if (!menu || !render || !render->renderer) return;

    drawMenuBackground(render->renderer, menu->type);

    int titleY = 80;
    drawTitle(render->renderer, render->font, menu->title, WINDOW_WIDTH / 2, titleY);

    if (strlen(menu->subtitle) > 0 && render->font) {
        SDL_Color subtitleColor = {180, 180, 180, 255};
        SDL_Surface* surface = TTF_RenderText_Solid(render->font, menu->subtitle, subtitleColor);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(render->renderer, surface);
            if (texture) {
                SDL_Rect rect = {WINDOW_WIDTH / 2 - surface->w / 2, titleY + 35,
                                surface->w, surface->h};
                SDL_RenderCopy(render->renderer, texture, NULL, &rect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }

    int menuStartY = WINDOW_HEIGHT / 2 - (menu->optionCount * (MENU_BUTTON_HEIGHT + MENU_BUTTON_SPACING)) / 2;
    int buttonX = WINDOW_WIDTH / 2 - MENU_BUTTON_WIDTH / 2;

    for (int i = 0; i < menu->optionCount; i++) {
        const MenuOption* opt = &menu->options[i];
        int buttonY = menuStartY + i * (MENU_BUTTON_HEIGHT + MENU_BUTTON_SPACING);
        bool isSelected = (i == menu->selectedIndex);

        SDL_Color fillColor, borderColor, textColor;

        if (!opt->enabled) {
            fillColor = (SDL_Color){40, 40, 40, 255};
            borderColor = (SDL_Color){60, 60, 60, 255};
            textColor = (SDL_Color){100, 100, 100, 255};
        } else if (isSelected) {
            fillColor = (SDL_Color){MENU_HIGHLIGHT_R, MENU_HIGHLIGHT_G, MENU_HIGHLIGHT_B, MENU_HIGHLIGHT_A};
            borderColor = (SDL_Color){150, 200, 255, 255};
            textColor = (SDL_Color){255, 255, 255, 255};
        } else {
            fillColor = (SDL_Color){50, 50, 60, 255};
            borderColor = (SDL_Color){80, 80, 100, 255};
            textColor = (SDL_Color){200, 200, 200, 255};
        }

        drawStyledRect(render->renderer, buttonX, buttonY, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                       fillColor, borderColor, isSelected);

        if (render->font) {
            SDL_Surface* surface = TTF_RenderText_Solid(render->font, opt->label, textColor);
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(render->renderer, surface);
                if (texture) {
                    SDL_Rect textRect = {
                        buttonX + MENU_BUTTON_WIDTH / 2 - surface->w / 2,
                        buttonY + MENU_BUTTON_HEIGHT / 2 - surface->h / 2,
                        surface->w, surface->h
                    };
                    SDL_RenderCopy(render->renderer, texture, NULL, &textRect);
                    SDL_DestroyTexture(texture);
                }
                SDL_FreeSurface(surface);
            }
        }

        if (isSelected) {
            SDL_SetRenderDrawColor(render->renderer, 255, 255, 255, 255);
            int arrowX = buttonX - 20;
            int arrowY = buttonY + MENU_BUTTON_HEIGHT / 2;

            for (int j = 0; j < 8; j++) {
                SDL_RenderDrawLine(render->renderer, arrowX, arrowY - j, arrowX, arrowY + j);
                arrowX++;
            }
        }
    }

    if (render->font) {
        const char* instructions = "[Haut/Bas] Naviguer   [Entree/Espace] Valider";
        SDL_Color instrColor = {120, 120, 120, 255};
        SDL_Surface* surface = TTF_RenderText_Solid(render->font, instructions, instrColor);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(render->renderer, surface);
            if (texture) {
                SDL_Rect rect = {WINDOW_WIDTH / 2 - surface->w / 2, WINDOW_HEIGHT - 70,
                                surface->w, surface->h};
                SDL_RenderCopy(render->renderer, texture, NULL, &rect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }

    SDL_RenderPresent(render->renderer);
}
