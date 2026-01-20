//
// Created by Wissem.
// Tray Management Headers
//

#ifndef TRAY_H
#define TRAY_H

#include "tray.h"
#include "character.h"
#include "settings.h"


// -- Initialisation du plateau --
void initTray(char tray[SETTINGS_TRAY_HEIGHT][SETTINGS_TRAY_WIDTH], Character c, int endPos[2]);

// -- Affichage du plateau --
void showTray(char tray[SETTINGS_TRAY_HEIGHT][SETTINGS_TRAY_WIDTH]);

// -- Mise à jour du plateau --
void updateTray(char tray[SETTINGS_TRAY_HEIGHT][SETTINGS_TRAY_WIDTH], Character c, Direction dir);

#endif