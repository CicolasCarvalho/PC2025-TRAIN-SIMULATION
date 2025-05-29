#ifndef PC2025_PARTICLE_SIMULATION_PARTICLE_SYSTYEM_H
#define PC2025_PARTICLE_SIMULATION_PARTICLE_SYSTYEM_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "../defs.h"
#include "../render/render.h"

typedef enum {
    AIR,
    SAND
} Particle;

typedef struct {
    Particle *particles;
    int32_t w;
    int32_t h;
} ParticleSystem;

ParticleSystem *ParticleSystem_new(int32_t w, int32_t h);
void ParticleSystem_free(ParticleSystem *ps);

void ParticleSystem_set_particle(ParticleSystem *ps, int32_t x, int32_t y, Particle type);
void ParticleSystem_update(ParticleSystem *ps);
void ParticleSystem_render(ParticleSystem *ps, RenderEngine *engine);


#endif // PC2025_PARTICLE_SIMULATION_PARTICLE_SYSTYEM_H