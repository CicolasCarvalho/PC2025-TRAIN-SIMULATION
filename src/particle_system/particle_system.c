#include "particle_system.h"

//-Definitions---------------------------------------------------------------------------------------------------------------

typedef enum {
    UP,
    UP_RIGHT,
    RIGHT,
    DOWN_RIGHT,
    DOWN,
    DOWN_LEFT,
    LEFT,
    UP_LEFT
} Direction;

//-Declarations--------------------------------------------------------------------------------------------------------------

static Particle *get_adjacent_particles(ParticleSystem *ps, int32_t x, int32_t y);
static void handle_air_particle(ParticleSystem *ps, int32_t x, int32_t y);
static void handle_sand_particle(ParticleSystem *ps, int32_t x, int32_t y);
static void handle_water_particle(ParticleSystem *ps, int32_t x, int32_t y);

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
                case AIR_PARTICLE:
                    handle_air_particle(ps, x, y);
                    break;
                case SAND_PARTICLE:
                    handle_sand_particle(ps, x, y);
                    break;
                case WATER_PARTICLE:
                    handle_water_particle(ps, x, y);
                    break;
                default:
                    RAISE("Unknown particle type (%i)", p);
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
                case AIR_PARTICLE:
                    RenderEngine_draw_pixel(engine, x, y, BLACK);
                    break;
                case SAND_PARTICLE:
                    RenderEngine_draw_pixel(engine, x, y, YELLOW);
                    break;
                case WATER_PARTICLE:
                    RenderEngine_draw_pixel(engine, x, y, BLUE);
                    break;

            }
        }
    }
}

//-static--------------------------------------------------------------------------------------------------------------------

static Particle *get_adjacent_particles(ParticleSystem *ps, int32_t x, int32_t y) {
    Particle *adj = malloc(8 * sizeof *adj);
    for (int32_t i = 0; i < 8; i++) {
        adj[i] = AIR_PARTICLE;
    }

    if (y > 0) {
        adj[UP] = ps->particles[GET_POSITION(x, y - 1, ps->w)];

        if (x > 0)
            adj[UP_LEFT] = ps->particles[GET_POSITION(x - 1, y - 1, ps->w)];

        if (x < ps->w - 1)
            adj[UP_RIGHT] = ps->particles[GET_POSITION(x + 1, y - 1, ps->w)];
    }

    if (y < ps->h - 1) {
        adj[DOWN] = ps->particles[GET_POSITION(x, y + 1, ps->w)];

        if (x > 0)
            adj[DOWN_LEFT] = ps->particles[GET_POSITION(x - 1, y + 1, ps->w)];

        if (x < ps->w - 1)
            adj[DOWN_RIGHT] = ps->particles[GET_POSITION(x + 1, y + 1, ps->w)];
    }

    if (x < ps->w - 1)
        adj[RIGHT] = ps->particles[GET_POSITION(x + 1, y, ps->w)];

    if (x > 0)
        adj[LEFT] = ps->particles[GET_POSITION(x - 1, y, ps->w)];

    return adj;
}

static void handle_air_particle(ParticleSystem *ps, int32_t x, int32_t y) {
    // air particles não fazem nada
    return;
}

static void handle_sand_particle(ParticleSystem *ps, int32_t x, int32_t y) {
    if (y + 1 < ps->h) {
        Particle below = ps->particles[GET_POSITION(x, y + 1, ps->w)];

        if (below == AIR_PARTICLE) {
            ps->particles[GET_POSITION(x, y + 1, ps->w)] = SAND_PARTICLE;
            ps->particles[GET_POSITION(x, y, ps->w)] = AIR_PARTICLE;
        } else if (x > 0 && ps->particles[GET_POSITION(x - 1, y + 1, ps->w)] == AIR_PARTICLE) {
            ps->particles[GET_POSITION(x - 1, y + 1, ps->w)] = SAND_PARTICLE;
            ps->particles[GET_POSITION(x, y, ps->w)] = AIR_PARTICLE;
        } else if (x < ps->w - 1 && ps->particles[GET_POSITION(x + 1, y + 1, ps->w)] == AIR_PARTICLE) {
            ps->particles[GET_POSITION(x + 1, y + 1, ps->w)] = SAND_PARTICLE;
            ps->particles[GET_POSITION(x, y, ps->w)] = AIR_PARTICLE;
        }        
    }
}

static void handle_water_particle(ParticleSystem *ps, int32_t x, int32_t y) {
    if (y + 1 < ps->h) {
        Particle *adj = get_adjacent_particles(ps, x, y);
        bool flag = rand() % 2;

        bool possible_spaces[8] = {
            0, 
            0,
            x < ps->w - 1                   && adj[RIGHT] == AIR_PARTICLE,
            x < ps->w - 1 && y < ps->h - 1  && adj[DOWN_RIGHT] == AIR_PARTICLE,
            y < ps->h - 1                   && adj[DOWN] == AIR_PARTICLE,
            x > 0 && y < ps->h - 1          && adj[DOWN_LEFT] == AIR_PARTICLE,
            x > 0                           && adj[LEFT] == AIR_PARTICLE,
            0,
        };

        if (possible_spaces[DOWN]) {
            ps->particles[GET_POSITION(x, y + 1, ps->w)] = WATER_PARTICLE;
            ps->particles[GET_POSITION(x, y, ps->w)] = AIR_PARTICLE;
        } else if (
            (possible_spaces[DOWN_RIGHT] && !possible_spaces[DOWN_LEFT]) ||
            (possible_spaces[DOWN_LEFT] && possible_spaces[DOWN_RIGHT] && flag == 0)) {
            ps->particles[GET_POSITION(x + 1, y + 1, ps->w)] = WATER_PARTICLE;
            ps->particles[GET_POSITION(x, y, ps->w)] = AIR_PARTICLE;
        } else if (
            (possible_spaces[DOWN_LEFT] && !possible_spaces[DOWN_RIGHT]) ||
            (possible_spaces[DOWN_RIGHT] && possible_spaces[DOWN_LEFT] && flag == 1)) {
            ps->particles[GET_POSITION(x - 1, y + 1, ps->w)] = WATER_PARTICLE;
            ps->particles[GET_POSITION(x, y, ps->w)] = AIR_PARTICLE;
        } else if (
            (possible_spaces[RIGHT] && !possible_spaces[LEFT]) ||
            (possible_spaces[LEFT] && possible_spaces[RIGHT] && flag == 0)) {
            ps->particles[GET_POSITION(x + 1, y, ps->w)] = WATER_PARTICLE;
            ps->particles[GET_POSITION(x, y, ps->w)] = AIR_PARTICLE;
        } else if (
            (possible_spaces[LEFT] && !possible_spaces[RIGHT]) ||
            (possible_spaces[RIGHT] && possible_spaces[LEFT] && flag == 1)) {
            ps->particles[GET_POSITION(x - 1, y, ps->w)] = WATER_PARTICLE;
            ps->particles[GET_POSITION(x, y, ps->w)] = AIR_PARTICLE;
        } 
        
        free(adj);
    }
}