//
// Created by Wissem.
// Character Management
//

#include "character.h"
#include <stdio.h>

// Initialisation d'un personnage
Character initCharacter(char* name) {
    Character c;
    c.name = name;
    c.pos[0] = 0;
    c.pos[1] = 0;
    c.move_count = 0;
    return c;
}

// Déplacement d'un personnage
Character moveCharacter(Character c, Direction dir) {
    switch (dir) {
        case DIR_RIGHT:
            c.pos[0] += 1;
            c.move_count++;
            break;
        case DIR_LEFT:
            c.pos[0] -= 1;
            c.move_count++;
            break;
        case DIR_UP:
            c.pos[1] -= 1;
            c.move_count++;
            break;
        case DIR_DOWN:
            c.pos[1] += 1;
            c.move_count++;
            break;
        default:
            break;
    }
    return c;
}

// Affichage d'un personnage
void showCharacter(Character c) {
    printf("-- Informations du personnage --\n");
    printf("Nom : %s\n", c.name);
    printf("Position (x, y) : %d, %d\n", c.pos[0], c.pos[1]);
    printf("Déplacements : %d\n", c.move_count);
    printf("--------------------------------\n");
}
