/**
 * @file render.h
 * @brief Fonctions de rendu SDL (fenêtre, textures, texte)
 */

#ifndef NUPRC_RENDER_H
#define NUPRC_RENDER_H

#include "core.h"

/* Initialisation SDL */
void initSDL(void);
SDL_Window* createWindow(const char* name, int w, int h);
SDL_Renderer* createRenderer(SDL_Window* window);
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);

/* Rendu de base */
void updateDisplay(SDL_Renderer* renderer);
void clearRenderer(SDL_Renderer* renderer);
SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer);
void renderTexture(SDL_Texture* tex, SDL_Renderer* r, int x, int y, int w, int h);

/* Chargement de la carte */
SDL_Texture** loadTileTextures(const char* path, SDL_Renderer* renderer);
void loadWorldMap(const char* path, int map[GRID_WORLD_HEIGHT][GRID_WORLD_WIDTH]);
void loadBlockingMap(const char* path, char map[GRID_WORLD_HEIGHT][GRID_WORLD_WIDTH]);

/* Texte */
void printText(int x, int y, const char* text, int w, int h, SDL_Renderer* r);
void printTextWithFont(int x, int y, const char* text, TTF_Font* font, SDL_Renderer* r);

/* Audio */
void playMusic(const char* path);

#endif
