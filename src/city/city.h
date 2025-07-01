#ifndef PC2025_ENTITY_SIMULATION_CITY_H
#define PC2025_ENTITY_SIMULATION_CITY_H

#include "stdint.h"
#include "raylib.h"
#include "../defs.h"

typedef struct {
    char name[32];
    int32_t x;
    int32_t y;
    float production_rate;
} City;

void City_draw(City *city);

#endif //PC2025_ENTITY_SIMULATION_CITY_H
