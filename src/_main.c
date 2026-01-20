//
// Created by Wissem. on 13 Jan 2026.
//

// --- Bibliothèques --- //
#include "character.h"
#include "tray.h"
#include "settings.h"



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>


// --- Fonctions Utilitaires --- //
// Détection de la fin du jeu
bool isEnd(Character c, int endPos[2], bool *win);
bool isEnd(Character c, int endPos[2], bool *win) {
    if (c.move_count >= SETTINGS_MAX_MOVE) { *win = false; return true;  }
    if (c.pos[0] == endPos[0] && c.pos[1] == endPos[1]) { *win = true; return true; }
    return false;
}

// Détection des bords du plateau
bool isExceed(Character c, Direction dir);
bool isExceed(Character c, Direction dir) {
    if (dir == DIR_UP && c.pos[1] == 0) { return true; }
    if (dir == DIR_DOWN && c.pos[1] == SETTINGS_TRAY_HEIGHT - 1) { return true; }
    if (dir == DIR_LEFT && c.pos[0] == 0) { return true; }
    if (dir == DIR_RIGHT && c.pos[0] == SETTINGS_TRAY_WIDTH - 1) { return true; }
    return false;
}


// --- Fonction d'entrée --- //
int main(void) {
    bool win = false;
    char userInput;
    int endPos[] = { rand() % SETTINGS_TRAY_HEIGHT, rand() % SETTINGS_TRAY_WIDTH };

    Character myChar = initCharacter("Wissem.");
    char tray[SETTINGS_TRAY_HEIGHT][SETTINGS_TRAY_WIDTH];
    initTray(tray, myChar, endPos);

    while (!isEnd(myChar, endPos, &win)) {
        showCharacter(myChar);
        showTray(tray);
        printf("Que voulez-vous faire ? (D/U/L/R)\n-> ");
        scanf("%c", &userInput);
        putchar('\n');

        Direction dir;
        if (userInput == 'D') { dir = DIR_DOWN; }
        else if (userInput == 'U') { dir = DIR_UP; }
        else if (userInput == 'L') { dir = DIR_LEFT; }
        else if (userInput == 'R') { dir = DIR_RIGHT; }
        else { dir = '0'; }

        if (!isExceed(myChar, dir)) {
            myChar = moveCharacter(myChar, dir);
            updateTray(tray, myChar, dir);
        } else {
            printf("Impossible d'effectuer cette opération car vous êtes au bord du plateau...\n");
            sleep(3);
        }
        system("clear");
    }

    if (win) { printf("Félicitations, vous avez gagné !\n"); }
    else { printf("Malheureusement, vous avez perdu.\n"); }
    showCharacter(myChar);
    return 0;
}