#include "particle_system.h"

//-Declarations--------------------------------------------------------------------------------------------------------------

static void handle_air_particle(ParticleSystem *ps, int32_t x, int32_t y);
static void handle_sand_particle(ParticleSystem *ps, int32_t x, int32_t y);

//-Functions-----------------------------------------------------------------------------------------------------------------

ParticleSystem *ParticleSystem_new(int32_t w, int32_t h) {
    ParticleSystem *ps = malloc(sizeof *ps);

    ps->w = w;
    ps->h = h;
    ps->particles = calloc(w * h, sizeof(Particle));
    
    return ps;
}

void ParticleSystem_free(ParticleSystem *ps) {
    free(ps->particles);
    free(ps);
}

void ParticleSystem_set_particle(ParticleSystem *ps, int32_t x, int32_t y, Particle type) {
    ps->particles[y * ps->w + x] = type;
}

void ParticleSystem_update(ParticleSystem *ps) {
    for (int32_t y = ps->h - 1; y >= 0; y--) {
        for (int32_t x = ps->w - 1; x >= 0; x--) {
            Particle p = ps->particles[y * ps->w + x];

            switch (p) {
                case SAND:
                    handle_sand_particle(ps, x, y);
                    break;
            }
        }
    }
}

void ParticleSystem_render(ParticleSystem *ps, RenderEngine *engine) {
    for (int32_t y = ps->h - 1; y >= 0; y--) {
        for (int32_t x = ps->w - 1; x >= 0; x--) {
            Particle p = ps->particles[GET_POSITION(x, y, ps->w)];

            switch (p) {
                case AIR:
                    RenderEngine_draw_pixel(engine, x, y, BLACK);
                    break;
                case SAND:
                    RenderEngine_draw_pixel(engine, x, y, YELLOW);
                    break;
            }
        }
    }
}

//-static--------------------------------------------------------------------------------------------------------------------

static void handle_air_particle(ParticleSystem *ps, int32_t x, int32_t y) {
    // air particles não fazem nada
    return;
}

static void handle_sand_particle(ParticleSystem *ps, int32_t x, int32_t y) {
    if (y + 1 < ps->h) {
        Particle below = ps->particles[(y + 1) * ps->w + x];

        if (below == AIR) {
            ps->particles[GET_POSITION(x, y + 1, ps->w)] = SAND;
            ps->particles[GET_POSITION(x, y, ps->w)] = AIR;
        } else if (x > 0 && ps->particles[GET_POSITION(x - 1, y + 1, ps->w)] == AIR) {
            ps->particles[GET_POSITION(x - 1, y + 1, ps->w)] = SAND;
            ps->particles[GET_POSITION(x, y, ps->w)] = AIR;
        } else if (x < ps->w - 1 && ps->particles[GET_POSITION(x + 1, y + 1, ps->w)] == AIR) {
            ps->particles[GET_POSITION(x + 1, y + 1, ps->w)] = SAND;
            ps->particles[GET_POSITION(x, y, ps->w)] = AIR;
        }        
    }
}