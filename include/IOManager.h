#ifndef IOMANAGER_H_
#define IOMANAGER_H_

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "settings.h"

/**
 * @brief Initialise la SDL
 */
void IO_initSDL(void);

/**
 * @brief Créé une fenêtre SDL
 * @param windowName Nom de la fenêtre
 * @param windowWidth Largeur de la fenêtre
 * @param windowHeight Hauteur de la fenêtre
 */
SDL_Window* IO_createWindow(char windowName[], int windowWidth, int windowHeight);

/**
 * @brief Créé un renderer
 * @param window La fenêtre SDL
 */
SDL_Renderer* IO_createRenderer(SDL_Window *window);

/**
 * @brief Charge une texture dans le rendu graphique
 * @param filePath Le chemin vers la texture
 * @param renderer Un pointeur vers la structure contenant le rendu
 */
SDL_Texture* IO_loadTexture(const char* filePath, SDL_Renderer *renderer);

/**
 * @brief Met la texture dans le rendu graphique à une position (x,y) dans le repère écran avec une taille donnée
 * @param texture La texture à mettre dans le rendu
 * @param renderer Un pointeur vers la structure contenant le rendu
 * @param x La coordonnée x dans le repère écran
 * @param y La coordonnée y dans le repère écran
 * @param width Largeur de la texture
 * @param height Hauteur de la texture
 */
void IO_renderTexture(SDL_Texture *texture, SDL_Renderer *renderer, int x, int y, int width, int height);

/**
 * @brief Initialise la bibliothèque TTF
 */
void IO_initTTF(void);

/**
 * @brief Créé une police de caractère avec une taille de police donnée en paramètre
 * @param fontPath Le chemin vers le fichier .ttf contenant la font
 * @param fontSize La taille de la font
 */
TTF_Font* IO_createFont(char fontPath[], int fontSize);

/**
 * @brief Met dans le rendu graphique un texte à la position (x,y) dans le repère écran avec une taille donnée
 * @param x La coordonnée x dans le repère écran
 * @param y La coordonnée y dans le repère écran
 * @param text Le texte à afficher
 * @param width La largeur du message sur l'écran
 * @param height La hauteur du message sur l'écran
 * @param font La police utilisée
 * @param color La SDL_Color utilisée
 * @param renderer Un pointeur vers une structure qui contient l'état du rendu graphique
 */
void IO_printText(int x, int y, char *text, int width, int height, TTF_Font* font, SDL_Color color, SDL_Renderer *renderer);

/**
 * @brief Met à jour l'affichage de l'écran
 * @param renderer Un pointeur sur une structure contenant l'état du rendu
 */
void IO_updateDisplay(SDL_Renderer *renderer);

/**
 * @brief Vide le rendu graphique
 * @param renderer Un pointeur sur une structure contenant l'état du rendu
 */
void IO_clearRenderer(SDL_Renderer *renderer);

/**
 * @brief Gestion du clavier
 * @return Une Direction (DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT) ou -1 si aucune touche n'a été pressée
 */
Direction IO_inputDirection(void);

/**
 * @brief Libère la mémoire de la fenêtre et du renderer
 * @param window Un pointeur sur une structure contenant la fenêtre
 * @param renderer Un pointeur sur une structure contenant l'état du rendu
 */
void IO_quit(SDL_Window *window, SDL_Renderer *renderer);

#endif

