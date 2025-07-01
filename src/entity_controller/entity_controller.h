#ifndef PC2025_ENTITY_SIMULATION_TRAIN_CONTROLLER_H
#define PC2025_ENTITY_SIMULATION_TRAIN_CONTROLLER_H

#include "stdint.h"
#include "pthread.h"
#include "math.h"
#include "../world/world.h"
#include "../render/render.h"
#include "../utils/utils.h"

typedef struct Entity Entity;

struct Entity {
    float x;
    float y;
    float speed;
    int32_t city_idx;
    int32_t target_idx;
    
    bool is_moving;
    float progress;
    
    bool is_loading;
    float load;

    int32_t path[CITY_COUNT];
    int32_t path_len;

    Color color;
};

typedef struct {
    World *world;
    Entity *entities;
    int32_t entities_count;
    bool is_displaying;
} EntityController;

EntityController *EntityController_new(int32_t entities_count, World *world);
void EntityController_free(EntityController *entity_controller);

void EntityController_update(EntityController *entity_controller, double dt);
void *EntityController_thread_ctx(void *arg);
void EntityController_draw(EntityController *entity_controller, bool is_highlight);

void EntityController_set_entity_city(EntityController *entity_controller, int32_t entity_idx, int32_t city_idx);
void EntityController_move_to(Entity *entity, int32_t target_city_idx);

#endif //PC2025_ENTITY_SIMULATION_TRAIN_CONTROLLER_H
