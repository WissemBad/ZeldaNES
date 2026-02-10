#ifndef NUPRC_AUDIO_H
#define NUPRC_AUDIO_H

#include "core.h"

typedef enum {
    AUDIO_SFX_ENEMY_KILLED,
    AUDIO_SFX_WALK,
    AUDIO_SFX_MENU_CLICK,
    AUDIO_SFX_MENU_MOVE,
    AUDIO_SFX_ATTACK,
    AUDIO_SFX_PLAYER_HIT,
    AUDIO_SFX_GAME_OVER,
    AUDIO_SFX_COUNT
} AudioSfxEvent;

typedef enum {
    AUDIO_MUSIC_MENU,
    AUDIO_MUSIC_GAMEPLAY,
    AUDIO_MUSIC_GAMEOVER,
    AUDIO_MUSIC_COUNT
} AudioMusicTrack;

bool Audio_init(const char* configPath);
void Audio_shutdown(void);
void Audio_playSfx(AudioSfxEvent event);
void Audio_updateWalk(bool isMoving);
void Audio_setMusicTrack(AudioMusicTrack track);
void Audio_stopMusic(void);

#endif
