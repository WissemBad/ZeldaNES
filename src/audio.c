#include "audio.h"
#include "assets.h"

#include <ctype.h>

typedef struct {
    char sfxEnemyKilled[256];
    char sfxWalk[256];
    char sfxMenuClick[256];
    char sfxMenuMove[256];
    char sfxAttack[256];
    char sfxPlayerHit[256];
    char sfxGameOver[256];
    char musicMenu[256];
    char musicGameplay[256];
    char musicGameOver[256];
    int masterVolume;
    int musicVolume;
    int sfxVolume;
    int walkIntervalFrames;
} AudioConfig;

typedef struct {
    bool initialized;
    AudioConfig config;
    Mix_Chunk* sfx[AUDIO_SFX_COUNT];
    Mix_Music* music[AUDIO_MUSIC_COUNT];
    AudioMusicTrack currentTrack;
    bool walkWasMoving;
    int walkTimer;
} AudioState;

static AudioState g_audio = {0};

static void trim(char* text) {
    if (!text) return;

    char* start = text;
    while (*start != '\0' && isspace((unsigned char)*start)) start++;
    if (start != text) memmove(text, start, strlen(start) + 1);

    int end = (int)strlen(text) - 1;
    while (end >= 0 && isspace((unsigned char)text[end])) {
        text[end] = '\0';
        end--;
    }
}

