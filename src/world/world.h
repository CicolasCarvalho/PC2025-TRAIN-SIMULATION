#ifndef PC2025_TRAIN_SIMULATION_WORLD_H
#define PC2025_TRAIN_SIMULATION_WORLD_H

#include "raylib.h"
#include "stdint.h"
#include "../city/city.h"

typedef struct {
    float distance;
    Color color;
} Railway;

typedef struct {
    City *cities;
    Railway **railway;
    int32_t city_capacity;
    int32_t city_count;
} World;

World *World_new(int32_t city_count);
void World_free(World *world);

void World_add_city(World *world, City city);
void World_create_railways(World *world, Railway railway, int32_t from_idx, int32_t to_idx);
void World_draw(World *world);

#endif //PC2025_TRAIN_SIMULATION_WORLD_H
