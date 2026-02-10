#ifndef NUPRC_RENDER_H
#define NUPRC_RENDER_H

#include "core.h"

void initSDL(void);
SDL_Window* createWindow(const char* name, int w, int h);
SDL_Renderer* createRenderer(SDL_Window* window);
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);

void updateDisplay(SDL_Renderer* renderer);
void clearRenderer(SDL_Renderer* renderer);
SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer);
void renderTexture(SDL_Texture* tex, SDL_Renderer* r, int x, int y, int w, int h);

SDL_Texture** loadTileTextures(const char* path, SDL_Renderer* renderer);
void loadWorldMap(const char* path, int map[GRID_WORLD_HEIGHT][GRID_WORLD_WIDTH]);
void loadBlockingMap(const char* path, char map[GRID_WORLD_HEIGHT][GRID_WORLD_WIDTH]);

void printText(int x, int y, const char* text, int w, int h, SDL_Renderer* r);
void printTextWithFont(int x, int y, const char* text, TTF_Font* font, SDL_Renderer* r);

#endif
