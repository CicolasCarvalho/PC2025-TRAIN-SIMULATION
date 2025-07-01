#ifndef PC2025_ENTITY_SIMULATION_WORLD_H
#define PC2025_ENTITY_SIMULATION_WORLD_H

#include "raylib.h"
#include "stdint.h"
#include "../city/city.h"

typedef struct {
    bool exists;
    Color color;
} Edge;

typedef struct {
    City *cities;
    Edge **edge;
    int32_t city_capacity;
    int32_t city_count;
} World;

World *World_new(int32_t city_count);
void World_free(World *world);

void World_add_city(World *world, City city);
void World_create_edges(World *world, Edge edge, int32_t from_idx, int32_t to_idx);
void World_draw(World *world, bool edges);

#endif //PC2025_ENTITY_SIMULATION_WORLD_H
