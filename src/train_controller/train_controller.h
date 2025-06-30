#ifndef PC2025_TRAIN_SIMULATION_TRAIN_CONTROLLER_H
#define PC2025_TRAIN_SIMULATION_TRAIN_CONTROLLER_H

#include "stdint.h"
#include "pthread.h"
#include "math.h"
#include "../world/world.h"
#include "../render/render.h"
#include "../utils/utils.h"

typedef struct Train Train;

struct Train {
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
    Train *trains;
    int32_t train_count;
    bool is_displaying;
} TrainController;

TrainController *TrainController_new(int32_t train_count, World *world);
void TrainController_free(TrainController *train_controller);

void TrainController_update(TrainController *train_controller, double dt);
void *TrainController_thread_ctx(void *arg);
void TrainController_draw(TrainController *train_controller, bool is_highlight);

void TrainController_set_train_city(TrainController *train_controller, int32_t train_idx, int32_t city_idx);
void TrainController_move_to(Train *train, int32_t target_city_idx);

#endif //PC2025_TRAIN_SIMULATION_TRAIN_CONTROLLER_H
