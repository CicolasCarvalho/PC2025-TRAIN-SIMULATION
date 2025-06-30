#include "render.h"

//-Functions-----------------------------------------------------------------------------------------------------------------

RenderEngine *RenderEngine_new(int32_t width, int32_t height) {
    RenderEngine *renderEngine = malloc(sizeof *renderEngine);

    renderEngine->w = width;
    renderEngine->h = height;

    return renderEngine;
}

void RenderEngine_initialize(RenderEngine *renderEngine, char *screen_name) {
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(renderEngine->w, renderEngine->h, screen_name);
    ClearWindowState(FLAG_VSYNC_HINT);
    // SetTargetFPS(TARGET_FPS);
}

void RenderEngine_destroy(RenderEngine *renderEngine) {
    CloseWindow();

    free(renderEngine);
}

void RenderEngine_start(RenderEngine *renderEngine) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(10, 10);
}

void RenderEngine_end(RenderEngine *renderEngine) {
    EndDrawing();
}

bool RenderEngine_close_event(RenderEngine *renderEngine) {
    return WindowShouldClose();
}