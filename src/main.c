//
// Created by Wissem. on 13 Jan 2026.
//

// --- Bibliothèques --- //
#include "character.h"
#include "tray.h"
#include "settings.h"



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "IOManager.h"


// --- Fonctions Utilitaires --- //
// Détection de la fin du jeu
bool isEnd(Character c, int endPos[2], bool *win);
bool isEnd(Character c, int endPos[2], bool *win) {
    if (c.move_count >= SETTINGS_MAX_MOVE) { *win = false; return true;  }
    if (c.pos[0] == endPos[0] && c.pos[1] == endPos[1]) { *win = true; return true; }
    return false;
}

// Détection des bords du plateau
bool isExceed(Character c, Direction dir);
bool isExceed(Character c, Direction dir)
{
    if (dir == DIR_UP && c.pos[1] == 0) { return true; }
    if (dir == DIR_DOWN && c.pos[1] == SETTINGS_TRAY_HEIGHT - 1) { return true; }
    if (dir == DIR_LEFT && c.pos[0] == 0) { return true; }
    if (dir == DIR_RIGHT && c.pos[0] == SETTINGS_TRAY_WIDTH - 1) { return true; }
    return false;
}

// --- Fonction d'entrée --- //
int main(int argc, char *argv[]) {
    int largeurCase = 50, hauteurCase = 50;

    initSDL();
    initTTF();

    SDL_Window *win = createWindow("ME_NUPRC", SETTINGS_TRAY_WIDTH * largeurCase, SETTINGS_TRAY_HEIGHT * hauteurCase);
    SDL_Renderer *ren = createRenderer(win);

    SDL_Texture *texPerso = loadTexture("assets/images/perso.bmp", ren);
    SDL_Texture *texArrivee = loadTexture("assets/images/arrivee.bmp", ren);

    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = 0;
        }

        clearRenderer(ren);

        if (texPerso) {
            SDL_Rect dst = {50, 50, largeurCase, hauteurCase};
            SDL_RenderCopy(ren, texPerso, NULL, &dst);
        }
        if (texArrivee) {
            SDL_Rect dst2 = {150, 50, largeurCase, hauteurCase};
            SDL_RenderCopy(ren, texArrivee, NULL, &dst2);
        }

        updateDisplay(ren);
        SDL_Delay(16);
    }

    if (texPerso) SDL_DestroyTexture(texPerso);
    if (texArrivee) SDL_DestroyTexture(texArrivee);
    QuitSDL(win, ren);
    return 0;
}
