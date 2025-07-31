#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "pthread.h"
#include "raylib.h"

#include "render/render.h"
#include "entity_controller/entity_controller.h"
#include "world/world.h"
#include "city/city.h"
#include "utils/utils.h"
#include "defs.h"

#if MPI_MODE
    #include "mpi/mpi.h"
#endif

void generate_cities(World *world, int32_t count);
void generate_edges(World *world);
void draw_target_cities(World *world, EntityController *entity_controller);

int main (int argc, char *argv[]) {
#if MPI_MODE
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int32_t thread_count = MULTITHREADING ? THREAD_COUNT : 1;

    if (size < THREAD_COUNT + 1) {
        RAISE("Deve haver pelo menos 2 processos MPI para executar este programa.");
    }

    if (rank > 0) {
        EntityController_run_slave_process(rank, size);
        MPI_Finalize();
        return 0;   
    }
#endif

    int seed = (int)get_time();
    PRINT("seed: %i", seed);
    srand(seed);

    PRINT("creating render engine...");
    RenderEngine *engine = RenderEngine_new(WINDOW_WIDTH, WINDOW_HEIGHT);
    RenderEngine_initialize(engine, "Agent Simulation");
    
    PRINT("creating world...");
    World *world = World_new(CITY_COUNT);
    generate_cities(world, CITY_COUNT);
    generate_edges(world);

    for (int32_t i = 0; i < world->city_count; i++) {
        City city = world->cities[i];
        PRINT("city %i: %s (%i, %i, %f)", i, city.name, city.x, city.y, city.production_rate);
    }

    PRINT("creating agent engine...");

    float dt = GetFrameTime();

    double last_fps_update = get_time();
    int32_t *fps_buffer = calloc(FPS_BUFFER, sizeof(int32_t));
    float avg_fps = 0;
    int32_t frame_count = 0;

    bool target_cities = false;
    bool edge = true;
    int32_t highlight = 0;

#if MULTITHREADING
    pthread_t threads[THREAD_COUNT];
    EntityController *thread_ctx[THREAD_COUNT];

    int32_t entity_per_thread = ENTITY_COUNT / THREAD_COUNT;
    int32_t remaining_entities = ENTITY_COUNT % THREAD_COUNT;

    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        thread_ctx[i] = EntityController_new(entity_per_thread + (i < remaining_entities ? 1 : 0), world);

        for (int32_t j = 0; j < thread_ctx[i]->entities_count; j++) {
            EntityController_set_entity_city(thread_ctx[i], j, (i * entity_per_thread + j) % world->city_count);
        }
        pthread_create(&threads[i], NULL, EntityController_thread_ctx, (void *)thread_ctx[i]);
    }
#else
    EntityController *entity_controller = EntityController_new(ENTITY_COUNT, world);

    for (int32_t i = 0; i < entity_controller->entities_count; i++) {
        EntityController_set_entity_city(entity_controller, i, i % world->city_count);
        // EntityController_move_to(entity_controller, world, i, (i + 2) % world->city_count);   
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
            edge = !edge;
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

    World_draw(world, edge);

#if MULTITHREADING
        for (int32_t i = 0; i < THREAD_COUNT; i++) {
            if (i == highlight - 1) continue;
            EntityController_draw(thread_ctx[i], highlight == 0);
            if (target_cities)
                draw_target_cities(world, thread_ctx[i]);
        }

        if (highlight > 0) {
            EntityController_draw(thread_ctx[highlight - 1], true);
        }        
#else
        EntityController_update(entity_controller, dt);
        EntityController_draw(entity_controller, true);
        if (target_cities)
            draw_target_cities(world, entity_controller);
#endif
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
        });
    }
}

void generate_edges(World *world) {
    for (int32_t i = 0; i < world->city_capacity; i++) {
        int32_t c = 0;
        do {    
            World_create_edges(world, (Edge) {
                .exists = true,
                .color = (Color) {.r = 255, .g = 255, .b = 255, .a = 100},
            }, i, (i + (++c)) % world->city_count);
        } while (rand() % 2);
    }
}

void draw_target_cities(World *world, EntityController *entity_controller) {
    for (int32_t i = 0; i < entity_controller->entities_count; i++) {
        int32_t offset_count = 0;
        Entity *entity = &entity_controller->entities[i];
        if (entity->path_len > 0) {
            int32_t last_city_idx = entity->path[entity->path_len - 1];
            for (int32_t j = 0; j < i; j++) {
                Entity *other_entity = &entity_controller->entities[j];
                if (other_entity->path_len > 0 && other_entity->path[other_entity->path_len - 1] == last_city_idx) {
                    offset_count++;
                }
            }
            City *last_city = &world->cities[last_city_idx];
            DrawCircle(last_city->x, last_city->y - 10 - (offset_count * 10), 3, entity->color);
        }
    }
}