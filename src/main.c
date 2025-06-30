#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "pthread.h"
#include "raylib.h"

#include "render/render.h"
#include "train_controller/train_controller.h"
#include "world/world.h"
#include "city/city.h"
#include "utils/utils.h"
#include "defs.h"

void generate_cities(World *world, int32_t count);
void generate_railways(World *world);
void draw_target_cities(World *world, TrainController *train_controller);

int main (void) {
    int seed = (int)get_time();
    PRINT("seed: %i", seed);
    srand(seed);

    PRINT("creating render engine...");
    RenderEngine *engine = RenderEngine_new(WINDOW_WIDTH, WINDOW_HEIGHT);
    RenderEngine_initialize(engine, "Train Simulation");
    
    PRINT("creating world...");
    World *world = World_new(CITY_COUNT);
    generate_cities(world, CITY_COUNT);
    generate_railways(world);

    for (int32_t i = 0; i < world->city_count; i++) {
        City city = world->cities[i];
        PRINT("city %i: %s (%i, %i, %f)", i, city.name, city.x, city.y, city.production_rate);
    }

    PRINT("creating train engine...");

    float dt = GetFrameTime();

    double last_fps_update = get_time();
    int32_t *fps_buffer = calloc(FPS_BUFFER, sizeof(int32_t));
    float avg_fps = 0;
    int32_t frame_count = 0;

    bool target_cities = false;
    bool railway = true;
    int32_t highlight = 0;

#if MULTITHREADING
    pthread_t threads[THREAD_COUNT];
    TrainController *thread_ctx[THREAD_COUNT];

    int32_t trains_per_thread = TRAIN_COUNT / THREAD_COUNT;
    int32_t remaining_trains = TRAIN_COUNT % THREAD_COUNT;

    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        thread_ctx[i] = TrainController_new(trains_per_thread + (i < remaining_trains ? 1 : 0), world);

        for (int32_t j = 0; j < thread_ctx[i]->train_count; j++) {
            TrainController_set_train_city(thread_ctx[i], j, (i * trains_per_thread + j) % world->city_count);
        }
        pthread_create(&threads[i], NULL, TrainController_thread_ctx, (void *)thread_ctx[i]);
    }
#else
    TrainController *train_controller = TrainController_new(TRAIN_COUNT, world);

    for (int32_t i = 0; i < train_controller->train_count; i++) {
        TrainController_set_train_city(train_controller, i, i % world->city_count);
        // TrainController_move_to(train_controller, world, i, (i + 2) % world->city_count);   
    }
#endif

    PRINT("starting rendering");
    while (!RenderEngine_close_event(engine)) {
        RenderEngine_start(engine);
        
        dt = GetFrameTime();

        if (get_time() - last_fps_update > FPS_INTERVAL) {
            last_fps_update = get_time();
            frame_count = (frame_count + 1) % FPS_BUFFER;
            fps_buffer[frame_count] = GetFPS();
            avg_fps = 0;
            for (int32_t i = 0; i < FPS_BUFFER; i++) {
                avg_fps += fps_buffer[i];
            }
            avg_fps /= FPS_BUFFER;

            PRINT("FPS: %f", avg_fps);
        }

        if (IsKeyPressed(KEY_J)) {
            target_cities = !target_cities;
        }
        if (IsKeyPressed(KEY_H)) {
            railway = !railway;
        }

#if MULTITHREADING
        if (IsKeyPressed(KEY_ZERO)) {
            for (int32_t i = 0; i < THREAD_COUNT; i++) {
                highlight = 0;
            }
        }
        for (int32_t i = 1; i <= (THREAD_COUNT > 10 ? 10: THREAD_COUNT); i++) {
            int32_t key_num = 48 + i;

            if (IsKeyPressed(key_num)) {
                highlight = i;
            }
        }
#endif

#if MULTITHREADING
        for (int32_t i = 0; i < THREAD_COUNT; i++) {
            if (i == highlight - 1) continue;
            TrainController_draw(thread_ctx[i], highlight == 0);
            if (target_cities)
                draw_target_cities(world, thread_ctx[i]);
        }

        if (highlight > 0) {
            TrainController_draw(thread_ctx[highlight - 1], true);
        }        
#else
        TrainController_update(train_controller, dt);
        TrainController_draw(train_controller);
        if (target_cities)
            draw_target_cities(world, train_controller);
#endif
        World_draw(world, railway);
        DrawText("1-n to filter thread / h to hide edges / j to show target", 10, WINDOW_HEIGHT - 30, 20, WHITE);

        RenderEngine_end(engine);
    }
    
#if MULTITHREADING
    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
#endif

    free(fps_buffer);
    PRINT("destroying render engine...");
    RenderEngine_destroy(engine);
    PRINT("render engine destroyed!");

    World_free(world);
    return 0;
}

void generate_cities(World *world, int32_t count) {
    for (int32_t i = 0; i < count; i++) {
        World_add_city(world, (City) {
            .name = "City",
            .x = rand() % (WINDOW_WIDTH - 100) + 50,
            .y = rand() % (WINDOW_HEIGHT - 100) + 50,
            .production_rate = 1.,
            .last_visit = 0,
        });
    }
}

void generate_railways(World *world) {
    for (int32_t i = 0; i < world->city_capacity; i++) {
        int32_t c = 0;
        do {    
            World_create_railways(world, (Railway) {
                .distance = 1,
                .color = (Color) {.r = 255, .g = 255, .b = 255, .a = 100},
            }, i, (i + (++c)) % world->city_count);
        } while (rand() % 2);
    }
}

void draw_target_cities(World *world, TrainController *train_controller) {
    for (int32_t i = 0; i < train_controller->train_count; i++) {
        int32_t offset_count = 0;
        Train *train = &train_controller->trains[i];
        if (train->path_len > 0) {
            int32_t last_city_idx = train->path[train->path_len - 1];
            for (int32_t j = 0; j < i; j++) {
                Train *other_train = &train_controller->trains[j];
                if (other_train->path_len > 0 && other_train->path[other_train->path_len - 1] == last_city_idx) {
                    offset_count++;
                }
            }
            City *last_city = &world->cities[last_city_idx];
            DrawCircle(last_city->x, last_city->y - 10 - (offset_count * 10), 3, train->color);
        }
    }
}