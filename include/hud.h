/**
 * @file hud.h
 * @brief Gestion de l'affichage du HUD (Heads-Up Display)
 *
 * Ce module gère l'affichage des informations du joueur à l'écran :
 * - Vies restantes
 * - Score actuel
 * - Nombre d'ennemis tués
 * - Temps de jeu
 * - Position dans le monde (salle actuelle)
 */

#ifndef NUPRC_HUD_H
#define NUPRC_HUD_H

#include "core.h"

//==============================================================================
// CONSTANTES HUD
//==============================================================================

/** Hauteur de la zone HUD en pixels */
#define HUD_HEIGHT              WINDOW_TEXTAREA_HEIGHT

/** Espacement entre les lignes de texte */
#define HUD_LINE_SPACING        25

/** Marges et positions du HUD */
#define HUD_MARGIN_LEFT         10
#define HUD_MARGIN_TOP          10
#define HUD_COLUMN_2_X          180
#define HUD_COLUMN_3_X          360
#define HUD_CONTROLS_X          550

/** Couleurs du HUD (RGB) */
#define HUD_BG_COLOR_R          40
#define HUD_BG_COLOR_G          40
#define HUD_BG_COLOR_B          40
#define HUD_BG_COLOR_A          255

#define HUD_SEPARATOR_COLOR_R   200
#define HUD_SEPARATOR_COLOR_G   200
#define HUD_SEPARATOR_COLOR_B   200
#define HUD_SEPARATOR_COLOR_A   255

//==============================================================================
// STRUCTURES
//==============================================================================

/**
 * @brief Configuration d'affichage du HUD
 *
 * Permet de personnaliser l'apparence du HUD
 */
typedef struct {
    SDL_Color backgroundColor;      /**< Couleur de fond du HUD */
    SDL_Color separatorColor;       /**< Couleur de la ligne de séparation */
    SDL_Color textColor;            /**< Couleur du texte */
    int       lineSpacing;          /**< Espacement entre les lignes */
    bool      showDebugInfo;        /**< Afficher les informations de debug */
} HUDConfig;

//==============================================================================
// FONCTIONS PUBLIQUES
//==============================================================================

/**
 * @brief Affiche le HUD complet à l'écran
 *
 * @param render État du système de rendu (fenêtre, renderer, font)
 * @param stats Statistiques du joueur (score, kills, temps)
 * @param lives Nombre de vies restantes
 * @param currentRoom Position actuelle dans le monde [x, y]
 */
void HUD_render(const RenderState* render, const PlayerStats* stats, int lives, const int currentRoom[2]);

/**
 * @brief Initialise la configuration par défaut du HUD
 *
 * @param config Pointeur vers la structure de configuration à initialiser
 */
void HUD_initConfig(HUDConfig* config);

/**
 * @brief Affiche le HUD avec une configuration personnalisée
 *
 * @param render État du système de rendu
 * @param stats Statistiques du joueur
 * @param lives Nombre de vies restantes
 * @param currentRoom Position actuelle dans le monde
 * @param config Configuration d'affichage personnalisée
 */
void HUD_renderWithConfig(const RenderState* render, const PlayerStats* stats,
                          int lives, const int currentRoom[2], const HUDConfig* config);

/**
 * @brief Affiche les informations de debug dans le HUD
 *
 * @param render État du système de rendu
 * @param fps Images par seconde actuelles
 * @param entityCount Nombre d'entités actives
 */
void HUD_renderDebugInfo(const RenderState* render, int fps, int entityCount);

/**
 * @brief Affiche un message temporaire sur le HUD
 *
 * @param render État du système de rendu
 * @param message Message à afficher
 * @param duration Durée d'affichage en frames
 */
void HUD_showMessage(const RenderState* render, const char* message, int duration);

/**
 * @brief Affiche une barre de vie graphique
 *
 * @param render État du système de rendu
 * @param x Position X de la barre
 * @param y Position Y de la barre
 * @param currentLives Vies actuelles
 * @param maxLives Vies maximales
 * @param width Largeur de la barre
 * @param height Hauteur de la barre
 */
void HUD_renderHealthBar(const RenderState* render, int x, int y,
                         int currentLives, int maxLives, int width, int height);

#endif // NUPRC_HUD_H
