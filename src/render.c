#include "render.h"
#include "assets.h"

void initSDL(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "Erreur d'initialisation de SDL_ttf : %s\n", TTF_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}

SDL_Window* createWindow(const char* windowName, const int windowWidth, const int windowHeight) {
    SDL_Window* window = SDL_CreateWindow(
        windowName,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth, windowHeight,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL) {
        fprintf(stderr, "Erreur SDL_CreateWindow : %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    return window;
}

SDL_Renderer* createRenderer(SDL_Window* window) {
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (renderer == NULL) {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    return renderer;
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer) {
    if (renderer != NULL) {
        SDL_DestroyRenderer(renderer);
    }
    if (window != NULL) {
        SDL_DestroyWindow(window);
    }
    TTF_Quit();
    SDL_Quit();
}

void updateDisplay(SDL_Renderer* renderer) {
    SDL_RenderPresent(renderer);
}

void clearRenderer(SDL_Renderer* renderer) {
    SDL_RenderClear(renderer);
}

SDL_Texture* loadTexture(const char* filePath, SDL_Renderer* renderer) {
    const char* resolvedPath = asset_full(filePath);
    SDL_Surface* surface = SDL_LoadBMP(resolvedPath);
    if (surface == NULL) {
        fprintf(stderr, "Erreur LoadBMP (%s) : %s\n", resolvedPath, SDL_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture == NULL) {
        fprintf(stderr, "Erreur CreateTextureFromSurface : %s\n", SDL_GetError());
    }

    return texture;
}

void renderTexture(SDL_Texture* texture, SDL_Renderer* renderer,
                   const int x, const int y, const int width, const int height) {
    SDL_Rect dst = { x, y, width, height };
    SDL_RenderCopy(renderer, texture, NULL, &dst);
}

SDL_Texture** loadTileTextures(const char* tileFilename, SDL_Renderer* renderer) {
    const char* resolvedPath = asset_full(tileFilename);
    SDL_Surface* atlas = SDL_LoadBMP(resolvedPath);
    if (atlas == NULL) {
        fprintf(stderr, "Erreur LoadBMP tiles (%s) : %s\n", resolvedPath, SDL_GetError());
        return NULL;
    }

    const int tileCount = MAP_TILES_WIDTH * MAP_TILES_HEIGHT;
    SDL_Texture** textures = malloc(tileCount * sizeof(SDL_Texture*));
    if (textures == NULL) {
        SDL_FreeSurface(atlas);
        return NULL;
    }

    int index = 0;
    for (int row = 0; row < MAP_TILES_HEIGHT; row++) {
        for (int col = 0; col < MAP_TILES_WIDTH; col++) {

            SDL_Surface* tileSurface = SDL_CreateRGBSurface(
                0, MAP_TILE_SIZE, MAP_TILE_SIZE, 32, 0, 0, 0, 0
            );
            if (tileSurface == NULL) {
                fprintf(stderr, "Erreur SDL_CreateRGBSurface pour tuile (%d,%d) : %s\n", row, col, SDL_GetError());
                textures[index] = NULL;
                index++;
                continue;
            }

            SDL_Rect srcRect = {
                .x = col * (MAP_TILE_SIZE + 1) + 1,
                .y = row * (MAP_TILE_SIZE + 1) + 1,
                .w = MAP_TILE_SIZE,
                .h = MAP_TILE_SIZE
            };

            if (SDL_BlitSurface(atlas, &srcRect, tileSurface, NULL) < 0) {
                fprintf(stderr, "Erreur SDL_BlitSurface pour tuile (%d,%d) : %s\n", row, col, SDL_GetError());
                SDL_FreeSurface(tileSurface);
                textures[index] = NULL;
                index++;
                continue;
            }

            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tileSurface);
            SDL_FreeSurface(tileSurface);
            if (tex == NULL) {
                fprintf(stderr, "Erreur CreateTextureFromSurface pour tuile (%d,%d) : %s\n", row, col, SDL_GetError());
            }
            textures[index] = tex;
            index++;
        }
    }

    SDL_FreeSurface(atlas);
    return textures;
}

void loadWorldMap(const char* filePath, int worldMap[GRID_WORLD_HEIGHT][GRID_WORLD_WIDTH]) {
    const char* resolvedPath = asset_full(filePath);
    FILE* file = fopen(resolvedPath, "r");
    if (file == NULL) {
        fprintf(stderr, "Erreur ouverture fichier : %s\n", resolvedPath);
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    int row = 0;

    while (row < GRID_WORLD_HEIGHT && fgets(buffer, sizeof(buffer), file) != NULL) {
        int col = 0;
        char* token = strtok(buffer, " \t\r\n");

        while (token != NULL && col < GRID_WORLD_WIDTH) {
            worldMap[row][col] = (int)strtol(token, NULL, 16);
            token = strtok(NULL, " \t\r\n");
            col++;
        }
        row++;
    }

    fclose(file);
}

void loadBlockingMap(const char* filePath, char blockingMap[GRID_WORLD_HEIGHT][GRID_WORLD_WIDTH]) {
    const char* resolvedPath = asset_full(filePath);
    FILE* file = fopen(resolvedPath, "r");
    if (file == NULL) {
        fprintf(stderr, "Erreur ouverture fichier : %s\n", resolvedPath);
        exit(EXIT_FAILURE);
    }

    char buffer[512];
    int row = 0;

    while (row < GRID_WORLD_HEIGHT && fgets(buffer, sizeof(buffer), file) != NULL) {
        for (int col = 0; col < GRID_WORLD_WIDTH && buffer[col] != '\0' && buffer[col] != '\n'; col++) {
            blockingMap[row][col] = buffer[col];
        }
        row++;
    }

    fclose(file);
}

void printText(const int x, const int y, const char* text,
               const int width, const int height, SDL_Renderer* renderer) {
    TTF_Font* font = TTF_OpenFont(asset_full(WINDOW_FONT_PATH), WINDOW_FONT_SIZE);
    if (font == NULL) {
        fprintf(stderr, "Erreur chargement police : %s\n", TTF_GetError());
        return;
    }

    SDL_Color color = {255, 255, 0, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (surface == NULL) {
        TTF_CloseFont(font);
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture != NULL) {
        SDL_Rect rect = { x, y, width, height };
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }

    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
}

void printTextWithFont(int x, int y, const char* text, TTF_Font* font, SDL_Renderer* renderer) {
    if (font == NULL || text == NULL || renderer == NULL) return;

    SDL_Color color = {255, 255, 0, 255};
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text, color, 400);
    if (surface == NULL) return;


    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture != NULL) {
        SDL_Rect rect = { x, y, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }

    SDL_FreeSurface(surface);
}
