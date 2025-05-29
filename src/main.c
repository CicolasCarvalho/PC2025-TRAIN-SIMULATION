#include <stdio.h>
#include <stdint.h>
#include "raylib.h"
#include "render/render.h"
#include "particle_system/particle_system.h"
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

int main (void) {
    PRINT("creating render engine...");
    RenderEngine *engine = RenderEngine_new(WINDOW_WIDTH, WINDOW_HEIGHT);
    RenderEngine_initialize(engine, "Particle Simulation");
    PRINT("render engine loaded!");

    PRINT("createing particle system...");
    ParticleSystem *particle_system = ParticleSystem_new(WINDOW_WIDTH, WINDOW_HEIGHT);
    PRINT("particle system created!");

    double elapsed_time = get_time(),
           start_time = get_time();

    while (!RenderEngine_close_event(engine)) {
        elapsed_time = get_time();

        if (elapsed_time - start_time > DELTA_TIME) {
            PRINT("tick! (dt: %lf, fps: %lf)", elapsed_time - start_time, 1.0/(elapsed_time - start_time));
            start_time = get_time();
            
            ParticleSystem_set_particle(particle_system, rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT, SAND);
            
            ParticleSystem_update(particle_system);
            ParticleSystem_render(particle_system, engine);
            RenderEngine_render(engine);
        }
    }

    PRINT("destroying render engine...");
    RenderEngine_destroy(engine);
    PRINT("render engine destroyed!");
    return 0;
}