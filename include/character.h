//
// Created by Wissem.
// Character Management Headers
//

#ifndef CHARACTER_H
#define CHARACTER_H

#include "settings.h"

// Structure du personnage
struct cStruct {
    char* name;
    int pos[2];
    int move_count;
} typedef Character;

// Initialisation d'un personnage
Character initCharacter(char* name);

// Déplacement d'un personnage
Character moveCharacter(Character c, Direction dir);

// Affichage d'un personnage
void showCharacter(Character c);

#endif