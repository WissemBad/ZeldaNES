#include "IOManager.h"
#include "settings.h"

void IO_initSDL(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation de la SDL : %s", SDL_GetError());
        SDL_Quit();
    }
}

SDL_Window* IO_createWindow(char windowName[], int windowWidth, int windowHeight) {
    SDL_Window *window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, windowWidth,
                                          windowHeight, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("SDL_CreateWindow Error\n");
        SDL_Quit();
    }
    return window;
}

SDL_Renderer* IO_createRenderer(SDL_Window *window) {
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer Error");
        SDL_Quit();
    }
    return renderer;
}

SDL_Texture* IO_loadTexture(const char* filePath, SDL_Renderer *renderer) {
    SDL_Texture *texture = NULL;
    SDL_Surface *loadedImage = SDL_LoadBMP(filePath);
    if (loadedImage != NULL) {
        texture = SDL_CreateTextureFromSurface(renderer, loadedImage);
        SDL_FreeSurface(loadedImage);
        if (texture == NULL) {
            printf("CreateTextureFromSurface Error : %s\n", SDL_GetError());
        }
    } else {
        printf("LoadBMP Error : %s\n", SDL_GetError());
    }
    return texture;
}

void IO_renderTexture(SDL_Texture *texture, SDL_Renderer *renderer, int x, int y, int width, int height) {
    SDL_Rect destination;
    destination.x = x;
    destination.y = y;
    destination.w = width;
    destination.h = height;
    SDL_RenderCopy(renderer, texture, NULL, &destination);
}

void IO_initTTF(void) {
    if (TTF_Init() == -1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
    }
}

TTF_Font* IO_createFont(char fontPath[], int fontSize) {
    TTF_Font* font = TTF_OpenFont(fontPath, fontSize);
    if (font == NULL) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        SDL_Quit();
    }
    return font;
}

void IO_printText(int x, int y, char *text, int width, int height, TTF_Font* font, SDL_Color color, SDL_Renderer *renderer) {
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect messageRect;
    messageRect.x = x;
    messageRect.y = y;
    messageRect.w = width;
    messageRect.h = height;
    SDL_RenderCopy(renderer, message, NULL, &messageRect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

void IO_updateDisplay(SDL_Renderer *renderer) {
    SDL_RenderPresent(renderer);
}

void IO_clearRenderer(SDL_Renderer *renderer) {
    SDL_RenderClear(renderer);
}

Direction inputDirection(void) {
    int direction = -1;
    SDL_Event event;
    int key;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                key = event.key.keysym.sym;
                switch (key) {
                    case SDLK_LEFT:
                        direction = DIR_LEFT;
                        break;
                    case SDLK_RIGHT:
                        direction = DIR_RIGHT;
                        break;
                    case SDLK_UP:
                        direction = DIR_UP;
                        break;
                    case SDLK_DOWN:
                        direction = DIR_DOWN;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_QUIT:
                direction = -2;
                break;
        }
    }
    return direction;
}

void IO_quit(SDL_Window *window, SDL_Renderer *renderer) {
    if (window) SDL_DestroyWindow(window);
    if (renderer) SDL_DestroyRenderer(renderer);
    SDL_Quit();
}
