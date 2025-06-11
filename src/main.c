#include <stdio.h>
#include <stdint.h>
#include "raylib.h"

#include "render/render.h"
#include "particle_system/particle_system.h"
#include "utils/utils.h"
#include "defs.h"

#ifdef WIN32

#include <windows.h>
double get_time() {
    LARGE_INTEGER t, f;
    QueryPerformanceCounter(&t);
    QueryPerformanceFrequency(&f);
    return (double)t.QuadPart/(double)f.QuadPart;
}

#else

#include <sys/time.h>
#include <sys/resource.h>

double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec*1e-6;
}

#endif

void handle_mouse_click(ParticleSystem *particle_system, Particle particle);

int main (void) {
    PRINT("creating render engine...");
    RenderEngine *engine = RenderEngine_new(WINDOW_WIDTH, WINDOW_HEIGHT);
    RenderEngine_initialize(engine, "Particle Simulation");
    PRINT("render engine loaded!");

    PRINT("creating particle system...");
    ParticleSystem *particle_system = ParticleSystem_new(WINDOW_WIDTH, WINDOW_HEIGHT);
    PRINT("particle system created!");

    double elapsed_time = get_time(),
           start_time = get_time(),
           dt = 0,
           fps = 0;

    while (!RenderEngine_close_event(engine)) {
        elapsed_time = get_time();

        if (elapsed_time - start_time > (1.0/TARGET_FPS)) {
            dt = elapsed_time - start_time;
            fps = 1.0/dt;
            PRINT("(dt: %lf, fps: %lf)", dt, fps);
            start_time = get_time();
            
            for(uint32_t i = 0; i < TICK_PER_SECOND; i++) {
                handle_mouse_click(particle_system, SAND_PARTICLE);

                ParticleSystem_update(particle_system);
                ParticleSystem_render(particle_system, engine);
                RenderEngine_render(engine);
            }
        }
    }

    PRINT("destroying render engine...");
    RenderEngine_destroy(engine);
    PRINT("render engine destroyed!");
    return 0;
}

void handle_mouse_click(ParticleSystem *particle_system, Particle particle) {
    if (IsMouseButtonDown(0)) {
        int32_t x = GetMouseX(), y = GetMouseY();

        for (int32_t i = -BRUSH_RADIUS / 2; i < BRUSH_RADIUS / 2; i++)
            for (int32_t j = -BRUSH_RADIUS / 2; j < BRUSH_RADIUS / 2; j++)
                if (x + i < WINDOW_WIDTH && x + i >= 0 && y + j < WINDOW_HEIGHT && y + j >= 0)
                ParticleSystem_set_particle(particle_system, x + i, y + j, particle);
    }
}