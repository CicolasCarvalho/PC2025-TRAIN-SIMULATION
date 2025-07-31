#include "entity_controller.h"

static void reset_entity(Entity *entity);
static void trace_route_to(World *world, Entity *entity, int32_t destination);
static void Entity_update(Entity *entity, World *world, float dt);

typedef struct {
    int32_t origin_x;
    int32_t origin_y;
    int32_t target_x;
    int32_t target_y;

    float x;
    float y;
    float speed;
    int32_t city_idx;
    int32_t target_idx;
    
    bool is_moving;
    float progress;    

    int32_t path[CITY_COUNT];
    int32_t path_len;

    float dt;
} MPISendContext;

EntityController *EntityController_new(int32_t entities_count, World *world)
{
    EntityController *tc = (EntityController *)malloc(sizeof(EntityController));

    tc->entities_count = entities_count;
    tc->entities = (Entity *)malloc(sizeof(Entity) * entities_count);
    tc->world = world;
    tc->is_displaying = true;

    for (int32_t i = 0; i < entities_count; i++)
    {
        Entity *entity = &tc->entities[i];

        reset_entity(entity);

        entity->speed = 100;
        entity->color = (Color){.r = (rand() % 255), .g = (rand() % 255), .b = (rand() %255), .a = 255};
    }

    return tc;
}

void EntityController_free(EntityController *entity_controller)
{
    free(entity_controller->entities);
    free(entity_controller);
}

void EntityController_update(EntityController *entity_controller, double dt)
{
    for (int32_t i = 0; i < entity_controller->entities_count; i++)
    {
        Entity *entity = &entity_controller->entities[i];

        Entity_update(entity, entity_controller->world, dt);
    }
}

void *EntityController_thread_ctx(void *arg) {
    EntityController *ctx = (EntityController *)arg;

    static double last_time = 0.0;
    double current_time = get_time();
    float dt = (float)(current_time - last_time);

    while (!WindowShouldClose()) {
        last_time = current_time;
        current_time = get_time();
        dt = (float)(current_time - last_time);

        EntityController_update(ctx, dt);
    }

    return NULL;
}

void EntityController_draw(EntityController *entity_controller, bool is_highlight)
{
    for (int32_t i = 0; i < entity_controller->entities_count; i++)
    {
        Entity *entity = &entity_controller->entities[i];

        if (is_highlight) {
            DrawRectangle(entity->x - 4, entity->y - 4, 8, 8, entity->color);
        } else {
            DrawRectangle(entity->x - 4, entity->y - 4, 8, 8, (Color){
                .r = 50,
                .g = 50,
                .b = 50,
                .a = 255
            });
        }
    }
}

void EntityController_set_entity_city(EntityController *entity_controller, int32_t entity_idx, int32_t city_idx)
{
    Entity *entity = &entity_controller->entities[entity_idx];
    City *city = &entity_controller->world->cities[city_idx];

    reset_entity(entity);

    entity->x = city->x;
    entity->y = city->y;
    entity->city_idx = city_idx;
    entity->is_loading = true;
}

void EntityController_move_to(Entity *entity, int32_t target_city_idx)
{
    entity->target_idx = target_city_idx;
    entity->is_moving = true;
    entity->progress = .0;
}

