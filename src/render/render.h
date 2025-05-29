#ifndef PC2025_PARTICLE_SIMULATION_RENDER_H
#define PC2025_PARTICLE_SIMULATION_RENDER_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "raylib.h"

typedef struct {
    Color *buffer;
    int32_t w;
    int32_t h;
} RenderEngine;

RenderEngine *RenderEngine_new(int32_t width, int32_t height);
void RenderEngine_destroy(RenderEngine *engine);

void RenderEngine_initialize(RenderEngine *engine, char *screen_name);
void RenderEngine_clear(RenderEngine *engine);
void RenderEngine_render(RenderEngine *engine);
void RenderEngine_draw_pixel(RenderEngine *engine, int32_t x, int32_t y, Color color);
bool RenderEngine_close_event(RenderEngine *renderEngine);

#endif //PC2025_PARTICLE_SIMULATION_RENDER_H
