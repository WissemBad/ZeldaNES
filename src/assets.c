#include "assets.h"

static char g_assetsRoot[1024] = {0};
static bool g_assetsReady = false;
static char g_pathBuffers[16][1024];
static int g_pathBufferIndex = 0;

static bool isAbsolutePath(const char* path) {
    if (!path || path[0] == '\0') return false;
    if (path[0] == '/' || path[0] == '\\') return true;
    if (((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) &&
        path[1] == ':') {
        return true;
    }
    return false;
}

static const char* skipAssetsPrefix(const char* path) {
    if (!path) return path;
    if (strncmp(path, "assets/", 7) == 0) return path + 7;
    if (strncmp(path, "assets\\", 7) == 0) return path + 7;
    return path;
}

bool assets_init(void) {
    if (g_assetsReady) return true;

    const char* base = SDL_GetBasePath();
    if (!base) {
        fprintf(stderr, "Impossible de recuperer SDL_GetBasePath: %s\n", SDL_GetError());
        return false;
    }

    const char* suffix = "assets/";
    snprintf(g_assetsRoot, sizeof(g_assetsRoot), "%s%s", base, suffix);
    SDL_free((void*)base);
    g_assetsReady = true;
    return true;
}

const char* asset_full(const char* relPath) {
    if (!relPath || relPath[0] == '\0') return relPath;
    if (isAbsolutePath(relPath)) return relPath;

    if (!g_assetsReady && !assets_init()) {
        return relPath;
    }

    const char* normalizedRelPath = skipAssetsPrefix(relPath);
    g_pathBufferIndex = (g_pathBufferIndex + 1) % 16;
    snprintf(g_pathBuffers[g_pathBufferIndex], sizeof(g_pathBuffers[g_pathBufferIndex]), "%s%s", g_assetsRoot, normalizedRelPath);
    return g_pathBuffers[g_pathBufferIndex];
}

const char* assets_root(void) {
    if (!g_assetsReady && !assets_init()) return "";
    return g_assetsRoot;
}
