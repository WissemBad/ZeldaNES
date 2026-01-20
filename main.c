//
// Created by Wissem. on 13 Jan 2026.
//
// --- Bibliothèques --- //
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// --- Variables Globales --- //
#define WIDTH 10
#define HEIGHT 10

#define MAX_MOVE 20

#define CHAR_PLAYER 'X'
#define CHAR_EMPTY ' '
#define CHAR_END 'A'


// --- Gestion des Types --- //
// Structure du personnage
struct cStruct {
    char* name;
    int pos[2]; // x, y
    int move_count;
} typedef Character;

// Énumération des types
enum dirs { UP, DOWN, LEFT, RIGHT } typedef Direction;
enum bools { FALSE, TRUE } typedef Bool;


// --- Gestion du Personnage --- //
// Initialisation d'un personnage
Character initCharacter(char* name);
Character initCharacter(char* name) {
    Character c;
    c.name = name;
    c.pos[0] = 0;
    c.pos[1] = 0;
    c.move_count = 0;
    return c;
}

// Déplacement d'un personnage
Character moveCharacter(Character c, Direction dir);
Character moveCharacter(Character c, Direction dir) {
    switch (dir) {
        case RIGHT:
            c.pos[0] += 1;
            c.move_count++;
            break;
        case LEFT:
            c.pos[0] -= 1;
            c.move_count++;
            break;
        case UP:
            c.pos[1] -= 1;
            c.move_count++;
            break;
        case DOWN:
            c.pos[1] += 1;
            c.move_count++;
            break;
        default:
            break;
    }
    return c;
}

// Affichage d'un personnage
void showCharacter(Character c);
void showCharacter(Character c) {
    printf("-- Informations du personnage --\n");
    printf("Nom : %s\n", c.name);
    printf("Position (x, y) : %d, %d\n", c.pos[0], c.pos[1]);
    printf("Déplacements : %d\n", c.move_count);
    printf("--------------------------------\n");
}


// --- Gestion du Plateau --- //
// Initialisation du plateau
void initTray(char tray[HEIGHT][WIDTH], Character c, int endPos[2]);
void initTray(char tray[HEIGHT][WIDTH], Character c, int endPos[2]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            tray[i][j] = CHAR_EMPTY;
        }
    }

    tray[c.pos[1]][c.pos[0]] = CHAR_PLAYER;
    tray[endPos[1]][endPos[0]] = CHAR_END;
}

// Affichage du plateau
void showTray(char tray[HEIGHT][WIDTH]);
void showTray(char tray[HEIGHT][WIDTH]) {
    printf("-- Tableau de jeu --\n");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%c", tray[i][j]);
        }
        putchar('\n');
    }
    printf("--------------------\n");
}

// Mise à jour du plateau
void updateTray(char tray[HEIGHT][WIDTH], Character c, Direction dir);
void updateTray(char tray[HEIGHT][WIDTH], Character c, Direction dir) {
    switch (dir) {
        case UP:
            tray[c.pos[1]][c.pos[0]] = CHAR_PLAYER;
            tray[c.pos[1]+1][c.pos[0]] = '|';
            break;
        case DOWN:
            tray[c.pos[1]][c.pos[0]] = CHAR_PLAYER;
            tray[c.pos[1]-1][c.pos[0]] = '|';
            break;
        case LEFT:
            tray[c.pos[1]][c.pos[0]] = CHAR_PLAYER;
            tray[c.pos[1]][c.pos[0]+1] = '-';
            break;
        case RIGHT:
            tray[c.pos[1]][c.pos[0]] = CHAR_PLAYER;
            tray[c.pos[1]][c.pos[0]-1] = '-';
            break;
        default:
            break;
    }
}


// --- Fonctions Utilitaires --- //
// Détection de la fin du jeu
Bool isEnd(Character c, int endPos[2], Bool *win);
Bool isEnd(Character c, int endPos[2], Bool *win) {
    if (c.move_count >= MAX_MOVE) { *win = FALSE; return TRUE;  }
    if (c.pos[0] == endPos[0] && c.pos[1] == endPos[1]) { *win = TRUE; return TRUE; }
    return FALSE;
}

// Détection des bords du plateau
Bool isExceed(Character c, Direction dir);
Bool isExceed(Character c, Direction dir) {
    if (dir == UP && c.pos[1] == 0) { return TRUE; }
    if (dir == DOWN && c.pos[1] == HEIGHT-1) { return TRUE; }
    if (dir == LEFT && c.pos[0] == 0) { return TRUE; }
    if (dir == RIGHT && c.pos[0] == WIDTH-1) { return TRUE; }
    return FALSE;
}


// --- Fonction d'entrée --- //
int main(void) {
    Bool win = FALSE;
    char userInput;
    int endPos[] = { rand() % HEIGHT, rand() % WIDTH };

    Character myChar = initCharacter("Wissem.");
    char tray[HEIGHT][WIDTH];
    initTray(tray, myChar, endPos);

    while (!isEnd(myChar, endPos, &win)) {
        showCharacter(myChar);
        showTray(tray);
        printf("Que voulez-vous faire ? (D/U/L/R)\n-> ");
        scanf("%c", &userInput);
        putchar('\n');

        Direction dir;
        if (userInput == 'D') { dir = DOWN; }
        else if (userInput == 'U') { dir = UP; }
        else if (userInput == 'L') { dir = LEFT; }
        else if (userInput == 'R') { dir = RIGHT; }
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