void EntityController_run_slave_process(int32_t rank, int32_t size) {
#if MPI_MODE
    MPISendContext *entity = malloc(sizeof(MPISendContext));

    while (1) {
        MPI_Recv(entity, sizeof(MPISendContext), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int32_t origin_x = entity->origin_x;
        int32_t origin_y = entity->origin_y;
        int32_t target_x = entity->target_x;
        int32_t target_y = entity->target_y;

        double distance = sqrt(pow(origin_x - target_x, 2) + pow(origin_y - target_y, 2));

        entity->progress += (entity->speed / distance) * entity->dt;

        if (entity->progress >= 1)
        {
            entity->progress = 1;
            entity->is_moving = false;
            entity->city_idx = entity->target_idx;
            entity->target_idx = -1;

            if (entity->path_len > 0)
            {
                for (int32_t j = 0; j < entity->path_len - 1; j++)
                {
                    entity->path[j] = entity->path[j + 1];
                }
                entity->path_len--;
            }

            if (entity->path_len > 0)
            {
                EntityController_move_to(entity, entity->path[0]);
            }
        }
        
        entity->x = (origin_x * (1 - entity->progress)) + (target_x * entity->progress);
        entity->y = (origin_y * (1 - entity->progress)) + (target_y * entity->progress);
        
        MPI_Send(entity, sizeof(MPISendContext), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    }
    
#endif
}

static void reset_entity(Entity *entity)
{
    entity->x = 0;
    entity->y = 0;
    entity->city_idx = -1;
    entity->target_idx = -1;
    entity->is_moving = false;
    entity->progress = 0.;
    entity->load = 0.;
    entity->is_loading = false;
    entity->path_len = 0;
}

static void trace_route_to(World *world, Entity *entity, int32_t destination)
{
    int32_t q[CITY_COUNT];
    int32_t head = 0, tail = 0;

    int32_t dist[CITY_COUNT];
    int32_t prev[CITY_COUNT];

    for (int32_t i = 0; i < world->city_count; i++)
    {
        dist[i] = -1;
        prev[i] = -1;
    }

    q[tail++] = entity->city_idx;
    dist[entity->city_idx] = 0;

    while (head < tail)
    {
        int32_t u = q[head++];

        for (int32_t v = 0; v < world->city_count; v++)
        {
            if (world->edge[u][v].exists && dist[v] == -1)
            {
                dist[v] = dist[u] + 1;
                prev[v] = u;
                q[tail++] = v;
            }
        }
    }

    if (dist[destination] == -1)
    {
        WARN("Nenhum caminho de %i para %i", entity->city_idx, destination);
        return;
    }

    int32_t curr = destination;
    entity->path_len = 0;
    while (curr != -1)
    {
        entity->path[entity->path_len++] = curr;
        curr = prev[curr];
    }

    for (int32_t i = 0; i < entity->path_len / 2; i++)
    {
        int32_t temp = entity->path[i];
        entity->path[i] = entity->path[entity->path_len - 1 - i];
        entity->path[entity->path_len - 1 - i] = temp;
    }

    EntityController_move_to(entity, entity->path[1]);
}

static void Entity_update(Entity *entity, World *world, float dt)
{
    if (entity->is_moving)
    {
        int32_t origin_x = world->cities[entity->city_idx].x,
                origin_y = world->cities[entity->city_idx].y,
                target_x = world->cities[entity->target_idx].x,
                target_y = world->cities[entity->target_idx].y;
#if MPI_MODE
        MPISendContext ctx = (MPISendContext){
            .origin_x = origin_x,
            .origin_y = origin_y,
            .target_x = target_x,
            .target_y = target_y,
            .x = entity->x,
            .y = entity->y,
            .speed = entity->speed,
            .city_idx = entity->city_idx,
            .target_idx = entity->target_idx,
            .is_moving = entity->is_moving,
            .progress = entity->progress,
            .path = {0},
            .path_len = entity->path_len,
            .dt = dt
        };

        for (int32_t i = 0; i < entity->path_len; i++)
        {
            ctx.path[i] = entity->path[i];
        }

        MPI_Send(&ctx, sizeof(MPISendContext), MPI_BYTE, 1, 0, MPI_COMM_WORLD);

        MPI_Recv(&ctx, sizeof(MPISendContext), MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        entity->x = ctx.x;
        entity->y = ctx.y;
        entity->city_idx = ctx.city_idx;
        entity->target_idx = ctx.target_idx;
        entity->is_moving = ctx.is_moving;
        entity->progress = ctx.progress;
        entity->path_len = ctx.path_len;
        for (int32_t i = 0; i < entity->path_len; i++)
        {
            entity->path[i] = ctx.path[i];
        }

        return;
#endif
        double distance = sqrt(pow(origin_x - target_x, 2) + pow(origin_y - target_y, 2));

        entity->progress += (entity->speed / distance) * dt;

        if (entity->progress >= 1)
        {
            entity->progress = 1;
            entity->is_moving = false;
            entity->city_idx = entity->target_idx;
            entity->target_idx = -1;

            if (entity->path_len > 0)
            {
                for (int32_t j = 0; j < entity->path_len - 1; j++)
                {
                    entity->path[j] = entity->path[j + 1];
                }
                entity->path_len--;
            }

            if (entity->path_len > 0)
            {
                EntityController_move_to(entity, entity->path[0]);
            }
            else
            {
                entity->load = 0;
                entity->is_loading = true;
            }

            return;
        }

        entity->x = (origin_x * (1 - entity->progress)) + (target_x * entity->progress);
        entity->y = (origin_y * (1 - entity->progress)) + (target_y * entity->progress);
    }
    else if (entity->is_loading)
    {
        entity->load += world->cities[entity->city_idx].production_rate * dt;

        if (entity->load > 1.)
        {
            entity->load = 1;
            entity->is_loading = false;
            int32_t destination = 0;
            do
            {
                destination = rand() % world->city_count;
            } while (destination == entity->city_idx);

            trace_route_to(world, entity, destination);
        }
    }
}