static int clampInt(int value, int minValue, int maxValue) {
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

static void copyValue(char* dst, size_t dstSize, const char* value) {
    if (!dst || dstSize == 0) return;
    if (!value) {
        dst[0] = '\0';
        return;
    }
    strncpy(dst, value, dstSize - 1);
    dst[dstSize - 1] = '\0';
}

static void setDefaultConfig(AudioConfig* cfg) {
    memset(cfg, 0, sizeof(*cfg));
    copyValue(cfg->musicMenu, sizeof(cfg->musicMenu), "sounds/music/menu.ogg");
    copyValue(cfg->musicGameplay, sizeof(cfg->musicGameplay), "sounds/music/gameplay.ogg");
    copyValue(cfg->musicGameOver, sizeof(cfg->musicGameOver), "sounds/music/gameover.ogg");
    copyValue(cfg->sfxEnemyKilled, sizeof(cfg->sfxEnemyKilled), "sounds/sfx/enemy_killed.wav");
    copyValue(cfg->sfxWalk, sizeof(cfg->sfxWalk), "sounds/sfx/walk.wav");
    copyValue(cfg->sfxMenuClick, sizeof(cfg->sfxMenuClick), "sounds/sfx/menu_click.wav");
    copyValue(cfg->sfxMenuMove, sizeof(cfg->sfxMenuMove), "sounds/sfx/menu_move.wav");
    copyValue(cfg->sfxAttack, sizeof(cfg->sfxAttack), "sounds/sfx/attack.wav");
    copyValue(cfg->sfxPlayerHit, sizeof(cfg->sfxPlayerHit), "sounds/sfx/player_hit.wav");
    copyValue(cfg->sfxGameOver, sizeof(cfg->sfxGameOver), "sounds/sfx/game_over.wav");
    cfg->masterVolume = 128;
    cfg->musicVolume = 96;
    cfg->sfxVolume = 128;
    cfg->walkIntervalFrames = 9;
}

static void applyConfigEntry(AudioConfig* cfg, const char* key, const char* value) {
    if (strcmp(key, "MUSIC_MENU") == 0) copyValue(cfg->musicMenu, sizeof(cfg->musicMenu), value);
    else if (strcmp(key, "MUSIC_GAMEPLAY") == 0) copyValue(cfg->musicGameplay, sizeof(cfg->musicGameplay), value);
    else if (strcmp(key, "MUSIC_GAMEOVER") == 0) copyValue(cfg->musicGameOver, sizeof(cfg->musicGameOver), value);
    else if (strcmp(key, "SFX_ENEMY_KILLED") == 0) copyValue(cfg->sfxEnemyKilled, sizeof(cfg->sfxEnemyKilled), value);
    else if (strcmp(key, "SFX_WALK") == 0) copyValue(cfg->sfxWalk, sizeof(cfg->sfxWalk), value);
    else if (strcmp(key, "SFX_MENU_CLICK") == 0) copyValue(cfg->sfxMenuClick, sizeof(cfg->sfxMenuClick), value);
    else if (strcmp(key, "SFX_MENU_MOVE") == 0) copyValue(cfg->sfxMenuMove, sizeof(cfg->sfxMenuMove), value);
    else if (strcmp(key, "SFX_ATTACK") == 0) copyValue(cfg->sfxAttack, sizeof(cfg->sfxAttack), value);
    else if (strcmp(key, "SFX_PLAYER_HIT") == 0) copyValue(cfg->sfxPlayerHit, sizeof(cfg->sfxPlayerHit), value);
    else if (strcmp(key, "SFX_GAME_OVER") == 0) copyValue(cfg->sfxGameOver, sizeof(cfg->sfxGameOver), value);
    else if (strcmp(key, "MASTER_VOLUME") == 0) cfg->masterVolume = atoi(value);
    else if (strcmp(key, "MUSIC_VOLUME") == 0) cfg->musicVolume = atoi(value);
    else if (strcmp(key, "SFX_VOLUME") == 0) cfg->sfxVolume = atoi(value);
    else if (strcmp(key, "WALK_INTERVAL_FRAMES") == 0) cfg->walkIntervalFrames = atoi(value);
}

static void loadConfigFile(AudioConfig* cfg, const char* path) {
    const char* resolvedPath = asset_full(path);
    FILE* file = fopen(resolvedPath, "r");
    if (!file) {
        fprintf(stderr, "Audio config introuvable: %s\n", resolvedPath);
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        trim(line);
        if (line[0] == '\0' || line[0] == '#') continue;

        char* equal = strchr(line, '=');
        if (!equal) continue;

        *equal = '\0';
        char* key = line;
        char* value = equal + 1;
        trim(key);
        trim(value);
        applyConfigEntry(cfg, key, value);
    }

    fclose(file);
}

static Mix_Chunk* loadChunk(const char* path) {
    if (!path || path[0] == '\0') return NULL;
    const char* resolvedPath = asset_full(path);
    Mix_Chunk* chunk = Mix_LoadWAV(resolvedPath);
    if (!chunk) {
        fprintf(stderr, "SFX non charge: %s (%s)\n", resolvedPath, Mix_GetError());
    }
    return chunk;
}

static Mix_Music* loadMusic(const char* path) {
    if (!path || path[0] == '\0') return NULL;
    const char* resolvedPath = asset_full(path);
    Mix_Music* music = Mix_LoadMUS(resolvedPath);
    if (!music) {
        fprintf(stderr, "Music non chargee: %s (%s)\n", resolvedPath, Mix_GetError());
    }
    return music;
}

static void applyVolumes(const AudioConfig* cfg) {
    const int master = clampInt(cfg->masterVolume, 0, 128);
    const int music = clampInt(cfg->musicVolume, 0, 128);
    const int sfx = clampInt(cfg->sfxVolume, 0, 128);
    const int finalMusic = (master * music) / 128;
    const int finalSfx = (master * sfx) / 128;
    Mix_VolumeMusic(finalMusic);
    Mix_Volume(-1, finalSfx);
}

bool Audio_init(const char* configPath) {
    if (g_audio.initialized) return true;

    if (SDL_WasInit(SDL_INIT_AUDIO) == 0 && SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Audio init SDL echouee: %s\n", SDL_GetError());
        return false;
    }

    Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3);
    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0) {
        fprintf(stderr, "Audio init mixer echouee: %s\n", Mix_GetError());
        return false;
    }

    Mix_AllocateChannels(24);
    setDefaultConfig(&g_audio.config);
    loadConfigFile(&g_audio.config, configPath);

    g_audio.config.masterVolume = clampInt(g_audio.config.masterVolume, 0, 128);
    g_audio.config.musicVolume = clampInt(g_audio.config.musicVolume, 0, 128);
    g_audio.config.sfxVolume = clampInt(g_audio.config.sfxVolume, 0, 128);
    g_audio.config.walkIntervalFrames = clampInt(g_audio.config.walkIntervalFrames, 1, 60);

    g_audio.sfx[AUDIO_SFX_ENEMY_KILLED] = loadChunk(g_audio.config.sfxEnemyKilled);
    g_audio.sfx[AUDIO_SFX_WALK] = loadChunk(g_audio.config.sfxWalk);
    g_audio.sfx[AUDIO_SFX_MENU_CLICK] = loadChunk(g_audio.config.sfxMenuClick);
    g_audio.sfx[AUDIO_SFX_MENU_MOVE] = loadChunk(g_audio.config.sfxMenuMove);
    g_audio.sfx[AUDIO_SFX_ATTACK] = loadChunk(g_audio.config.sfxAttack);
    g_audio.sfx[AUDIO_SFX_PLAYER_HIT] = loadChunk(g_audio.config.sfxPlayerHit);
    g_audio.sfx[AUDIO_SFX_GAME_OVER] = loadChunk(g_audio.config.sfxGameOver);

    g_audio.music[AUDIO_MUSIC_MENU] = loadMusic(g_audio.config.musicMenu);
    g_audio.music[AUDIO_MUSIC_GAMEPLAY] = loadMusic(g_audio.config.musicGameplay);
    g_audio.music[AUDIO_MUSIC_GAMEOVER] = loadMusic(g_audio.config.musicGameOver);

    applyVolumes(&g_audio.config);
    g_audio.currentTrack = AUDIO_MUSIC_COUNT;
    g_audio.walkWasMoving = false;
    g_audio.walkTimer = 0;
    g_audio.initialized = true;
    return true;
}

