#ifndef NUPRC_ASSETS_H
#define NUPRC_ASSETS_H

#include "core.h"

bool assets_init(void);
const char* asset_full(const char* relPath);
const char* assets_root(void);

#endif
