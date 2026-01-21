#ifndef CHARACTER_H
#define CHARACTER_H

#include <SDL2/SDL.h>
#include "settings.h"

/**
 * @brief Données stockées pour représenter un personnage.
 */
typedef struct characterStruct {
    char* name;
    int position[2];
    int moveCount;
    SDL_Texture* texture;
} Character;

/**
 * @brief Crée un personnage à partir d'un nom et d'une texture.
 * @param name Nom assigné au personnage
 * @param texturePath Chemin relatif vers la texture SDL à charger
 * @param renderer Renderer SDL utilisé pour créer la texture
 * @return Le personnage initialisé avec sa texture
 */
Character Character_init(char* name, char* texturePath, SDL_Renderer *renderer);

/**
 * @brief Met à jour la position du personnage selon une direction.
 * @param character Personnage à déplacer
 * @param direction Direction dans laquelle se déplacer
 * @return Le personnage après sa mise à jour
 */
Character Character_move(Character character, Direction direction);

/**
 * @brief Affiche les données du personnage dans la console de débogage.
 * @deprecated Utiliser une interface graphique pour afficher les informations.
 * @param character Personnage à afficher
 */
void Character_print(Character character);

/**
 * @brief Dessine le personnage à sa position actuelle sur le renderer.
 * @param character Personnage à dessiner
 * @param renderer Renderer SDL utilisé pour le dessin
 */
void Character_draw(Character character, SDL_Renderer *renderer);

/**
 * @brief Libère les ressources du personnage.
 * @param character Personnage à détruire
 */
void Character_destroy(Character *character);

#endif