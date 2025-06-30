#ifndef PC2025_TRAIN_SIMULATION_H
#define PC2025_TRAIN_SIMULATION_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "raylib.h"
#include "../defs.h"

typedef struct {
    int32_t w;
    int32_t h;
} RenderEngine;

RenderEngine *RenderEngine_new(int32_t width, int32_t height);
void RenderEngine_destroy(RenderEngine *engine);

void RenderEngine_initialize(RenderEngine *engine, char *screen_name);
void RenderEngine_start(RenderEngine *engine);
void RenderEngine_end(RenderEngine *engine);
bool RenderEngine_close_event(RenderEngine *renderEngine);

#endif //PC2025_TRAIN_SIMULATION_H
