#include "render.h"

//-Functions-----------------------------------------------------------------------------------------------------------------

RenderEngine *RenderEngine_new(int32_t width, int32_t height) {
    RenderEngine *renderEngine = malloc(sizeof *renderEngine);

    renderEngine->buffer = malloc(sizeof(Color) * width * height);
    renderEngine->w = width;
    renderEngine->h = height;

    return renderEngine;
}

void RenderEngine_initialize(RenderEngine *renderEngine, char *screen_name) {
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(renderEngine->w, renderEngine->h, screen_name);
}

void RenderEngine_destroy(RenderEngine *renderEngine) {
    CloseWindow();

    free(renderEngine->buffer);
    free(renderEngine);
}

void RenderEngine_clear(RenderEngine *renderEngine) {
    memset(renderEngine->buffer, 0, renderEngine->w * renderEngine->h * sizeof(Color));
}

void RenderEngine_render(RenderEngine *renderEngine) {
    BeginDrawing();
    ClearBackground(BLACK);

    for (int32_t y = 0; y < renderEngine->h; y++) {
        for (int32_t x = 0; x < renderEngine->w; x++) {            
            Color pixel = renderEngine->buffer[y * renderEngine->w + x];
            DrawPixel(x, y, pixel);
        }
    }

    EndDrawing();
}

void RenderEngine_draw_pixel(RenderEngine *renderEngine, int32_t x, int32_t y, Color color) {
    renderEngine->buffer[y * renderEngine->w + x] = color;
}

bool RenderEngine_close_event(RenderEngine *renderEngine) {
    return WindowShouldClose();
}