void Audio_shutdown(void) {
    if (!g_audio.initialized) return;

    Mix_HaltMusic();

    for (int i = 0; i < AUDIO_SFX_COUNT; i++) {
        if (g_audio.sfx[i]) {
            Mix_FreeChunk(g_audio.sfx[i]);
            g_audio.sfx[i] = NULL;
        }
    }
    for (int i = 0; i < AUDIO_MUSIC_COUNT; i++) {
        if (g_audio.music[i]) {
            Mix_FreeMusic(g_audio.music[i]);
            g_audio.music[i] = NULL;
        }
    }

    Mix_CloseAudio();
    Mix_Quit();
    g_audio.initialized = false;
}

void Audio_playSfx(AudioSfxEvent event) {
    if (!g_audio.initialized) return;
    if (event < 0 || event >= AUDIO_SFX_COUNT) return;
    if (!g_audio.sfx[event]) return;
    Mix_PlayChannel(-1, g_audio.sfx[event], 0);
}

void Audio_updateWalk(bool isMoving) {
    if (!g_audio.initialized) return;
    if (!isMoving) {
        g_audio.walkWasMoving = false;
        g_audio.walkTimer = 0;
        return;
    }

    if (!g_audio.walkWasMoving || g_audio.walkTimer <= 0) {
        Audio_playSfx(AUDIO_SFX_WALK);
        g_audio.walkTimer = g_audio.config.walkIntervalFrames;
    } else {
        g_audio.walkTimer--;
    }

    g_audio.walkWasMoving = true;
}

void Audio_setMusicTrack(AudioMusicTrack track) {
    if (!g_audio.initialized) return;
    if (track < 0 || track >= AUDIO_MUSIC_COUNT) return;
    if (!g_audio.music[track]) return;
    if (g_audio.currentTrack == track && Mix_PlayingMusic()) return;

    if (Mix_PlayMusic(g_audio.music[track], -1) == 0) {
        g_audio.currentTrack = track;
    } else {
        fprintf(stderr, "Lecture musique impossible: %s\n", Mix_GetError());
    }
}

void Audio_stopMusic(void) {
    if (!g_audio.initialized) return;
    Mix_HaltMusic();
    g_audio.currentTrack = AUDIO_MUSIC_COUNT;
}
