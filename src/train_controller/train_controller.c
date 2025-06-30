#include "train_controller.h"

static void reset_train(Train *train);
static void trace_route_to(World *world, Train *train, int32_t destination);
static void Train_update(Train *train, World *world, float dt);

TrainController *TrainController_new(int32_t train_count, World *world)
{
    TrainController *tc = (TrainController *)malloc(sizeof(TrainController));

    tc->train_count = train_count;
    tc->trains = (Train *)malloc(sizeof(Train) * train_count);
    tc->world = world;
    tc->is_displaying = true;

    for (int32_t i = 0; i < train_count; i++)
    {
        Train *train = &tc->trains[i];

        reset_train(train);

        train->speed = 100;
        train->color = (Color){.r = rand() % 256, .g = rand() % 256, .b = rand() % 256, .a = 255};
    }

    return tc;
}

void TrainController_free(TrainController *train_controller)
{
    free(train_controller->trains);
    free(train_controller);
}

void TrainController_update(TrainController *train_controller, double dt)
{
    for (int32_t i = 0; i < train_controller->train_count; i++)
    {
        Train *train = &train_controller->trains[i];

        Train_update(train, train_controller->world, dt);
    }
}

void *TrainController_thread_ctx(void *arg) {
    TrainController *ctx = (TrainController *)arg;

    static double last_time = 0.0;
    double current_time = get_time();
    float dt = (float)(current_time - last_time);

    while (!WindowShouldClose()) {
        last_time = current_time;
        current_time = get_time();
        dt = (float)(current_time - last_time);

        TrainController_update(ctx, dt);
    }

    return NULL;
}

void TrainController_draw(TrainController *train_controller)
{
    for (int32_t i = 0; i < train_controller->train_count; i++)
    {
        Train *train = &train_controller->trains[i];

        if (train_controller->is_displaying) {
            DrawRectangle(train->x - 4, train->y - 4, 8, 8, train->color);
        } else {
            DrawRectangle(train->x - 4, train->y - 4, 8, 8, (Color){
                .r = 255,
                .g = 0,
                .b = 0,
                .a = 100
            });
        }
    }
}

void TrainController_set_train_city(TrainController *train_controller, int32_t train_idx, int32_t city_idx)
{
    Train *train = &train_controller->trains[train_idx];
    City *city = &train_controller->world->cities[city_idx];

    reset_train(train);

    train->x = city->x;
    train->y = city->y;
    train->city_idx = city_idx;
    train->is_loading = true;
}

void TrainController_move_to(Train *train, int32_t target_city_idx)
{
    train->target_idx = target_city_idx;
    train->is_moving = true;
    train->progress = .0;
}

static void reset_train(Train *train)
{
    train->x = 0;
    train->y = 0;
    train->city_idx = -1;
    train->target_idx = -1;
    train->is_moving = false;
    train->progress = 0.;
    train->load = 0.;
    train->is_loading = false;
    train->path_len = 0;
}

static void trace_route_to(World *world, Train *train, int32_t destination)
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

    q[tail++] = train->city_idx;
    dist[train->city_idx] = 0;

    while (head < tail)
    {
        int32_t u = q[head++];

        for (int32_t v = 0; v < world->city_count; v++)
        {
            if (world->railway[u][v].distance > 0 && dist[v] == -1)
            {
                dist[v] = dist[u] + 1;
                prev[v] = u;
                q[tail++] = v;
            }
        }
    }

    if (dist[destination] == -1)
    {
        WARN("Nenhum caminho de %i para %i", train->city_idx, destination);
        return;
    }

    int32_t curr = destination;
    train->path_len = 0;
    while (curr != -1)
    {
        train->path[train->path_len++] = curr;
        curr = prev[curr];
    }

    for (int32_t i = 0; i < train->path_len / 2; i++)
    {
        int32_t temp = train->path[i];
        train->path[i] = train->path[train->path_len - 1 - i];
        train->path[train->path_len - 1 - i] = temp;
    }

    TrainController_move_to(train, train->path[1]);
}

static void Train_update(Train *train, World *world, float dt)
{
    if (train->is_moving)
    {
        int32_t origin_x = world->cities[train->city_idx].x,
                origin_y = world->cities[train->city_idx].y,
                target_x = world->cities[train->target_idx].x,
                target_y = world->cities[train->target_idx].y;
        double distance = sqrt(pow(origin_x - target_x, 2) + pow(origin_y - target_y, 2));

        train->progress += (train->speed / distance) * dt;

        if (train->progress >= 1)
        {
            train->progress = 1;
            train->is_moving = false;
            train->city_idx = train->target_idx;
            train->target_idx = -1;

            if (train->path_len > 0)
            {
                for (int32_t j = 0; j < train->path_len - 1; j++)
                {
                    train->path[j] = train->path[j + 1];
                }
                train->path_len--;
            }

            if (train->path_len > 0)
            {
                TrainController_move_to(train, train->path[0]);
            }
            else
            {
                train->load = 0;
                train->is_loading = true;
            }

            return;
        }

        train->x = (origin_x * (1 - train->progress)) + (target_x * train->progress);
        train->y = (origin_y * (1 - train->progress)) + (target_y * train->progress);
    }
    else if (train->is_loading)
    {
        train->load += world->cities[train->city_idx].production_rate * dt;

        if (train->load > 1.)
        {
            train->load = 1;
            train->is_loading = false;
            int32_t destination = 0;
            do
            {
                destination = rand() % world->city_count;
            } while (destination == train->city_idx);

            trace_route_to(world, train, destination);
        }
    }
}