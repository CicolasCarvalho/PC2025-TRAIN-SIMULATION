#ifndef PC2025_TRAIN_SIMULATION_CITY_H
#define PC2025_TRAIN_SIMULATION_CITY_H

#include "stdint.h"
#include "raylib.h"
#include "../defs.h"

typedef struct {
    char name[32];
    int32_t x;
    int32_t y;
    float production_rate;
    int32_t last_visit;
} City;

void City_draw(City *city);

#endif //PC2025_TRAIN_SIMULATION_CITY_H
