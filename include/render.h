/**
 * @file render.h
 * @brief Fonctions de rendu SDL (fenêtre, textures, texte)
 */

#ifndef NUPRC_RENDER_H
#define NUPRC_RENDER_H

#include "core.h"

//==============================================================================
// FONCTIONS - INITIALISATION SDL
//==============================================================================

/** Initialise la bibliothèque SDL et ses modules */
void initSDL(void);

/** Crée une fenêtre SDL centrée à l'écran */
SDL_Window* createWindow(const char* windowName, int windowWidth, int windowHeight);

/** Crée un moteur de rendu SDL */
SDL_Renderer* createRenderer(SDL_Window* window);

/** Libère les ressources SDL (fenêtre, renderer) */
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);

//==============================================================================
// FONCTIONS - RENDU DE BASE
//==============================================================================

/** Met à jour l'affichage de l'écran */
void updateDisplay(SDL_Renderer* renderer);

/** Efface le contenu du moteur de rendu */
void clearRenderer(SDL_Renderer* renderer);

/** Charge une texture depuis un fichier image BMP */
SDL_Texture* loadTexture(const char* filePath, SDL_Renderer* renderer);

/** Affiche une texture à une position donnée */
void renderTexture(SDL_Texture* texture, SDL_Renderer* renderer,
                   int x, int y, int width, int height);

//==============================================================================
// FONCTIONS - CARTE
//==============================================================================

/** Charge toutes les textures de tuiles depuis un fichier d'atlas */
SDL_Texture** loadTileTextures(const char* tileFilename, SDL_Renderer* renderer);

/** Charge la carte du monde depuis un fichier texte */
void loadWorldMap(const char* filePath, int worldMap[GRID_WORLD_HEIGHT][GRID_WORLD_WIDTH]);

/** Charge la carte de blocage depuis un fichier texte */
void loadBlockingMap(const char* filePath, char blockingMap[GRID_WORLD_HEIGHT][GRID_WORLD_WIDTH]);

//==============================================================================
// FONCTIONS - TEXTE
//==============================================================================

/** Affiche un texte avec taille personnalisée (ouvre/ferme la police) */
void printText(int x, int y, const char* text, int width, int height, SDL_Renderer* renderer);

/** Affiche un texte avec une police déjà chargée (plus efficace) */
void printTextWithFont(int x, int y, const char* text, TTF_Font* font, SDL_Renderer* renderer);

//==============================================================================
// FONCTIONS - AUDIO
//==============================================================================

/** Joue une musique au format MP3 */
void playMusic(const char* filePath);

#endif // NUPRC_RENDER_H

